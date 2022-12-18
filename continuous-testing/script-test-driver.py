#!/usr/bin/python3

import argparse
import json
import logging
import requests
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

lgr.setLevel(logging.INFO)

def test_script(script_hex: str, script_asm: str) -> None:
    test_cmd = [test_program, script_hex, script_asm]
    p = subprocess.Popen(test_cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = p.communicate()
    # stderr could contain some warning info, we will ignore them.
    retval = p.wait()
    if stdout.decode('utf8') == 'okay\n' and retval == 0:
        lgr.info(f'test program reports okay.')    
    else:
        err_msg = (
            f'Test program [{test_program}] reports error. stdout: {stdout}, stderr: {stderr}, retval: {retval}. '
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
    url_latestblock = 'https://blockchain.info/latestblock'
    lgr.info(f'Requesting latest block through [{url_latestblock}]')
    resp = requests.get(url_latestblock)
    latest_height = int(resp.json()['height'])
    lgr.info(f"Latest block fetched, it's height is {latest_height:,}")
    try:
        since_block = int(args['since-block'])
    except Exception:
        since_block = latest_height
    lgr.info(f'the test script will test since block {since_block:,} to {latest_height:,}')

    height = since_block
    retry, max_retry = 0, 30
    while height < latest_height:
        
        lgr.handlers.clear()
        formatter = logging.Formatter(
            fmt=f'%(asctime)s | %(levelname)7s | {height} | %(message)s', datefmt='%Y-%m-%d %H:%M'
        )
        file_handler = logging.FileHandler('/var/log/bitcoin-internals/script-test.log')
        file_handler.setFormatter(formatter)
        lgr.addHandler(file_handler)

        url_block_by_height = f'https://blockchain.info/block-height/{height}'
        lgr.info(f'Requesting transactions in the latest block through [{url_block_by_height}]')
        try:
            resp = requests.get(url_block_by_height)
            assert len(resp.json()['blocks']) > 0
            retry = 0
        except Exception:
            if retry > max_retry:
                raise RuntimeError(f'HTTP GETing {url_block_by_height} failed and max_retry exceeded')
            lgr.warning(f'0 blocks are fetched from {url_block_by_height}, will sleep() then retry...')
            time.sleep(30)
            retry += 1
            continue
        latest_block = resp.json()['blocks'][0]
        txes = latest_block['tx']
        lgr.info(f"All transactions fetched, count: {len(txes):,}")


        idx = 0
        while idx < len(txes):
            lgr.info(f'[{idx+1}/{len(txes)}] tx hash: {txes[idx]["hash"]}')
            tx_url = f'https://blockstream.info/api/tx/{txes[idx]["hash"]}'
            try:
                tx = requests.get(tx_url).json()
                retry = 0
            except Exception:
                if retry > max_retry:
                    raise RuntimeError(f'HTTP GETing {tx_url} failed and max_retry exceeded')
                lgr.warning('Failed to fetch tx, will sleep() then retry...')
                continue
            
            for j in range(len(tx['vin'])):
                tx_in = tx['vin'][j]
                if tx_in['scriptsig'] == '' and tx_in['scriptsig_asm'] == '':
                    lgr.info(f'[{idx+1}/{len(txes)}] {j}-th input script is empty, skipped')
                    continue
                lgr.info(f'[{idx+1}/{len(txes)}] testing {j}-th input...')
                try:
                    test_script(str(tx_in['scriptsig']), str(tx_in['scriptsig_asm']))
                except Exception as ex:
                    err_msg = f'testing {str(tx_in["scriptsig"])} (ASM: {str(tx_in["scriptsig_asm"])}),'
                    err_msg += f'height: {height}, transaction idx: {idx}: {ex}'
                    lgr.exception(err_msg)
                    raise RuntimeError(err_msg)
            for j in range(len(tx['vout'])):
                tx_out = tx['vout'][j]
                if tx_out['scriptpubkey'] == '' and tx_out['scriptpubkey_asm'] == '':
                    lgr.info(f'[{idx+1}/{len(txes)}] {j+1}-th output script is empty, skipped')
                    continue
                lgr.info(f'[{idx+1}/{len(txes)}] testing {j+1}-th output...')
                try:
                    test_script(str(tx_out['scriptpubkey']), str(tx_out['scriptpubkey_asm']))
                except Exception as ex:
                    err_msg = f'testing {str(tx_out["scriptpubkey"])} (ASM: {str(tx_out["scriptpubkey_asm"])}),'
                    err_msg += f'height: {height}, transaction idx: {idx}: {ex}'
                    lgr.exception(err_msg)
                    raise RuntimeError(err_msg)
            time.sleep(0.75)
            idx += 1
        height += 1

    lgr.info(f'All scripts are tested by [{test_program}] and no errors are reported')

if __name__ == '__main__':
    main()
