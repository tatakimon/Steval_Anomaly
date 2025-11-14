What we want long term

STWIN.box mounted on CNC:

sleeps most of the time,

wakes on vibration, logs DHCX accel+gyro to SD,

when machine stops, sends logged data via BLE to a Pi (or PC),

then sleeps again.

For now: we’re not doing real wake-on-vibration or SD yet. We are:

validating the state machine logic,

validating BLE data streaming,

and starting to optimize radio / battery behavior.

What we implemented in this firmware step-by-step

Custom BLE “USER DATA” characteristic

New service + notify characteristic (USER_DATA_Notify).

We can send arbitrary text lines (later binary) to a client (Python).

Simple run state machine

RS_SLEEP → RS_LOG → RS_XFER → RS_SLEEP → …

Currently:

RS_LOG = “mock logging” → fills a 10-line text buffer instead of real sensor data.

RS_XFER = sends those 10 lines over the new notify characteristic.

This runs even if no client is connected; when a client subscribes, it sees lines.

GATT quiet-window

On hci_le_connection_complete_event, we set:

user_gatt_ready = 0

user_conn_t0 = HAL_GetTick()

In MX_BLESensorsPnPL_Process() we wait ~2 seconds, then set:

user_gatt_ready = 1

The state machine only sends user data (USER_DATA_Notify) when user_gatt_ready == 1, to avoid hammering GATT right after connect.

Python side

Bleak script:

Scans for your device (BLEPnP),

Connects,

Subscribes to:

user data characteristic → prints your 10-line chunks,

battery characteristic → prints battery info.

It runs in a loop so it can reconnect if the board disappears and comes back.

Advertising duty-cycle (battery-ish behavior)

We added a radio state machine in MX_BLESensorsPnPL_Process():

USER_RADIO_ADV   (advertising ON)
USER_RADIO_SLEEP (advertising OFF)


Boot:

ST’s original set_connectable_ble() runs → ADV starts.

We set user_radio_state = USER_RADIO_ADV.

When not connected:

ADV for 5 seconds → call user_stop_advertising() → go to USER_RADIO_SLEEP.

Sleep (no advertising) for 60 seconds → call set_connectable_ble() → back to USER_RADIO_ADV.

LED:

Blink green only in USER_RADIO_ADV (when advertising),

Off in USER_RADIO_SLEEP or when connected.

Result:

Board is only discoverable during short ADV windows.

Your Python script continuously scans and will “catch” the board whenever one of these windows opens.

This is your “leave the device and still be able to check it later” pattern.

What’s next / things we’ve parked for later

Stuff we explicitly said “we’ll do this later”:

Replace mock logging with real DHCX at 1 kHz

Use DMA + binary ring buffer or at least raw BSP_MOTION_SENSOR_GetAxes sampling.

Decide final on-device format (likely binary frames) and JSONL conversion on the Pi.

Real SD logging

Instead of 10 fake lines in RAM, write real windows (e.g., 100 samples) to SD as binary.

After logging phase, re-read from SD and stream to Pi via BLE.

Wake-on-vibration and machine stop detection

Replace constant state cycling with:

Trigger interrupt from DHCX,

Detect machine stopped (low RMS level or no interrupts),

Only then enter RS_XFER.

More robust reconnect & edge cases

What happens if connection drops mid-XFER?

Do we resume where we left off or discard that window?

Add watchdog-style timeouts per phase.

For right now, blocking issues are:

Get user_stop_advertising() properly implemented in ble_manager.c.

Remove user_radio_state usage from ble_manager.c (or wire it correctly with extern).

Confirm that:

Board boots, advertises 5 s (LED blinking),

Then stops advertising 60 s (LED off),

Python auto-scan script can eventually connect whenever an ADV window opens,

After connect you see:

USER[RS]: LOG (mock 10 lines)

USER[RS]: XFER (notify 10 lines)

Data printing on Python side.

Once that loop is stable, we can safely start swapping the “mock 10 lines” with “real sensor + SD pipeline” without breaking BLE behavior every time.
