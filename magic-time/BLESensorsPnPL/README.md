# STWIN.box – ISM330DHCX DMA + BLE PNPL Pipeline (Snapshot)

## 0. Board / Example / Goal

**Board:** `STEVAL-STWINBX1` (STWIN.box, STM32U5 + BlueNRG-2 + ISM330DHCX).

**FW base:** ST example `FP-SNS-STBOX1 – BLESensorsPnPL`.

We did **not** touch the `.ioc`; we add our code on top of ST’s project.

**Goal (current milestone):**

- Configure ISM330DHCX to ≈1–2 kHz ODR (we use 1666 Hz now).
- Record a 2-second window → 2000 samples of `(gx, gy, gz, ax, ay, az)` into RAM using SPI2 + DMA.
- After the window is full, upload all 2000 samples over BLE PNPL (existing PnP-Like char).
- Python PC script subscribes to PNPL, parses the 12-byte binary samples, and writes JSONL to disk.

**Next step:**  
After each 2k sample window, do one battery measurement and later add SD-card logging (likely also DMA).

---

## 1. Sensor configuration (ISM330DHCX @ ~1.6 kHz)

In the ISM330DHCX driver, we changed the “when enabled” setters so we always select high ODR:

```c
static int32_t ISM330DHCX_ACC_SetOutputDataRate_When_Enabled(ISM330DHCX_Object_t *pObj, float Odr)
{
  ism330dhcx_odr_xl_t new_odr;

  new_odr = (Odr <=   12.5f) ? ISM330DHCX_XL_ODR_12Hz5
            : (Odr <=   26.0f) ? ISM330DHCX_XL_ODR_26Hz
            : (Odr <=   52.0f) ? ISM330DHCX_XL_ODR_52Hz
            : (Odr <=  104.0f) ? ISM330DHCX_XL_ODR_104Hz
            : (Odr <=  208.0f) ? ISM330DHCX_XL_ODR_208Hz
            : (Odr <=  416.0f) ? ISM330DHCX_XL_ODR_416Hz
            : (Odr <=  833.0f) ? ISM330DHCX_XL_ODR_833Hz
            : (Odr <= 1666.0f) ? ISM330DHCX_XL_ODR_1666Hz
            : (Odr <= 3332.0f) ? ISM330DHCX_XL_ODR_3332Hz
            :                    ISM330DHCX_XL_ODR_6667Hz;

  /* For now we force 1666 Hz regardless of Odr argument */
  if (ism330dhcx_xl_data_rate_set(&(pObj->Ctx), ISM330DHCX_XL_ODR_1666Hz) != ISM330DHCX_OK)
    return ISM330DHCX_ERROR;

  return ISM330DHCX_OK;
}

static int32_t ISM330DHCX_GYRO_SetOutputDataRate_When_Enabled(ISM330DHCX_Object_t *pObj, float Odr)
{
  ism330dhcx_odr_g_t new_odr;
  /* similar map… */

  if (ism330dhcx_gy_data_rate_set(&(pObj->Ctx), ISM330DHCX_GY_ODR_1666Hz) != ISM330DHCX_OK)
    return ISM330DHCX_ERROR;

  return ISM330DHCX_OK;
}


Result in UART:

REC1K: samples=2000, dt≈1123 ms, ~1780 Hz

So sensor side is fast enough; we conceptually treat it as a ~1 kHz window of 2000 samples.





















2. Data structures & globals (pipeline)

In app_blesensorspnpl.c (or a dedicated dhcx_dma_pipe.c we include), we have:

typedef struct
{
  int16_t gx, gy, gz;
  int16_t ax, ay, az;
} dhcx_raw_t;

#define DHCX_TOTAL_SAMPLES    2000U
#define DHCX_RECORD_SECONDS   2U

static dhcx_raw_t g_samples[DHCX_TOTAL_SAMPLES];
static uint32_t   g_samples_count = 0;

typedef enum {
  PIPE_STATE_SLEEP = 0,
  PIPE_STATE_WAIT_TRIGGER,
  PIPE_STATE_RECORD
} pipe_state_t;

static pipe_state_t g_pipe_state = PIPE_STATE_SLEEP;
static uint32_t     g_pipe_t0    = 0;

/* DMA completion flag */
volatile uint8_t spi2_dma_done = 0;


We also use BLE globals:

#include "ble_function.h"
extern uint16_t ConnectionHandle;          // from BLE stack

































3. SPI2 + DMA setup for ISM330DHCX

We do not use CubeMX; instead we configure DMA manually in our file (e.g. dhcx_dma_pipe.c):

extern SPI_HandleTypeDef hspi2;
DMA_HandleTypeDef hdma_spi2_rx;
DMA_HandleTypeDef hdma_spi2_tx;

static void DHCX_SPI2_DMA_Init(void)
{
  __HAL_RCC_GPDMA1_CLK_ENABLE();

  /* RX: GPDMA1_Channel1 -> SPI2_RX */
  hdma_spi2_rx.Instance              = GPDMA1_Channel1;
  hdma_spi2_rx.Init.Request          = GPDMA1_REQUEST_SPI2_RX;
  hdma_spi2_rx.Init.BlkHWRequest     = DMA_BREQ_SINGLE_BURST;
  hdma_spi2_rx.Init.Direction        = DMA_PERIPH_TO_MEMORY;
  hdma_spi2_rx.Init.SrcInc           = DMA_SINC_FIXED;
  hdma_spi2_rx.Init.DestInc          = DMA_DINC_INCREMENTED;
  hdma_spi2_rx.Init.SrcDataWidth     = DMA_SRC_DATAWIDTH_BYTE;
  hdma_spi2_rx.Init.DestDataWidth    = DMA_DEST_DATAWIDTH_BYTE;
  hdma_spi2_rx.Init.Priority         = DMA_HIGH_PRIORITY;
  hdma_spi2_rx.Init.TransferEventMode= DMA_TCEM_BLOCK_TRANSFER;
  hdma_spi2_rx.Init.Mode             = DMA_NORMAL;
  HAL_DMA_Init(&hdma_spi2_rx);
  __HAL_LINKDMA(&hspi2, hdmarx, hdma_spi2_rx);

  /* TX: GPDMA1_Channel2 -> SPI2_TX */
  hdma_spi2_tx.Instance              = GPDMA1_Channel2;
  hdma_spi2_tx.Init.Request          = GPDMA1_REQUEST_SPI2_TX;
  hdma_spi2_tx.Init.BlkHWRequest     = DMA_BREQ_SINGLE_BURST;
  hdma_spi2_tx.Init.Direction        = DMA_MEMORY_TO_PERIPH;
  hdma_spi2_tx.Init.SrcInc           = DMA_SINC_INCREMENTED;
  hdma_spi2_tx.Init.DestInc          = DMA_DINC_FIXED;
  hdma_spi2_tx.Init.SrcDataWidth     = DMA_SRC_DATAWIDTH_BYTE;
  hdma_spi2_tx.Init.DestDataWidth    = DMA_DEST_DATAWIDTH_BYTE;
  hdma_spi2_tx.Init.Priority         = DMA_HIGH_PRIORITY;
  hdma_spi2_tx.Init.TransferEventMode= DMA_TCEM_BLOCK_TRANSFER;
  hdma_spi2_tx.Init.Mode             = DMA_NORMAL;
  HAL_DMA_Init(&hdma_spi2_tx);
  __HAL_LINKDMA(&hspi2, hdmatx, hdma_spi2_tx);

  /* NVIC */
  HAL_NVIC_SetPriority(GPDMA1_Channel1_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(GPDMA1_Channel1_IRQn);
  HAL_NVIC_SetPriority(GPDMA1_Channel2_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(GPDMA1_Channel2_IRQn);
}


Interrupt handlers in stm32u5xx_it.c:

extern DMA_HandleTypeDef hdma_spi2_rx;
extern DMA_HandleTypeDef hdma_spi2_tx;

void GPDMA1_Channel1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_spi2_rx);
}

void GPDMA1_Channel2_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_spi2_tx);
}

/* Already present: */
extern SPI_HandleTypeDef hspi2;
void SPI2_IRQHandler(void)
{
  HAL_SPI_IRQHandler(&hspi2);
}


DMA completion callback (global, e.g. in dhcx_dma_pipe.c):

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
  if (hspi->Instance == SPI2)
  {
    spi2_dma_done = 1U;
  }
}


We call DHCX_SPI2_DMA_Init() once during init (e.g., after MX_SPI2_Init() in app_blesensorspnpl.c).






































4. DMA read wrapper for ISM330DHCX
static void dhcx_cs_low(void);
static void dhcx_cs_high(void);

/* Low-level DMA read: reg -> dst[len] */
static HAL_StatusTypeDef dhcx_read_dma(uint8_t reg, uint8_t *dst, uint16_t len)
{
  static uint8_t spi_tx[32];
  static uint8_t spi_rx[32];

  if ((uint16_t)(len + 1U) > sizeof(spi_tx))
    return HAL_ERROR;

  spi_tx[0] = 0x80U | (reg & 0x7FU);  // read + auto-inc
  memset(&spi_tx[1], 0, len);

  spi2_dma_done = 0U;
  dhcx_cs_low();

  HAL_StatusTypeDef st =
      HAL_SPI_TransmitReceive_DMA(&hspi2, spi_tx, spi_rx, len + 1U);

  if (st != HAL_OK)
  {
    dhcx_cs_high();

    if (st == HAL_BUSY)
      return HAL_BUSY;      // caller tries again later

    STBOX1_PRINTF("DHCX: HAL_SPI_TransmitReceive_DMA err=%d on reg 0x%02X\r\n",
                  st, reg);
    return st;
  }

  uint32_t t0 = HAL_GetTick();
  while (spi2_dma_done == 0U)
  {
    if ((HAL_GetTick() - t0) > 1000U)
    {
      dhcx_cs_high();
      HAL_SPI_Abort(&hspi2);
      STBOX1_PRINTF("DHCX: DMA timeout on reg 0x%02X\r\n", reg);
      return HAL_TIMEOUT;
    }
  }

  dhcx_cs_high();
  memcpy(dst, &spi_rx[1], len);
  return HAL_OK;
}


Helpers that use this:

Status poll:

static uint8_t dhcx_both_ready(void)
{
  uint8_t s = 0;
  if (HAL_OK != dhcx_read_dma(DHCX_REG_STATUS, &s, 1U))
    return 0U;

  return ((s & 0x03U) == 0x03U);   // GDA | XLDA
}


Gyro + Accel read into dhcx_raw_t:

static HAL_StatusTypeDef dhcx_read_gyro_accel_dma(dhcx_raw_t *out)
{
  uint8_t buf[12];
  HAL_StatusTypeDef st = dhcx_read_dma(DHCX_REG_OUTX_L_G, buf, sizeof(buf));
  if (st != HAL_OK)
    return st;

  out->gx = (int16_t)(buf[1]  << 8 | buf[0]);
  out->gy = (int16_t)(buf[3]  << 8 | buf[2]);
  out->gz = (int16_t)(buf[5]  << 8 | buf[4]);
  out->ax = (int16_t)(buf[7]  << 8 | buf[6]);
  out->ay = (int16_t)(buf[9]  << 8 | buf[8]);
  out->az = (int16_t)(buf[11] << 8 | buf[10]);
  return HAL_OK;
}


We verified via UART + debugger that gx, gy, gz, ax, ay, az are sane.





























5. BLE gating & trigger state machine

We use a small state machine in DHCX_Pipeline_Step():

void DHCX_Pipeline_Step(void)
{
  uint32_t now = HAL_GetTick();

  switch (g_pipe_state)
  {
  case PIPE_STATE_SLEEP:
    /* After each window we go here; we will re-arm trigger next tick */
    g_pipe_state = PIPE_STATE_WAIT_TRIGGER;
    break;

  case PIPE_STATE_WAIT_TRIGGER:
  {
    static uint8_t armed = 0U;

    /* Require BLE connection + PnPLike subscription */
    if ((ConnectionHandle != 0U) &&
        W2ST_CHECK_CONNECTION(W2ST_CONNECT_PNPLIKE))
    {
      if (armed == 0U)
      {
        g_pipe_t0 = now;
        armed     = 1U;
        STBOX1_PRINTF("PIPE: BLE connected → arming trigger timer\r\n");
      }
      else if ((now - g_pipe_t0) >= 3000U)   // 3 s after sub
      {
        STBOX1_PRINTF("PIPE: fake trigger → start record\r\n");
        g_samples_count = 0;
        g_pipe_t0       = now;
        g_pipe_state    = PIPE_STATE_RECORD;
        armed           = 0U;
        STBOX1_PRINTF("PIPE: 1kHz record start\r\n");
      }
    }
    else
    {
      armed = 0U;
    }
    break;
  }

  case PIPE_STATE_RECORD:
    /* see next section */
    break;
  }
}


notify_event_pn_p_like() (in ble_function.c) sets / clears the bit:

void notify_event_pn_p_like(ble_notify_event_t Event)
{
  if (Event == BLE_NOTIFY_SUB)
  {
    W2ST_ON_CONNECTION(W2ST_CONNECT_PNPLIKE);
    STBOX1_PRINTF("PnPLike Subscribe\r\n");
  }
  else if (Event == BLE_NOTIFY_UNSUB)
  {
    W2ST_OFF_CONNECTION(W2ST_CONNECT_PNPLIKE);
    STBOX1_PRINTF("PnPLike Unsubscribe\r\n");
  }
}





























6. RECORD state (DMA) – 2-second window
case PIPE_STATE_RECORD:
{
  /* 1) Fill buffer with as many ready samples as possible */
  while ((g_samples_count < DHCX_TOTAL_SAMPLES) && dhcx_both_ready())
  {
    dhcx_raw_t s;
    HAL_StatusTypeDef st = dhcx_read_gyro_accel_dma(&s);
    if (st != HAL_OK)
    {
      STBOX1_PRINTF("PIPE: read error, aborting record\r\n");
      break;
    }
    g_samples[g_samples_count++] = s;
  }

  uint32_t dt = now - g_pipe_t0;

  /* 2) Stop after 2 s or when buffer full */
  if ((dt >= (DHCX_RECORD_SECONDS * 1000U)) ||
      (g_samples_count >= DHCX_TOTAL_SAMPLES))
  {
    uint32_t eff_hz =
        (dt > 0U) ? (g_samples_count * 1000U / dt) : 0U;

    STBOX1_PRINTF("REC1K: samples=%lu, dt=%lu ms, ~%lu Hz\r\n",
                  (unsigned long)g_samples_count,
                  (unsigned long)dt,
                  (unsigned long)eff_hz);

    if (g_samples_count > 0U)
    {
      const dhcx_raw_t *s0 = &g_samples[0];
      STBOX1_PRINTF("PIPE: first=[G %d %d %d | A %d %d %d]\r\n",
                    s0->gx, s0->gy, s0->gz,
                    s0->ax, s0->ay, s0->az);
    }

    /* 3) Kick off PNPL upload of full window */
    if (W2ST_CHECK_CONNECTION(W2ST_CONNECT_PNPLIKE))
    {
      DHCX_PnPL_StartUpload(g_samples_count);
    }

    /* 4) Go to SLEEP; WAIT_TRIGGER will re-arm */
    g_pipe_state = PIPE_STATE_SLEEP;
  }
  break;
}





























7. PNPL upload engine (full-window upload + pool handling)

Globals:

static uint32_t g_upload_len     = 0;
static uint32_t g_upload_idx     = 0;
static uint8_t  g_upload_running = 0;


Start + continue:

static void DHCX_PnPL_ContinueUpload(void);

void DHCX_PnPL_StartUpload(uint32_t len)
{
  g_upload_len     = len;
  g_upload_idx     = 0;
  g_upload_running = 1U;

  STBOX1_PRINTF("PIPE: prepare PNPL upload, %lu samples\r\n",
                (unsigned long)g_upload_len);

  DHCX_PnPL_ContinueUpload();
}

static void DHCX_PnPL_ContinueUpload(void)
{
  if (!g_upload_running)
    return;

  while (g_upload_idx < g_upload_len)
  {
    const dhcx_raw_t *s = &g_samples[g_upload_idx];

    uint8_t buf[12];
    int16_t vals[6] =
    {
      s->gx, s->gy, s->gz,
      s->ax, s->ay, s->az
    };

    for (int k = 0; k < 6; k++)
    {
      buf[2U * k]     = (uint8_t)(vals[k] & 0xFF);
      buf[2U * k + 1] = (uint8_t)((vals[k] >> 8) & 0xFF);
    }

    uint8_t st = ble_pn_p_like_update(buf, sizeof(buf));
    STBOX1_PRINTF("PIPE: PNPL upload idx=%lu status=%u\r\n",
                  (unsigned long)g_upload_idx, st);

    if (st == 0U)
    {
      g_upload_idx++;
    }
    else if (st == 100U)
    {
      /* TX pool empty – wait for tx_pool_available event */
      return;
    }
    else
    {
      STBOX1_PRINTF("PIPE: PNPL upload error at idx=%lu status=%u\r\n",
                    (unsigned long)g_upload_idx, st);
      g_upload_running = 0U;
      return;
    }
  }

  STBOX1_PRINTF("PIPE: upload complete, %lu samples\r\n",
                (unsigned long)g_upload_len);
  g_upload_running = 0U;
}


We resume when the BLE stack says buffers are available:

void aci_gatt_tx_pool_available_event_function(uint16_t Connection_Handle,
                                               uint16_t Available_Buffers)
{
  /* existing logging... */

  if (g_upload_running)
  {
    DHCX_PnPL_ContinueUpload();
  }
}


Result in UART:

PIPE: prepare PNPL upload, 2000 samples
PIPE: PNPL upload idx=... status=0
PIPE: PNPL upload idx=... status=100
...
PIPE: upload complete, 2000 samples



























8. Python side (Bleak, JSONL writer)

dhcx_pnpl_windows.py / dma_ble.py:

import asyncio, platform, struct, json
from bleak import BleakClient, BleakScanner, BleakError

NAME = "BLEPnP"
PNPL_CHAR = "0000001b-0002-11e1-ac36-0002a5d5c51b"
FILE_PATH = "dhcx_windows.jsonl"

def handle_pnpl(_handle, data: bytearray, f):
    if len(data) != 12:
        print("[PNPL] unexpected len:", len(data))
        return

    gx, gy, gz, ax, ay, az = struct.unpack("<hhhhhh", data)
    rec = {"gx": gx, "gy": gy, "gz": gz,
           "ax": ax, "ay": ay, "az": az}
    f.write(json.dumps(rec) + "\n")
    f.flush()

    print(f"[PNPL] G=({gx:6d},{gy:6d},{gz:6d}) "
          f"A=({ax:6d},{ay:6d},{az:6d})")

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


For each 2-second window, the file grows by 2000 JSON lines.
The terminal prints all samples.




























9. Next steps (plan, including battery read)

Per-window battery read (next immediate task):

After we finish a window (upload complete, 2000 samples) or right after we decide recording is done (before PIPE_STATE_SLEEP), call an existing battery measurement function (e.g. the one used by the BLE battery service).

For now, do one blocking ADC measurement per window – that’s fine (2 s of data, then a single conversion).

Store this into a small metadata struct, e.g.:

static uint16_t g_last_battery_mV;
static uint8_t  g_last_battery_pct;


Later:

Extend the JSONL schema to append "battery_mV" / "battery_pct" once per window (e.g., send a tiny PNPL JSON frame or add a “header packet” before the sample stream).

Optional clean-up:

Tune ODR / decimation so we get very close to 1 kHz (if needed).

Decide when to stop auto-retriggering (currently we keep making windows as long as BLE is connected & subscribed).

Later milestone – SD card logging via DMA:

Introduce a double buffer: while one 2k window is being written to SD via DMA, you fill the next 2k window via SPI-DMA.

Use a different GPDMA channel for SD writes, lower priority than SPI2 to avoid starving sensor reads.

Keep BLE upload optional: either upload first, then write SD, or just log to SD for “offline mode”.