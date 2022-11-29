import logging
import requests
import sys
import time


lgr = logging.getLogger()
handler = logging.StreamHandler(sys.stdout)
handler.setFormatter(
    fmt=logging.Formatter('%(asctime)s | %(name)s | %(levelname)s | %(message)s',
    datefmt='%Y-%m-%d %H:%M:%S'
))
lgr.addHandler(handler)
lgr.setLevel(logging.INFO)

url_latestblock = 'https://blockchain.info/latestblock'
lgr.info(f'Requesting latest block through [{url_latestblock}]')
resp = requests.get(url_latestblock)
height = int(resp.json()['height'])
lgr.info(f"Latest block fetched, it's height is {height:,}")

url_block_by_height = f'https://blockchain.info/block-height/{height}'
lgr.info(f'Requesting transactions in the latest block through [{url_block_by_height}]')
resp = requests.get(url_block_by_height)
latest_block = resp.json()['blocks'][0]
txes = latest_block['tx']
lgr.info(f"All transactions fetched, count: {len(txes):,}")

for i in range(len(txes)):
    lgr.info(f'[{i+1:,}/{len(txes):,}] tx hash: {txes[i]["hash"]}')
    resp = requests.get(f'https://blockstream.info/api/tx/{txes[i]["hash"]}/hex')
    lgr.info(f'[{i+1:,}/{len(txes):,}] tx bytes: {resp.text}')
    time.sleep(1)    

