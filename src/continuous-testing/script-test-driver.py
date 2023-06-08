#!/usr/bin/python3

import typing
from confluent_kafka import Producer
from Crypto.Cipher import AES
from Crypto.Util.Padding import pad

import argparse
import json
import logging
import requests
import socket
import subprocess
import sys
import time
import os

test_program = os.path.join(os.path.dirname(__file__), 'script-test')
state_file_path = os.path.join(
    os.path.expanduser("~/.config/"), f'{os.path.basename(__file__)}.json'
)
lgr = logging.getLogger()
handler = logging.StreamHandler(sys.stdout)
# handler = logging.FileHandler('/var/log/libmybitcoin/script-test.log')
handler.setFormatter(
    fmt=logging.Formatter('%(asctime)s | %(levelname)7s | %(message)s',
                          datefmt='%Y-%m-%d %H:%M'
                          ))
lgr.addHandler(handler)
lgr.setLevel(logging.INFO)

if (
    os.getenv('LIBMYBITCOIN_KAFKA_BROKERS') is not None and
    os.getenv('LIBMYBITCOIN_KAFKA_TOPIC') is not None
):
    producer = Producer({
        'bootstrap.servers': os.getenv('LIBMYBITCOIN_KAFKA_BROKERS'),
        'client.id': socket.getfqdn()
    })
else:
    lgr.warning('kafka-related environment variables not set, will disable it')
    producer = None


def kafka_cb(err, msg):
    if err is not None:
        lgr.error(f'Failed to deliver message [{msg}], reason: {err}')


def send_to_kafka(
    block_metadata: typing.Dict[str, typing.Any], status: str
) -> None:

    def aes_encrypt(raw):
        key = os.getenv('LIBMYBITCOIN_KAFKA_32BYTE_ENC_KEY')
        if key is not None:
            raw = pad(raw.encode(), 16)
            cipher = AES.new(key.encode('utf-8'), AES.MODE_ECB)
            return cipher.encrypt(raw)
        else:
            lgr.warning('AES encryption is not enabled')
            return raw

    if producer is None:
        return
    producer.produce(
        os.getenv('LIBMYBITCOIN_KAFKA_TOPIC'),
        value=aes_encrypt(json.dumps({
            'host': socket.getfqdn(),
            'unix_ts': time.time(),
            'block_height': block_metadata['height'],
            'block_ts': block_metadata['timestamp'],
            'tx_count': block_metadata['tx_count'],
            'status': status
        })),
        callback=kafka_cb
    )
    producer.poll(timeout=1)


def test_script(script_hex: str, script_asm: str) -> None:
    test_cmd = [
        test_program, '--script-hex', script_hex, '--script-asm', script_asm
    ]
    p = subprocess.Popen(
        test_cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE
    )
    stdout, stderr = p.communicate()
    # stderr could contain some warning info, we will ignore them.
    retval = p.wait()
    if stderr is not None and stderr.decode('utf8') != '':
        # stderr is considered information only.
        lgr.warning(f'stderr from test program: {stderr}')
    if stdout.decode('utf8') != 'okay\n' and retval != 0:
        err_msg = (
            f'Test program [{test_program}] reports error. stdout: '
            f'{stdout.decode("utf8")}, stderr: {stderr.decode("utf8")}, '
            f'retval: {retval}. '
            f'To re-run: {test_program} '
            f'--script-hex "{script_hex}"  --script-asm "{script_asm}"'
        )
        lgr.error(err_msg)
        raise RuntimeError(err_msg)


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
        try:
            with open(state_file_path) as f:
                since_block_height = json.load(f)['since_block']
            if latest_height - since_block_height < 5:
                since_block_height = 0
                lgr.warn(
                    f'since_block_height ({since_block_height}) is very close '
                    f'to latest_height ({latest_height}), will start from 0 '
                    'again'
                )

        except Exception:
            since_block_height = 0
    lgr.info(
        f'the test script will test since block {since_block_height:,} '
        f'to {latest_height:,}'
    )

    height = since_block_height
    retry, max_retry = 0, 30
    while height <= latest_height:

        url_block_by_height = f'https://blockstream.info/api/block-height/{height}'
        block_metadata = None
        lgr.info(
            f'Requesting transactions in the block {height} '
            f'through [{url_block_by_height}]'
        )
        block_hash = ''
        try:
            resp = requests.get(url_block_by_height)
            block_hash = resp.text
            lgr.info(f'block_id fetched: {block_hash}')
            url_block_metadata = f'https://blockstream.info/api/block/{block_hash}'
            lgr.info(
                f'HTTP GETing block metadata through {url_block_metadata}')
            resp = requests.get(url_block_metadata)
            block_metadata = resp.json()
            retry = 0
        except Exception:
            if retry > max_retry:
                err_msg = f'HTTP GETing {url_block_by_height} failed and max_retry ({max_retry}) exceeded'
                send_to_kafka(
                    {'height': -1, 'timestamp': -1, 'tx_count': -1}, err_msg
                )
                raise RuntimeError(err_msg)
            lgr.warning(
                f'0 blocks are fetched from {url_block_by_height}, will sleep() then retry...'
            )
            time.sleep(30)
            retry += 1
            continue
        tx_count = block_metadata['tx_count']
        lgr.info(f"block_metadata fetched, tx_count: {tx_count}")
        lgr.handlers.clear()
        formatter = logging.Formatter(
            fmt=f'%(asctime)s | %(levelname)7s | {height},{tx_count} | %(message)s',
            datefmt='%Y-%m-%d %H:%M'
        )
        handler = logging.StreamHandler(sys.stdout)
        # handler = logging.FileHandler('/var/log/libmybitcoin/script-test.log')
        handler.setFormatter(formatter)
        lgr.addHandler(handler)

        total_idx = 0

        while total_idx < tx_count:
            paged_txes = None
            paged_tx_url = f'https://blockstream.info/api/block/{block_hash}/txs/{total_idx}'

            try:
                lgr.info(
                    f'HTTP GETing tx [{total_idx}, {total_idx+25}]through {paged_tx_url}'
                )
                resp = requests.get(paged_tx_url, timeout=600)
                paged_txes = resp.json()
                lgr.info(f"tx GET'ed")
                retry = 0
            except Exception:
                if retry > max_retry:
                    err_msg = f'HTTP GETing {paged_tx_url} failed and max_retry exceeded'
                    send_to_kafka(block_metadata, err_msg)
                    raise RuntimeError()
                lgr.warning(
                    'Failed to fetch paged_txes, will sleep() then retry...')
                time.sleep(30)
                continue
            for i in range(len(paged_txes)):
                tx = paged_txes[i]
                for j in range(len(tx['vin'])):
                    tx_in = tx['vin'][j]
                    if tx_in['scriptsig'] == '' and tx_in['scriptsig_asm'] == '':
                        lgr.debug(
                            f'[{total_idx+i}] {j}-th input script is empty, skipped')
                        continue
                    lgr.debug(f'[{total_idx+i}] testing {j}-th input...')
                    try:
                        test_script(str(tx_in['scriptsig']), str(
                            tx_in['scriptsig_asm']))
                        lgr.info(
                            f'{j + 1:02}-th in  of {total_idx + i:,}-th tx: '
                            f'test program reports okay.'
                        )
                    except Exception as ex:
                        err_msg = f'testing {str(tx_in["scriptsig"])} (ASM: {str(tx_in["scriptsig_asm"])}),'
                        err_msg += f'height: {height}, transaction idx: {total_idx+i}: {ex}'
                        send_to_kafka(block_metadata, err_msg)
                        lgr.exception(err_msg)
                        raise RuntimeError(err_msg)
                for j in range(len(tx['vout'])):
                    tx_out = tx['vout'][j]
                    if tx_out['scriptpubkey'] == '' and tx_out['scriptpubkey_asm'] == '':
                        lgr.debug(
                            f'[{total_idx+i}] {j+1}-th output script is empty, skipped')
                        continue
                    lgr.debug(f'[{total_idx+i}] testing {j+1}-th output...')
                    try:
                        test_script(str(tx_out['scriptpubkey']), str(
                            tx_out['scriptpubkey_asm']))
                        lgr.info(
                            f'{j + 1:02}-th out of {total_idx + i:,}-th tx: test program reports okay.')
                    except Exception as ex:
                        err_msg = f'testing {str(tx_out["scriptpubkey"])} (ASM: {str(tx_out["scriptpubkey_asm"])}),'
                        err_msg += f'height: {height}, transaction idx: {total_idx+i}: {ex}'
                        send_to_kafka(block_metadata, err_msg)
                        lgr.exception(err_msg)
                        raise RuntimeError(err_msg)

            total_idx += 25
        send_to_kafka(block_metadata, 'okay')
        with open(state_file_path, 'w') as f:
            json.dump({'since_block': height}, f)
        height += 1

    lgr.info(
        f'All scripts are tested by [{test_program}] and no errors are reported')


if __name__ == '__main__':
    main()
