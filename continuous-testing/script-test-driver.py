#!/usr/bin/python3

import json
import logging
import requests
import subprocess
import os

test_program = os.path.join(os.path.dirname(__file__), 'script-test.out')
lgr = logging.getLogger()
#stream_handler = logging.StreamHandler(sys.stdout)
file_handler = logging.FileHandler('/var/log/bitcoin-internals/script-test.log')
file_handler.setFormatter(
    fmt=logging.Formatter('%(asctime)s | %(name)s | %(levelname)s | %(message)s',
    datefmt='%Y-%m-%d %H:%M:%S'
))
lgr.addHandler(file_handler)
lgr.setLevel(logging.INFO)

def test_script(script_hex: str, script_asm: str) -> None:
    test_cmd = [test_program, script_hex, script_asm]
    p = subprocess.Popen(test_cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = p.communicate()
    # stderr could contain some warning info, we will ignore them.
    retval = p.wait()
    if stdout.decode('utf8') == 'okay\n' and retval == 0:
        lgr.info(f'test program [{test_program}] reports okay.')
    else:
        err_msg = (
            f'Test program [{test_program}] reports error. stdout: {stdout}, stderr: {stderr}, retval: {retval}. '
            f'To re-run: {test_program} "{script_hex}" "{script_asm}"'
        )
        lgr.error(err_msg)
        raise ValueError(err_msg)


def main() -> None:
    url_latestblock = 'https://blockchain.info/latestblock'
    lgr.info(f'Requesting latest block through [{url_latestblock}]')
    resp = requests.get(url_latestblock)
    height = int(resp.json()['height'])
    lgr.info(f"Latest block fetched, it's height is {height:,}")

    url_block_by_height = f'https://blockchain.info/block-height/{height}'
    lgr.info(f'Requesting transactions in the latest block through [{url_block_by_height}]')
    resp = requests.get(url_block_by_height)
    if len(resp.json()['blocks']) == 0:
        raise ValueError(f'0 blocks are fetched from {url_block_by_height}. The content is: {resp.json()}')
    latest_block = resp.json()['blocks'][0]
    txes = latest_block['tx']
    lgr.info(f"All transactions fetched, count: {len(txes):,}")


    for i in range(len(txes)):
        lgr.info(f'[{i+1}/{len(txes)}/{height}] tx hash: {txes[i]["hash"]}')
        try:
            tx = requests.get(f'https://blockstream.info/api/tx/{txes[i]["hash"]}').json()
        except Exception:
            lgr.exception('Failed to fetch tx, this record will be skipped')
            continue
        for j in range(len(tx['vin'])):
            tx_in = tx['vin'][j]
            if tx_in['scriptsig'] == '' and tx_in['scriptsig_asm'] == '':
                lgr.info(f'[{i+1}/{len(txes)}/{height}] {j}-th input script is empty, skipped')
                continue
            lgr.info(f'[{i+1}/{len(txes)}/{height}] testing {j}-th input...')
            test_script(str(tx_in['scriptsig']), str(tx_in['scriptsig_asm']))
        for j in range(len(tx['vout'])):
            tx_out = tx['vout'][j]
            if tx_out['scriptpubkey'] == '' and tx_out['scriptpubkey_asm'] == '':
                lgr.info(f'[{i+1}/{len(txes)}/{height}] {j+1}-th output script is empty, skipped')
                continue
            lgr.info(f'[{i+1}/{len(txes)}/{height}] testing {j+1}-th output...')
            test_script(str(tx_out['scriptpubkey']), str(tx_out['scriptpubkey_asm']))

    lgr.info(f'All scripts are tested by [{test_program}] and no errors are reported')

if __name__ == '__main__':
    main()
