#!/usr/bin/python3

from base64 import decode
import logging
import requests
import subprocess
import os

lgr = logging.getLogger()
#stream_handler = logging.StreamHandler(sys.stdout)
file_handler = logging.FileHandler('/var/log/bitcoin-internals/script-test.log')
file_handler.setFormatter(
    fmt=logging.Formatter('%(asctime)s | %(name)s | %(levelname)s | %(message)s',
    datefmt='%Y-%m-%d %H:%M:%S'
))
lgr.addHandler(file_handler)
lgr.setLevel(logging.INFO)

url_latestblock = 'https://blockchain.info/latestblock'
lgr.info(f'Requesting latest block through [{url_latestblock}]')
resp = requests.get(url_latestblock)
height = int(resp.json()['height'])
lgr.info(f"Latest block fetched, it's height is {height:,}")

url_block_by_height = f'https://blockchain.info/block-height/{height}'
lgr.info(f'Requesting transactions in the latest block through [{url_block_by_height}]')
resp = requests.get(url_block_by_height)
if len(resp.json()['blocks']) == 0:
    raise ValueError(f'0 blocks are fetched from {url_block_by_height}')
latest_block = resp.json()['blocks'][0]
txes = latest_block['tx']
lgr.info(f"All transactions fetched, count: {len(txes):,}")

test_program = os.path.join(os.path.dirname(__file__), 'script-test.out')
for i in range(len(txes)):
    lgr.info(f'[{i+1}/{len(txes)}] tx hash: {txes[i]["hash"]}')
    # resp = requests.get(f'https://blockstream.info/api/tx/{txes[i]["hash"]}/hex')
    #slgr.info(f'[{i+1:,}/{len(txes):,}] tx bytes: {resp.text}')
    for j in range(len(txes[i]['inputs'])):
        tx_in = txes[i]['inputs'][j]
        if tx_in['script'] == '':
            lgr.info(f'[{i+1}/{len(txes)}] script is empty, skipped')
            continue
        test_cmd = [test_program, str(tx_in['script'])]        
        p = subprocess.Popen(test_cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        stdout, stderr = p.communicate()
        # stderr could contain some warning info, we will ignore them.
        retval = p.wait()
        if stdout.decode('utf8') == 'okay\n' and retval == 0:
            lgr.info(f'[{i+1}/{len(txes)}] {j+1}th input: test program [{test_program}] reports okay.')
        else:
            raise ValueError(
                f'Test program [{test_program}] reports error. stdout: {stdout}, stderr: {stderr}, retval: {retval}. '
                f'txes["height"]: {latest_block["height"]}, index: {i}, tx_in["script"]: {tx_in}'
            )

lgr.info(f'All scripts are tested by [{test_program}] and no errors are reported')