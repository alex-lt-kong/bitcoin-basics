#!/usr/bin/python3

from confluent_kafka import Producer
from Crypto.Cipher import AES
from Crypto.Util.Padding import pad

import argparse
import json
import logging
import requests
import socket
import subprocess
import time
import os

test_program = os.path.join(os.path.dirname(__file__), 'tx-test')
lgr = logging.getLogger()
#stream_handler = logging.StreamHandler(sys.stdout)
file_handler = logging.FileHandler('/var/log/bitcoin-internals/block-test.log')
file_handler.setFormatter(
    fmt=logging.Formatter('%(asctime)s | %(levelname)s | %(message)s',
    datefmt='%Y-%m-%d %H:%M'
))
lgr.addHandler(file_handler)
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


def send_to_kafka(block_metadata: object, status: str) -> None:

    def aes_encrypt(raw):
        key = os.getenv('LIBMYBITCOIN_KAFKA_32BYTE_ENC_KEY')
        if key is not None:
            raw = pad(raw.encode(),16)
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
    test_cmd = [test_program, script_hex, script_asm]
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
            f'To re-run: {test_program} "{script_hex}" "{script_asm}"'
        )
        lgr.error(err_msg)
        raise RuntimeError(err_msg)


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument(
        '--since-block', dest='since-block', default='',
        help=(
            'the test script will test since this block to the latest block, '
            'default to the latest block'
        )
    )
    args = vars(ap.parse_args())
    #lgr.info(f'Requesting latest block through [{url_latestblock}]')
    resp = requests.post(
        'http://127.0.0.1:8332/',
        auth=requests.auth.HTTPBasicAuth('rpcuser', 'rpcpassword'),
        json={
            "jsonrpc": "1.0",
            "method": "getbestblockhash",
            "params": []
        }
    )
    if resp.json()['result'] is None or  resp.json()['error'] is not None:
        raise RuntimeError(f'Failed to call bitcoind: {resp.text}')
    latest_block_hash = resp.json()['result']
    print(f'latest_block_hash: {latest_block_hash}')
    resp = requests.post(
        'http://127.0.0.1:8332/',
        auth=requests.auth.HTTPBasicAuth('rpcuser', 'rpcpassword'),
        json={
            "jsonrpc": "1.0",
            "method": "getblock",
            "params": [latest_block_hash]
        }
    )
    if resp.json()['result'] is None or  resp.json()['error'] is not None:
        raise RuntimeError(f'Failed to call bitcoind: {resp.text}')
    latest_height = resp.json()['result']['height']
    lgr.info(f"Latest block fetched, it's height is {latest_height:,}")
    print(f'latest_height: {latest_height}')
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
    while height <= latest_height:
        print(f'height: {height}')
        resp = requests.post(
            'http://127.0.0.1:8332/',
            auth=requests.auth.HTTPBasicAuth('rpcuser', 'rpcpassword'),
            json={
                "jsonrpc": "1.0",
                "method": "getblockhash",
                "params": [height]
            }
        )
        if resp.json()['result'] is None or  resp.json()['error'] is not None:
            raise RuntimeError(f'Failed to call bitcoind: {resp.text}')
        block_hash = resp.json()['result']
        print(f'block_hash: {block_hash}')
        resp = requests.post(
            'http://127.0.0.1:8332/',
            auth=requests.auth.HTTPBasicAuth('rpcuser', 'rpcpassword'),
            json={
                "jsonrpc": "1.0",
                "method": "getblock",
                "params": [block_hash, 2]
            }
        )
        if resp.json()['result'] is None or  resp.json()['error'] is not None:
            raise RuntimeError(f'Failed to call bitcoind: {resp.text}')
        block_data = resp.json()['result']
        for i in range(block_data['nTx']):
            tx = block_data['tx'][i]
            # print(f'tx: {json.dumps(tx, indent=4)}')
            test_cmd = [test_program, tx['hex'], 'get_tx_in_count']
            p = subprocess.Popen(
                test_cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE
            )
            stdout, stderr = p.communicate()
            retval = p.wait()
            actual_tx_in_count = int(stdout.decode("utf8"))
            expect_tx_in_count = len(tx["vin"])
            if actual_tx_in_count != expect_tx_in_count:
                raise ValueError(
                    f'actual tx_in_count: {actual_tx_in_count}\n'
                    f'expect tx_in_count: {expect_tx_in_count}\n'
                )
                
            
            test_cmd = [test_program, tx['hex'], 'get_tx_out_count']
            p = subprocess.Popen(
                test_cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE
            )
            stdout, stderr = p.communicate()
            retval = p.wait()
            actual_tx_out_count = int(stdout.decode("utf8"))
            expect_tx_out_count = len(tx["vout"])
            assert actual_tx_out_count == expect_tx_out_count
        height += 1



    lgr.info(f'All scripts are tested by [{test_program}] and no errors are reported')


if __name__ == '__main__':
    main()
