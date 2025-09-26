# STEVAL‑STWIN.box: Dual‑IMU DMA Acquisition & Circular Buffer (Anomaly Sensing Prototype)

> **summary**: I built a low‑latency sensor pipeline on the **STEVAL‑STWINBX1 (STM32U585)** that streams simultaneous data from **two SPI IMUs (IIS3DWB + ISM330DHCX)** using **DMA + interrupt chaining**, pushes samples into a **100‑slot circular buffer**, and **logs/streams** over **UART2\@115200**. The design shows hands‑on embedded skills: STM32 HAL, SPI timing, DMA, ISR design, buffering, and practical debugging (WHO\_AM\_I, CS handling, prescalers, ODR alignment). This repo is the firmware foundation for an on‑device anomaly detection pipeline.

---

## What I built

- **Dual‑sensor SPI2 data path**: Reads **IIS3DWB (accel)** and **ISM330DHCX (accel + gyro)** via **SPI2**.
- **DMA‑driven acquisition**: Non‑blocking transfers; **ISR callback** toggles **Chip Select (CS)** and triggers the next read.
- **Interrupt‑chained two‑stage read**: sensor‑A → ISR → sensor‑B → ISR → buffer commit.
- **100‑sample circular buffer** with a compact sensor frame struct (timestamp + 3×3‑axis vectors).
- **UART2 logging** (@115200) prints a window when the buffer fills; easy to pipe to host tools.
- **Configurable SPI prescaler**; Mode‑0 used (sensors support 0/3).
- **Robust bring‑up**: WHO\_AM\_I checks (**0x7B** for IIS3DWB, **0x6B** for ISM330DHCX), unused CS lines driven safe.
- **Project‑ready hygiene**: Terse HAL init, DMA channels set **High** priority, optional independent watchdog via `.ioc`.

---

## Why it matters (skills demonstrated)

- **Embedded C on STM32U5** with **STM32CubeIDE/HAL**
- **High‑throughput SPI** with **DMA** and **interrupt state machines**
- **Real‑time buffering** (ring buffer), producer/consumer thinking
- **Sensor interfacing & validation** (WHO\_AM\_I, ODR/units, scaling)
- **Deterministic timing** reasoning (clock trees, prescalers, ISR latency)
- **Clean debugging** over UART; practical logs for ML ingestion

---

## Hardware & sensors

- **Board**: STEVAL‑STWINBX1 (STWIN.box, STM32U585).
- **Sensors on SPI2**:
  - **IIS3DWB** – high‑frequency accelerometer (used here as 3‑axis accel)
  - **ISM330DHCX** – 6‑DoF IMU; we use both **accel** and **gyro**
- **ODR reference**: 6.67 kHz on both IMU streams in this build (≈150 µs per sample)
- **Host link**: UART2 @ **115200 bps**

> CS lines are switched in software; only one sensor is active per DMA transaction (SPI is full duplex but we serialize via CS).

---

## Firmware architecture

```
+-------------------+     DMA        +-------------------+
|  SPI2 (IIS3DWB)   |<-------------->|  Memory (rx buf A) |
+---------^---------+                +---------v---------+
          |  ISR callback (HAL_SPI_TxRxCpltCallback)
          |  toggles CS, arms next transfer
+---------v---------+     DMA        +-------------------+
|  SPI2 (ISM330)    |<-------------->|  Memory (rx buf B) |
+---------^---------+                +---------v---------+
          |                           commit → CircularBuffer[100]
          |                           when full → UART dump
+---------v---------+
|    UART2 (log)    |
+-------------------+
```

**Key runtime states**

- `read_step ∈ {0,1}`: which sensor to read next
- `g_data_ready ∈ {0,1}`: signals a fully assembled multi‑sensor frame

**Data model (C)**

```c
typedef struct { int16_t x, y, z; } AxesRaw_t;
typedef struct {
  uint32_t  timestamp_ms;   // HAL_GetTick()
  AxesRaw_t iis3dwb_accel;  // 3‑axis accel
  AxesRaw_t ism330_accel;   // 3‑axis accel
  AxesRaw_t ism330_gyro;    // 3‑axis gyro
} SensorSample_t;
```

---

## Timing & throughput (rule‑of‑thumb)

- Example prescaler **/8** → SPI2 ≈ **20 MHz** (board clock permitting)
- Transfer size: **\~20 bytes** per dual‑sensor frame (7B DWB, 13B DHCX)
- Wire time: 20 B × 8 / 20 MHz ≈ **8 µs**
- Overheads (ISR + memcpy/parse): **\~25 µs** (typ.)
- **Total per frame: \~30–50 µs**, well below **150 µs** ODR interval → multiple acquisitions can share the same **ms** timestamp (HAL\_GetTick).

**Example log (buffer‑full dump)**

```
--- BUFFER FULL: Logging 100 samples ---
T:8439 | DWB_A:2936,-1439,16887 | DHCX_G:-20,-65,-33 | DHCX_A:1710,-737,16776
T:8439 | DWB_A:2936,-1439,16887 | DHCX_G:-20,-65,-33 | DHCX_A:1710,-737,16776
...
T:8440 | DWB_A:2936,-1439,16887 | DHCX_G:-20,-65,-33 | DHCX_A:1710,-737,16776
```

> The repeated `T` values reflect **ms‑granularity** timestamps vs. a **6.67 kHz** sensor ODR. A µs‑level timer can be added if needed.

---

## Repository map (what to open)

- **`SPI2_MDA_Chain_2_Sensor/`**  ➜ ⭐ **Final design**: DMA + interrupt chaining for both IMUs; 100‑sample ring; UART dump.
- `DMA_DHCX/`                     ➜ DMA acquisition from **ISM330DHCX** only + UART print.
- `Olmuyorr_DMA_DWB/`             ➜ DMA acquisition from **IIS3DWB** only + UART print.
- `spi_IIS3DWB/`                  ➜ CPU (blocking) SPI reads (single‑sensor baseline).
- `DMA_IIS3DWB/`, `IIS3DWB_Test/` ➜ Early bring‑up experiments (non‑working snapshots kept for reference).

> See each folder’s `Core/Src/main.c` and the generated init blocks for peripheral configuration.

---

## Build & run (STM32CubeIDE)

1. Open the desired project (e.g., `SPI2_MDA_Chain_2_Sensor/`) in **STM32CubeIDE**.
2. Verify `.ioc` settings:
   - **SPI2**: Mode‑0, suitable prescaler (≤ 8 MHz sensor limit recommended), **DMA RX/TX enabled**, both channels **High priority**.
   - **GPIO**: CS pins for **IIS3DWB** and **ISM330DHCX** set as outputs; unused sensor CS held inactive.
   - **USART2**: **115200 8‑N‑1**.
   - *(Optional)* **IWDG** (watchdog) enabled for resiliency.
3. Build, flash via **ST‑Link**.
4. Open a serial terminal at **115200** to view logs.

**Bring‑up checklist**

- WHO\_AM\_I reads: `0x7B` (IIS3DWB), `0x6B` (ISM330DHCX). If `0xFF`, fix wiring/CS/power/Mode.
- If timestamps don’t change often at high ODR, that’s expected with `HAL_GetTick()`; switch to a µs timer if required.

---

## Design notes

- SPI is **serialized** via CS to guarantee clean frames; we intentionally do *not* attempt simultaneous capture.
- ISR is minimal: set flags, queue the next DMA; parsing/commit done outside critical sections.
- CS line management ensures only the intended device is active (others forced inactive).
- UART logs are intentionally concise for host parsing (Python/CSV/JSON adapters can be added).

---

## What’s next (roadmap)

- ✅ Stabilize dual‑sensor DMA chain and ring buffer (this repo)
- ⏭ Add **µs‑granularity timestamp** (TIM‑based) for precise fusion
- ⏭ Stream frames over **USB CDC** or SD logging at ODR rate
- ⏭ On‑device **windowing + features** (RMS, kurtosis, spectral bands)
- ⏭ Tiny **autoencoder** / distance‑based **anomaly score** on MCU
- ⏭ Host‑side training/eval scripts; IMAD‑DS/DCASE adapters

---

## Example output (full fields)

```
IIS3DWB Accel [X,Y,Z]: 2926, -1362, 16855
ISM330  Gyro  [X,Y,Z]: -5,   -76,   -33
ISM330  Accel [X,Y,Z]: 1757, -580,  16772
```

---

## License

MIT

---

## Contact

**Kerem Karabiyik — Embedded / Edge‑AI Engineer**\
Email: [karabiyikerem@gmail.com](mailto\:karabiyikerem@gmail.com)
