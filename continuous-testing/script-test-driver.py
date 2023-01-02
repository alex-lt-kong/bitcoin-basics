#!/usr/bin/python3

from confluent_kafka import Producer

import argparse
import json
import logging
import requests
import socket
import subprocess
import time
import os

test_program = os.path.join(os.path.dirname(__file__), 'script-test.out')
lgr = logging.getLogger()
#stream_handler = logging.StreamHandler(sys.stdout)
file_handler = logging.FileHandler('/var/log/bitcoin-internals/script-test.log')
file_handler.setFormatter(
    fmt=logging.Formatter('%(asctime)s | %(levelname)s | %(message)s',
    datefmt='%Y-%m-%d %H:%M'
))
lgr.addHandler(file_handler)
lgr.setLevel(logging.INFO)

assert os.getenv('BITCOIN_INTERNALS_KAFKA_BROKERS') is not None
assert os.getenv('BITCOIN_INTERNALS_KAFKA_TOPIC') is not None

producer = Producer({
    'bootstrap.servers': os.getenv('BITCOIN_INTERNALS_KAFKA_BROKERS'),
    'client.id': socket.getfqdn()
})

def kafka_cb(err, msg):
    if err is not None:
        lgr.error(f'Failed to deliver message [{msg}], reason: {err}')


def send_to_kafka(block_height: int, status: str) -> None:
    producer.produce(
        os.getenv('BITCOIN_INTERNALS_KAFKA_TOPIC'),
        value=json.dumps({
            'host': socket.getfqdn(),
            'unix_ts': time.time(),
            'block_height': block_height,
            'status': status
        }),
        callback=kafka_cb
    )
    producer.poll(timeout=1)


def test_script(script_hex: str, script_asm: str) -> None:
    test_cmd = [test_program, script_hex, script_asm]
    p = subprocess.Popen(
        test_cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE
    )
    stdout, stderr = p.communicate()
    # stderr could contain some warning info, we will ignore them.
    retval = p.wait()
    if stdout.decode('utf8') == 'okay\n' and retval == 0:
        lgr.info(f'test program reports okay.')    
    else:
        err_msg = (
            f'Test program [{test_program}] reports error. stdout: '
            f'{stdout.decode("utf8")}, stderr: {stderr.decode("utf8")}, '
            f'retval: {retval}. '
            f'To re-run: {test_program} "{script_hex}" "{script_asm}"'
        )
        lgr.error(err_msg)
        raise RuntimeError(err_msg)
    if stderr is not None and stderr.decode('utf8') != '':
        lgr.warning(f'stderr from test program: {stderr}')


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument(
        '--since-block', dest='since-block', default='',
        help='the test script will test since this block to the latest block'       
    )
    args = vars(ap.parse_args())
    url_latestblock = 'https://blockstream.info/api/blocks'
    lgr.info(f'Requesting latest block through [{url_latestblock}]')
    resp = requests.get(url_latestblock)
    latest_block = resp.json()[0]
    latest_height = latest_block['height']
    lgr.info(f"Latest block fetched, it's height is {latest_height:,}")
    try:
        since_block_height = int(args['since-block'])
    except Exception:
        since_block_height = latest_height
    lgr.info(
        f'the test script will test since block {since_block_height:,} '
        f'to {latest_height:,}'
    )

    height = since_block_height
    retry, max_retry = 0, 30
    while height < latest_height:

        url_block_by_height = f'https://blockstream.info/api/block-height/{height}'        
        block_metadata = None
        lgr.info(
            f'Requesting transactions in the block {height} '
            f'through [{url_block_by_height}]'
        )
        block_id = ''
        try:
            resp = requests.get(url_block_by_height)
            block_id = resp.text
            lgr.info(f'block_id fetched: {block_id}')
            url_block_metadata = f'https://blockstream.info/api/block/{block_id}'
            lgr.info(f'HTTP GETing block metadata through {url_block_metadata}')
            resp = requests.get(url_block_metadata)
            block_metadata = resp.json()
            retry = 0
        except Exception:
            if retry > max_retry:
                err_msg = f'HTTP GETing {url_block_by_height} failed and max_retry ({max_retry}) exceeded'
                send_to_kafka(height, err_msg)
                raise RuntimeError(err_msg)
            lgr.warning(f'0 blocks are fetched from {url_block_by_height}, will sleep() then retry...')
            time.sleep(30)
            retry += 1
            continue
        tx_count = block_metadata['tx_count']
        lgr.info(f"block_metadata fetched, tx_count: {tx_count}")
        lgr.handlers.clear()
        formatter = logging.Formatter(
            fmt=f'%(asctime)s | %(levelname)7s | {height},{tx_count} | %(message)s', datefmt='%Y-%m-%d %H:%M'
        )
        file_handler = logging.FileHandler('/var/log/bitcoin-internals/script-test.log')
        file_handler.setFormatter(formatter)
        lgr.addHandler(file_handler)

        total_idx = 0
        
        while total_idx < tx_count:
            paged_txes = None
            paged_tx_url = f'https://blockstream.info/api/block/{block_id}/txs/{total_idx}'

            try:                
                lgr.info(f'HTTP GETing tx [{total_idx}, {total_idx+25}]through {paged_tx_url}')
                resp = requests.get(paged_tx_url, timeout=600)
                paged_txes = resp.json()
                lgr.info(f"tx GET'ed")
                retry = 0
            except Exception:
                if retry > max_retry:
                    err_msg = f'HTTP GETing {paged_tx_url} failed and max_retry exceeded'
                    send_to_kafka(height, err_msg)
                    raise RuntimeError()
                lgr.warning('Failed to fetch paged_txes, will sleep() then retry...')
                time.sleep(30)
                continue
            for i in range(len(paged_txes)):
                tx = paged_txes[i]
                for j in range(len(tx['vin'])):
                    tx_in = tx['vin'][j]
                    if tx_in['scriptsig'] == '' and tx_in['scriptsig_asm'] == '':
                        lgr.info(f'[{total_idx+i}] {j}-th input script is empty, skipped')
                        continue
                    lgr.info(f'[{total_idx+i}] testing {j}-th input...')
                    try:
                        test_script(str(tx_in['scriptsig']), str(tx_in['scriptsig_asm']))
                    except Exception as ex:
                        err_msg = f'testing {str(tx_in["scriptsig"])} (ASM: {str(tx_in["scriptsig_asm"])}),'
                        err_msg += f'height: {height}, transaction idx: {total_idx+i}: {ex}'
                        send_to_kafka(height, err_msg)
                        lgr.exception(err_msg)
                        raise RuntimeError(err_msg)
                for j in range(len(tx['vout'])):
                    tx_out = tx['vout'][j]
                    if tx_out['scriptpubkey'] == '' and tx_out['scriptpubkey_asm'] == '':
                        lgr.info(f'[{total_idx+i}] {j+1}-th output script is empty, skipped')
                        continue
                    lgr.info(f'[{total_idx+i}] testing {j+1}-th output...')
                    try:
                        test_script(str(tx_out['scriptpubkey']), str(tx_out['scriptpubkey_asm']))
                    except Exception as ex:
                        err_msg = f'testing {str(tx_out["scriptpubkey"])} (ASM: {str(tx_out["scriptpubkey_asm"])}),'
                        err_msg += f'height: {height}, transaction idx: {total_idx+i}: {ex}'
                        send_to_kafka(height, err_msg)
                        lgr.exception(err_msg)
                        raise RuntimeError(err_msg)
                
            total_idx += 25
        send_to_kafka(height, 'okay')            
        height += 1

    lgr.info(f'All scripts are tested by [{test_program}] and no errors are reported')

if __name__ == '__main__':
    main()
