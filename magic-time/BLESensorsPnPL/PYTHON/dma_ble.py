# dhcx_pnpl_windows.py
# pip install bleak

# dma_ble_windows.py
# pip install bleak

import asyncio
import platform
import struct
import json
import os
import time
from bleak import BleakClient, BleakScanner, BleakError

NAME = "BLEPnP"

# PNPL characteristic UUID – use the one you're already using for PNPL
# (Where you previously saw 'DHCXTEST' and 'PNPL first sample' arrive.)
PNPL_CHAR = "0000001b-0002-11e1-ac36-0002a5d5c51b"  # fill in
ADDR = "C1:D1:8C:61:A5:20"  # <- from UART
# Global file handle so the callback can write into it
jsonl_file = None
current_window = 0
current_sample = 0

FILE_PATH = "dhcx_windows.jsonl"


def handle_pnpl(_handle, data: bytearray, f):
        # 12 bytes = 6 little-endian int16
    if len(data) != 12:
        print("[PNPL] unexpected len:", len(data))
        return

    import struct
    gx, gy, gz, ax, ay, az = struct.unpack("<hhhhhh", data)

    rec = {
        "gx": gx, "gy": gy, "gz": gz,
        "ax": ax, "ay": ay, "az": az,
    }
    f.write(json.dumps(rec) + "\n")
    f.flush()

    print(f"[PNPL] G=({gx:6d},{gy:6d},{gz:6d}) A=({ax:6d},{ay:6d},{az:6d})")


async def wait_for_device(name: str):
    while True:
        print("[SCAN] Scanning for 30 s...")
        dev = await BleakScanner.find_device_by_name(name, timeout=30.0)
        if dev:
            print(f"[SCAN] Found {dev.name} @ {dev.address}")
            return dev
        print("[SCAN] Not found, retrying...")


async def run_session():
    dev = await wait_for_device(NAME)

    kwargs = {}
    if platform.system() == "Windows":
        kwargs["winrt"] = {"use_cached_services": True}

    try:
        async with BleakClient(dev, timeout=45, **kwargs) as client:
            
            print("[INFO] Connected:", client.is_connected)

            with open(FILE_PATH, "a", encoding="utf-8") as f:
                await client.start_notify(
                    PNPL_CHAR,
                    lambda h, d: handle_pnpl(h, d, f),
                )
                print("[INFO] Subscribed to PNPL")
                print("[INFO] Listening for samples/windows...")

                while True:
                    await asyncio.sleep(1.0)

    except BleakError as e:
        print("[ERROR] BLE error:", e)
    except Exception as e:
        print("[ERROR] Unexpected:", e)


async def main():
    while True:
        await run_session()
        print("[INFO] Session ended. Reconnecting in 2 s...")
        await asyncio.sleep(2.0)


if __name__ == "__main__":
    asyncio.run(main())

