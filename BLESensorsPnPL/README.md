PROJECT CONTEXT (STWIN.box BLE + logging)

Board & base FW:
- Board: STEVAL-STWINBX1 (STWIN.box).
- Base firmware: FP-SNS-STBOX1 "BLESensorsPnPL" example.
- main.c loop just calls MX_BLESensorsPnPL_Process().

What we have already modified:

1) Custom USER DATA service/characteristic
   - Implemented in ble_manager.c / app_blesensorspnpl.c.
   - 128-bit UUID for characteristic (notify-only):
       USER_CHAR = "f0de0001-d25a-7f9a-b348-4e7c23d6a13b"
   - Service + char created with aci_gatt_add_service / aci_gatt_add_char.
   - We track:
       uint16_t userDataSvcHandle;
       uint16_t userDataCharHandle;
       volatile uint8_t user_data_cccd;  // CCCD notify enabled
   - In aci_gatt_attribute_modified_event() we detect CCCD changes for userDataCharHandle
     and set user_data_cccd = 1/0 and print "USER: DATA notify ENABLED/DISABLED".

2) Run-state state machine in app_blesensorspnpl.c
   - States:
       typedef enum { RS_SLEEP = 0, RS_LOG, RS_XFER } run_state_t;
       static run_state_t g_run_state;
       static uint8_t user_line_idx, user_line_count;
   - USER_RS_Enter() sets the state and prints:
       "USER[RS]: SLEEP", "USER[RS]: LOG (mock 10 lines)", "USER[RS]: XFER (notify 10 lines)".
   - USER_RS_Handle() is called once per MX_BLESensorsPnPL_Process() loop.
     For now:
       - RS_SLEEP: after conditions, go to RS_LOG.
       - RS_LOG: fill 10 text lines into user_lines[] (mock data) and then RS_XFER.
       - RS_XFER: if connected + CCCD enabled + GATT ready, send those 10 lines via USER_DATA_Notify()
                  and then go back to RS_SLEEP.
   - This is just a test harness; we plan to replace mock text with real sensor windows.

3) GATT "quiet window" after connect
   - We added:
       volatile uint8_t user_gatt_ready = 0;
       uint32_t user_conn_t0;
   - In hci_le_connection_complete_event():
       user_gatt_ready = 0;
       user_conn_t0    = HAL_GetTick();
   - In hci_disconnection_complete_event():
       user_gatt_ready = 0;
   - In MX_BLESensorsPnPL_Process() main loop:
       if (ConnectionHandle != 0 && user_gatt_ready == 0) {
         if ((HAL_GetTick() - user_conn_t0) > USER_GATT_QUIET_MS) {
           user_gatt_ready = 1;
           STBOX1_PRINTF("USER: GATT ready after quiet window\r\n");
         }
       }
   - This prevents us from blasting notifications during service discovery.

4) Battery notifications
   - We kept the default ST battery characteristic (UUID 00020000-0001-11e1-ac36-0002a5d5c51b).
   - On the Python side we parse a 9-byte payload:
       [0] flags
       [1] raw level byte (not reliable)
       [2:4] reserved
       [4:6] voltage_mV (uint16 little-endian)
       [6:8] current (uint16, 0x8000 = "N/A")
       [8] status (0=Low,1=Discharging,2=NotPresent,3=Charging,4=Full)
   - For % we DO NOT trust the raw level byte. Instead we estimate from voltage using
     a linear fit based on two calibration points from the official ST app:
       - 3.382 V -> 13%
       - 3.775 V -> 53%
     So we use:
       SoC% ≈ a * V + b with a ≈ 101.78, b ≈ -331.22, clamped to [0,100].

5) Radio duty-cycling for advertising and LED
   - We added:
       typedef enum { USER_RADIO_IDLE=0, USER_RADIO_ADV, USER_RADIO_SLEEP } user_radio_state_t;
       volatile user_radio_state_t user_radio_state;
       uint32_t user_radio_t0;
       uint32_t user_led_t0;
   - On disconnect we:
       set_connectable = TRUE;  // ST flag
       user_radio_state = USER_RADIO_IDLE;
   - In MX_BLESensorsPnPL_Process(), when ConnectionHandle == 0:
       - If set_connectable is TRUE: we call set_connectable_ble(), mark
         user_radio_state = USER_RADIO_ADV, user_radio_t0 = now and start an "ADV window".
       - While in USER_RADIO_ADV and no connection yet, if (now - user_radio_t0) > 5000 ms
         we call user_stop_advertising() (which wraps aci_gap_set_advertising_enable(DISABLE,...))
         and move to USER_RADIO_SLEEP, user_radio_t0 = now.
       - While in USER_RADIO_SLEEP, after e.g. 60000 ms we set set_connectable = TRUE again
         to re-open an ADV window.
   - LED_GREEN is toggled only while advertising (USER_RADIO_ADV && ConnectionHandle==0),
     otherwise it is kept off.

Python side:

- Script: auto_reconnect_data_and_battery.py
- It:
   - Scans for device named "BLEPnP" with BleakScanner.find_device_by_name().
   - When found, connects via BleakClient, sets disconnected_callback.
   - After a small delay (~2 s) to respect quiet window:
       - start_notify(USER_CHAR, handle_user_data)
       - start_notify(BAT_CHAR, handle_battery)
   - handle_user_data: prints each notify as a line: "[DATA] ...".
   - handle_battery: uses parse_battery() + estimate_pct_from_voltage() to print:
       "[BAT] ~XX% (raw=YY) | Charging/Discharging/... | 3.78 V | current: N/A"
   - Outer loop auto-reconnects when session ends.

NEXT GOAL:

- Replace "LOG (mock 10 lines)" + text notifications with:
    (a) a binary packet/window format,
    (b) then real ISM330DHCX accel/gyro samples at 1 kHz (later, SD logging).
- Keep the state machine RS_SLEEP -> RS_LOG -> RS_XFER structure, but LOG will collect
  real sensor data and XFER will send binary windows to Python, which will parse them.
