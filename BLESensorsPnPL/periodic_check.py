# auto_reconnect_data_and_battery.py
# pip install bleak
import asyncio, platform, json
from bleak import BleakClient, BleakScanner, BleakError

NAME      = "BLEPnP"
USER_CHAR = "f0de0001-d25a-7f9a-b348-4e7c23d6a13b"

BAT_CHAR =  "00020000-0001-11e1-ac36-0002a5d5c51b"

def handle_user_data(_handle, data: bytearray):
    try:
        line = data.decode(errors="ignore").rstrip()
    except Exception:
        line = "RAW:" + data.hex()
    print("[DATA]", line)


def estimate_pct_from_voltage(voltage_mV: int) -> int:
    """
    Roughly match ST app % using a linear fit:
    - 3.382 V -> 13 %
    - 3.775 V -> 53 %
    """
    V = voltage_mV / 1000.0

    # coefficients fitted from your two points
    a = 101.78
    b = -331.22
    soc = a * V + b

    # clamp to [0, 100]
    if soc < 0:
        soc = 0
    if soc > 100:
        soc = 100

    return int(round(soc))

def parse_battery(data: bytes):
    # 9-byte STWIN battery char:
    #  [0]   : flags
    #  [1]   : raw level byte (not trusted)
    #  [2:4] : reserved / unknown
    #  [4:6] : voltage (uint16, mV)
    #  [6:8] : current (uint16, mA or 0x8000 = N/A)
    #  [8]   : status (uint8)
    if len(data) != 9:
        return {"raw_hex": data.hex()}

    flags      = data[0]
    raw_level  = data[1]
    voltage_mV = int.from_bytes(data[4:6], "little", signed=False)
    current_raw = int.from_bytes(data[6:8], "little", signed=False)
    status_byte = data[8]

    if current_raw == 0x8000:
        current_mA = None
    else:
        current_mA = current_raw

    status_map = {
        0x00: "Low",
        0x01: "Discharging",
        0x02: "NotPresent",
        0x03: "Charging",
        0x04: "Full",
    }
    status_str = status_map.get(status_byte, f"Unknown(0x{status_byte:02X})")

    est_pct = estimate_pct_from_voltage(voltage_mV)

    return {
        "raw_level_byte": raw_level,        # what the firmware sends
        "level_pct_est": est_pct,           # our voltage-based estimate
        "voltage_mV": voltage_mV,
        "voltage_V": voltage_mV / 1000.0,
        "current_mA": current_mA,
        "status": status_str,
        "flags": flags,
    }


def handle_battery(sender, data: bytes):
    info = parse_battery(data)
    if "raw_hex" in info:
        print("[BAT raw]", info["raw_hex"])
        return

    pct    = info["level_pct_est"]
    raw    = info["raw_level_byte"]
    v      = info["voltage_V"]
    status = info["status"]

    if info["current_mA"] is None:
        cur_str = "current: N/A"
    else:
        cur_str = f"current: {info['current_mA']} mA"

    print(f"[BAT] ~{pct}% (raw={raw}) | {status} | {v:.3f} V | {cur_str}")


async def wait_for_device(name: str):
    """
    Continuous scanning until the device appears.
    Works even if the board is advertising only 5s every 60s.
    """
    while True:
        print("[SCAN] Scanning for 30 s...")
        dev = await BleakScanner.find_device_by_name(name, timeout=30.0)
        if dev:
            print(f"[SCAN] Found {dev.name} @ {dev.address}")
            return dev
        print("[SCAN] Not found, scanning again...")







async def run_session():
    dev = await wait_for_device(NAME)

    kwargs = {}
    if platform.system() == "Windows":
        kwargs["winrt"] = {"use_cached_services": True}

    def on_disconnect(_client):
        print("[INFO] Disconnected callback fired.")

    try:
        async with BleakClient(dev, timeout=45, **kwargs) as client:
            client.set_disconnected_callback(on_disconnect)
            print("[INFO] Connected:", client.is_connected)

            # Quiet window in your FW
            await asyncio.sleep(2.0)

            await client.start_notify(USER_CHAR, handle_user_data)
            print("[INFO] Subscribed to USER DATA")

            try:
                await client.start_notify(BAT_CHAR, handle_battery)
                print("\r\n[INFO] Subscribed to BATTERY\r\n")
            except Exception as e:
                print("[WARN] Could not subscribe to BATTERY:", e)

            print("[INFO] Listening… (if link drops, we'll reconnect)")

            while True:
                await asyncio.sleep(1.0)

    except BleakError as e:
        print("[ERROR] BLE error:", e)
    except Exception as e:
        print("[ERROR] Unexpected:", e)


async def main():
    while True:
        await run_session()
        print("[INFO] Session ended. Reconnecting...")
        # optional small delay if you want
        await asyncio.sleep(2.0)


if __name__ == "__main__":
    asyncio.run(main())
