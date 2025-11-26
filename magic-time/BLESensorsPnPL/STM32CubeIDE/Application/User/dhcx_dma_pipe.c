/*
 * dhcx_dma_pipe.c
 *
 *  Created on: Nov 21, 2025
 *      Author: kerem
 */


/* USER CODE PATCH DHCX_DMA_PIPE BEGIN: dhcx_dma_pipe.c */

#include "dhcx_dma_pipe.h"

#include "stbox1_config.h"          /* STBOX1_PRINTF */
#include "STWIN.box_motion_sensors.h" /* CS_DHCX_* */
#include "steval_stwinbx1.h"
#include  <stdio.h>
#include  <stdlib.h>

/* USER CODE BEGIN Includes_DHCX_PNPL */


/* USER CODE BEGIN DHCX_DMA_HANDLES */
#include "stm32u5xx_hal.h"

extern SPI_HandleTypeDef hspi2;

DMA_HandleTypeDef hdma_spi2_rx;
DMA_HandleTypeDef hdma_spi2_tx;
/* USER CODE END DHCX_DMA_HANDLES */



#include "ble_manager.h"              // already used elsewhere in the project
extern uint16_t ConnectionHandle;     // from app_blesensorspnpl.c
extern volatile uint8_t g_pnpl_ready;
// Forward declaration: real type is tBleStatus (uint8_t), this is compatible
extern uint8_t ble_pn_p_like_update(uint8_t *data, uint8_t length);
/* USER CODE END Includes_DHCX_PNPL */





/* USER CODE PATCH: use SPI2 as DHCX bus (STWIN.box) */
extern SPI_HandleTypeDef hspi2;    // declared in main.c / stm32u5xx_hal_msp.c
#define DHCX_SPI_HANDLE hspi2

/* These come from the BLE / PnP framework */
extern uint8_t *JSON_string_command_wTP;
extern uint32_t JSON_len_command_wTP;

/* Sensor SPI handle (SPI2) from CubeMX */

/* ------------------- DHCX sample + pipeline config ------------------- */

typedef struct
{
  int16_t gx, gy, gz;
  int16_t ax, ay, az;
} dhcx_raw_t;

/* 1 kHz * 2 s = 2000 samples → 24 KB buffer */
#define DHCX_SAMPLES_PER_SEC    1000U
#define DHCX_RECORD_SECONDS     2U
#define DHCX_TOTAL_SAMPLES      (DHCX_SAMPLES_PER_SEC * DHCX_RECORD_SECONDS)

#define DHCX_PNPL_BYTES_PER_SAMPLE  12U   // 6×int16
#define DHCX_PNPL_MAX_PAYLOAD       60U   // 5 samples per notification
const uint32_t max_window_samples = 8U; // instead of 20U


typedef enum
{
  PIPE_STATE_WAIT_TRIGGER = 0,   /* fake: wait 15 s */
  PIPE_STATE_RECORD,             /* ~2 s DMA capture */
  PIPE_STATE_UPLOAD,             /* binary upload via PnP-like */
  PIPE_STATE_SLEEP               /* idle for now */
} pipe_state_t;

static pipe_state_t g_pipe_state;
static uint32_t     g_pipe_t0;
static dhcx_raw_t   g_samples[DHCX_TOTAL_SAMPLES];
static uint16_t     g_samples_count;
static uint8_t      g_upload_started;


/* USER PIPE UPLOAD STATE BEGIN */
static uint32_t g_upload_idx = 0U;
static uint32_t g_upload_n   = 0U;
/* USER PIPE UPLOAD STATE END */


static uint8_t dhcx_both_ready(void);

static HAL_StatusTypeDef dhcx_read_gyro_accel_dma(dhcx_raw_t *out);


/* ------------------------ DHCX register defs ------------------------- */






#ifndef DHCX_REG_WHOAMI
#define DHCX_REG_WHOAMI    0x0F
#define DHCX_REG_STATUS    0x1E
#define DHCX_REG_CTRL1_XL  0x10
#define DHCX_REG_CTRL2_G   0x11
#define DHCX_REG_CTRL3_C   0x12
#define DHCX_REG_OUTX_L_G  0x22

#define DHCX_BDU           (1u << 6)
#define DHCX_IF_INC        (1u << 2)

#define DHCX_ODR_6k66      (0x0Au)       /* 6.66 kHz */
#define DHCX_FS_XL_2G      (0x00u << 2)  /* ±2 g    */
#define DHCX_FS_G_2000DPS  (0x03u << 2)  /* ±2000 dps */
#endif

/* ----------------------- low-level DMA helpers ------------------------ */


/* Chip-select helpers for DHCX */
static inline void dhcx_cs_low(void)
{
  HAL_GPIO_WritePin(BSP_ISM330DHCX_CS_PORT,
                    BSP_ISM330DHCX_CS_PIN,
                    GPIO_PIN_RESET);
}

static inline void dhcx_cs_high(void)
{
  HAL_GPIO_WritePin(BSP_ISM330DHCX_CS_PORT,
                    BSP_ISM330DHCX_CS_PIN,
                    GPIO_PIN_SET);
}


volatile uint32_t spi2_dma_done = 0;
volatile uint32_t spi2_dma_irq_hits = 0;
/* We only care about SPI2 DMA completion here */
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
  if (hspi->Instance == SPI2)
  {
	spi2_dma_irq_hits++;
    spi2_dma_done = 1;
  }
}

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
  if (hspi->Instance == SPI2)
  {
    spi2_dma_done = 1;
  }
}







extern DMA_HandleTypeDef handle_GPDMA1_Channel0; // TX
extern DMA_HandleTypeDef handle_GPDMA1_Channel1; // RX



/* USER CODE BEGIN DHCX_DMA_INIT */
static void DHCX_SPI2_DMA_Init(void)
{

  /* 1) Ensure GPDMA1 clock is on (you already do this in MX_GPDMA1_Init) */
  __HAL_RCC_GPDMA1_CLK_ENABLE();

  /* 2) Configure RX channel (pick a free channel, here Channel1 as example) */
  hdma_spi2_rx.Instance                    = GPDMA1_Channel1;
  hdma_spi2_rx.Init.Request                = GPDMA1_REQUEST_SPI2_RX;
  hdma_spi2_rx.Init.BlkHWRequest           = DMA_BREQ_SINGLE_BURST;
  hdma_spi2_rx.Init.Direction              = DMA_PERIPH_TO_MEMORY;
  hdma_spi2_rx.Init.SrcInc                 = DMA_SINC_FIXED;
  hdma_spi2_rx.Init.DestInc                = DMA_DINC_INCREMENTED;
  hdma_spi2_rx.Init.SrcDataWidth           = DMA_SRC_DATAWIDTH_BYTE;
  hdma_spi2_rx.Init.DestDataWidth          = DMA_DEST_DATAWIDTH_BYTE;
  hdma_spi2_rx.Init.Priority               = DMA_HIGH_PRIORITY;
  hdma_spi2_rx.Init.TransferEventMode      = DMA_TCEM_BLOCK_TRANSFER;
  hdma_spi2_rx.Init.Mode                   = DMA_NORMAL;

  if (HAL_DMA_Init(&hdma_spi2_rx) != HAL_OK)
  {
    Error_Handler(); // or your own error hook
  }

  /* Link RX DMA to SPI2 */
  __HAL_LINKDMA(&hspi2, hdmarx, hdma_spi2_rx);

  /* 3) Configure TX channel (Channel2 as example) */
  hdma_spi2_tx.Instance                    = GPDMA1_Channel2;
  hdma_spi2_tx.Init.Request                = GPDMA1_REQUEST_SPI2_TX;
  hdma_spi2_tx.Init.BlkHWRequest           = DMA_BREQ_SINGLE_BURST;
  hdma_spi2_tx.Init.Direction              = DMA_MEMORY_TO_PERIPH;
  hdma_spi2_tx.Init.SrcInc                 = DMA_SINC_INCREMENTED;
  hdma_spi2_tx.Init.DestInc                = DMA_DINC_FIXED;
  hdma_spi2_tx.Init.SrcDataWidth           = DMA_SRC_DATAWIDTH_BYTE;
  hdma_spi2_tx.Init.DestDataWidth          = DMA_DEST_DATAWIDTH_BYTE;
  hdma_spi2_tx.Init.Priority               = DMA_HIGH_PRIORITY;
  hdma_spi2_tx.Init.TransferEventMode      = DMA_TCEM_BLOCK_TRANSFER;
  hdma_spi2_tx.Init.Mode                   = DMA_NORMAL;

  if (HAL_DMA_Init(&hdma_spi2_tx) != HAL_OK)
  {
    Error_Handler();
  }

  __HAL_LINKDMA(&hspi2, hdmatx, hdma_spi2_tx);

  /* 4) NVIC for these 2 channels */
  HAL_NVIC_SetPriority(GPDMA1_Channel1_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(GPDMA1_Channel1_IRQn);

  HAL_NVIC_SetPriority(GPDMA1_Channel2_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(GPDMA1_Channel2_IRQn);
}
/* USER CODE END DHCX_DMA_INIT */
















/* Simple blocking write (no DMA) */
static HAL_StatusTypeDef dhcx_write(uint8_t reg, uint8_t val)
{


  uint8_t tx[2] = { (uint8_t)(reg & 0x7F), val }; /* MSB=0 → write */
  dhcx_cs_low();
  HAL_StatusTypeDef st = HAL_SPI_Transmit(&hspi2, tx, 2, HAL_MAX_DELAY);
  dhcx_cs_high();
  return st;
}





/* Generic read using SPI2 + DMA: send reg|0x80 then read len bytes */
/* USER CODE PATCH DHCX_DMA_PIPE BEGIN: dhcx_read_dma debug blocking */
/* USER CODE PATCH DHCX READ (blocking) BEGIN */

	static HAL_StatusTypeDef dhcx_read_blocking(uint8_t reg, uint8_t *dst, uint16_t len)
	{
	  uint8_t tx[32];
	  uint8_t rx[32];

	  if ((uint16_t)(len + 1U) > (uint16_t)sizeof(tx))
	  {
	    return HAL_ERROR;
	  }

	  tx[0] = 0x80U | (reg & 0x7FU);   // read bit
	  memset(&tx[1], 0, len);

	  dhcx_cs_low();
	  int32_t bsp_ret = BSP_SPI2_SendRecv(tx, rx, (uint16_t)(len + 1U));
	  dhcx_cs_high();

	  if (bsp_ret != BSP_ERROR_NONE)
	  {
	    return HAL_ERROR;
	  }

	  memcpy(dst, &rx[1], len);
	  return HAL_OK;
	}

	/* USER CODE PATCH DHCX READ (blocking) END */

	/* USER CODE PATCH DHCX_DMA_PIPE DMA READ BEGIN */

	/**
	  * @brief  Blocking read using SPI2 DMA.
	  *         Kicks DMA, waits on spi2_dma_done flag, then copies data.
	  * @note   Still blocking from caller’s POV, but uses DMA underneath.
	  */
	static HAL_StatusTypeDef dhcx_read_dma(uint8_t reg, uint8_t *dst, uint16_t len)
	{
	  static uint8_t spi_tx[32];
	  static uint8_t spi_rx[32];

	  if ((uint16_t)(len + 1U) > (uint16_t)sizeof(spi_tx))
	  {
	    return HAL_ERROR;
	  }

	  /* 0x80 = read, auto-increment */
	  spi_tx[0] = 0x80U | (reg & 0x7FU);
	  memset(&spi_tx[1], 0, len);





	  spi2_dma_done = 0U;
	  dhcx_cs_low();

	  HAL_StatusTypeDef st = HAL_SPI_TransmitReceive_DMA(&hspi2, spi_tx, spi_rx, len + 1U);
	  if (st != HAL_OK)
	  {
	    dhcx_cs_high();

	    if (st == HAL_BUSY)
	    {
	      // A previous DMA is still in progress → caller should just try later
	      // No print here to avoid spamming the UART
	      return HAL_BUSY;
	    }

	    STBOX1_PRINTF("DHCX: HAL_SPI_TransmitReceive_DMA err=%d on reg 0x%02X\r\n",
	                  st, reg);
	    return st;
	  }









	  /* DEBUG HACK BEGIN */
	 // spi2_dma_done = 1;   // <--- force-complete
	  /* DEBUG HACK END */
	  /* Wait for DMA complete callback to set spi2_dma_done */

	  while (spi2_dma_done == 0U)
	  {


	  }

	  dhcx_cs_high();



	  memcpy(dst, &spi_rx[1], len);
	  return HAL_OK;
	}


	/* USER CODE PATCH DHCX_DMA_PIPE DMA READ END */









/* USER CODE PATCH DHCX_1KHZ_REC BEGIN */
/**
  * @brief  Blocking recorder: ~1 kHz into g_samples[] for duration_ms.
  * @note   Uses current dhcx_both_ready() + dhcx_read_gyro_accel_dma().
  *         Later we can add window-level threshold + SD burst here.
  */

/* USER CODE PATCH DHCX_1KHZ_REC END */











/* STATUS: both gyro + accel ready? */
static uint8_t dhcx_both_ready(void)
{
  uint8_t s = 0;
  if (HAL_OK != dhcx_read_dma(DHCX_REG_STATUS, &s, 1U))
  {
    return 0U;
  }
  return ((s & 0x03U) == 0x03U);   /* GDA | XLDA */
}

/* Burst 6 axes starting at OUTX_L_G */
static HAL_StatusTypeDef dhcx_read_gyro_accel_dma(dhcx_raw_t *out)
{
  uint8_t b[12];
  HAL_StatusTypeDef st = dhcx_read_dma(DHCX_REG_OUTX_L_G, b, sizeof(b));
  if (st != HAL_OK)
  {
    return st;
  }

  out->gx = (int16_t)((b[1] << 8) | b[0]);
  out->gy = (int16_t)((b[3] << 8) | b[2]);
  out->gz = (int16_t)((b[5] << 8) | b[4]);
  out->ax = (int16_t)((b[7] << 8) | b[6]);
  out->ay = (int16_t)((b[9] << 8) | b[8]);
  out->az = (int16_t)((b[11] << 8) | b[10]);

  return HAL_OK;
}

/* ------------------------ Pipeline implementation ---------------------- */
extern int32_t BSP_SPI2_Init(void);


void DHCX_Pipeline_Init(void)
{
  if (BSP_SPI2_Init() != BSP_ERROR_NONE)
  {
    STBOX1_PRINTF("BSP_SPI2_Init failed\r\n");
    return;
  }

  /* USER CODE BEGIN After_SPI2_Init */
  DHCX_SPI2_DMA_Init();
  /* USER CODE END After_SPI2_Init */

  // WHO_AM_I test
  uint8_t who = 0;
  if (HAL_OK == dhcx_read_blocking(DHCX_REG_WHOAMI, &who, 1U))
  {
    STBOX1_PRINTF("DHCX WHO_AM_I = 0x%02X\r\n", who);
  }
  else
  {
    STBOX1_PRINTF("DHCX WHO_AM_I read failed\r\n");
  }
  /* USER DHCX DMA WHOAMI TEST BEGIN */
   {
     uint8_t who_dma = 0U;
     HAL_StatusTypeDef st = dhcx_read_dma(DHCX_REG_WHOAMI, &who_dma, 1U);

     if (st == HAL_OK)
     {
       STBOX1_PRINTF("DHCX WHO_AM_I (DMA) = 0x%02X\r\n", who_dma);
     }
     else
     {
      // STBOX1_PRINTF("DHCX WHO_AM_I (DMA) read failed, st=%d\r\n", (int)st);
     }
   }
   /* USER DHCX DMA WHOAMI TEST END */



}
/* USER CODE PATCH DHCX INIT END */


//-------------------------------------------STATE MACHINE---------------------------------------------------


void DHCX_Pipeline_Step(void)
{
  uint32_t now = HAL_GetTick();

  switch (g_pipe_state)
  {



  //-------------------------------------------TRIGGER---------------------------------------------------



    case PIPE_STATE_WAIT_TRIGGER:


    {
       static uint8_t armed = 0U;

       /* Only arm the trigger if we have a BLE connection */
       if (ConnectionHandle != 0U)
       {
         if (armed == 0U)
         {
           g_pipe_t0 = HAL_GetTick();   // start delay *after* connect
           armed     = 1U;
           STBOX1_PRINTF("PIPE: BLE connected → arming trigger timer\r\n");
         }
         else
         {
           uint32_t dt = HAL_GetTick() - g_pipe_t0;
           if (dt >= 3000U)   /* 3 s after connection; change if you want */
           {
             STBOX1_PRINTF("PIPE: fake trigger → start record\r\n");
             g_samples_count = 0;
             g_pipe_state    = PIPE_STATE_RECORD;
             g_pipe_t0       = HAL_GetTick();   // <-- add this
             armed           = 0U;      // disarm for this cycle
           }
         }
       }
       else
       {
         /* No connection: disarm, wait */
         armed = 0U;
       }
       break;
     }
     /* USER PIPE STATE END: gate trigger on BLE connection */




      //-------------------------------------------TRIGGER---------------------------------------------------









      //-------------------------------------------RECORD---------------------------------------------------


    case PIPE_STATE_RECORD:
    {
    	uint32_t t_start = HAL_GetTick();
    	      uint32_t local_count = 0U;

    	      STBOX1_PRINTF("PIPE: 1kHz record start\r\n");

    	      /* Blocking loop: grab up to DHCX_TOTAL_SAMPLES samples */
    	      while (local_count < DHCX_TOTAL_SAMPLES)
    	      {
    	        /* Wait until both accel & gyro have new data ready */
    	        while (!dhcx_both_ready())
    	        {
    	          /* busy wait – this is OK for a 2 s burst */
    	        }

    	        dhcx_raw_t s;
    	        if (HAL_OK != dhcx_read_gyro_accel_dma(&s))
    	        {
    	          STBOX1_PRINTF("PIPE: read error, aborting record\r\n");
    	          break;
    	        }

    	        g_samples[local_count++] = s;
    	      }

    	      g_samples_count = local_count;

    	      uint32_t t_end = HAL_GetTick();
    	      uint32_t dt = t_end - t_start;

    	      STBOX1_PRINTF("REC1K: samples=%lu, dt=%lu ms, ~%lu Hz\r\n",
    	                    (unsigned long)g_samples_count,
    	                    (unsigned long)dt,
    	                    (unsigned long)((g_samples_count * 1000UL) / (dt ? dt : 1UL)));

    	      /* First-sample debug */
    	      if (g_samples_count > 0U)
    	      {
    	        const dhcx_raw_t *s0 = &g_samples[0];
    	        STBOX1_PRINTF("PIPE: first=[G %d %d %d | A %d %d %d]\r\n",
    	                      s0->gx, s0->gy, s0->gz,
    	                      s0->ax, s0->ay, s0->az);
    	      }

    	      /* Send up to 20 samples over PNPL if BLE connected */
    	      if (ConnectionHandle != 0U)
    	      {
    	          g_upload_idx = 0U;
    	          g_upload_n   = g_samples_count;  // typically 2000
    	          STBOX1_PRINTF("PIPE: prepare PNPL upload, %lu samples\r\n",
    	                        (unsigned long)g_upload_n);

    	          g_pipe_state = PIPE_STATE_UPLOAD;
    	        }
    	        else
    	        {
    	          STBOX1_PRINTF("PIPE: no BLE connection → skipping PNPL upload\r\n");
    	          g_pipe_state = PIPE_STATE_SLEEP;
    	        }

    	        break;
    	      }
    	    /* USER PIPE RECORD BLOCKING END */
      //-------------------------------------------RECORD---------------------------------------------------







      //-------------------------------------------UPLOAD---------------------------------------------------


    case PIPE_STATE_UPLOAD:
    {
    	/* If disconnected, abort upload */
    	      if (ConnectionHandle == 0U)
    	      {
    	        STBOX1_PRINTF("PIPE: upload aborted (BLE disconnected)\r\n");
    	        g_pipe_state = PIPE_STATE_SLEEP;
    	        break;
    	      }

    	      /* Try to send a few samples per tick to avoid hogging TX pool */
    	      const uint32_t max_per_tick = 4U;
    	      uint32_t sent_this_tick = 0U;

    	      while ((g_upload_idx < g_upload_n) && (sent_this_tick < max_per_tick))
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
    	          buf[2 * k]     = (uint8_t)(vals[k] & 0xFF);
    	          buf[2 * k + 1] = (uint8_t)((vals[k] >> 8) & 0xFF);
    	        }

    	        uint8_t st = ble_pn_p_like_update(buf, 12U);
    	        STBOX1_PRINTF("PIPE: PNPL upload idx=%lu status=%u\r\n",
    	                      (unsigned long)g_upload_idx, st);

    	        if (st == 0U)
    	        {
    	          /* OK, move to next sample */
    	          g_upload_idx++;
    	          sent_this_tick++;
    	        }
    	        else
    	        {
    	          /* TX pool full / not ready.
    	             Wait for aci_gatt_tx_pool_available_event + next tick. */
    	          break;
    	        }
    	      }

    	      if (g_upload_idx >= g_upload_n)
    	      {
    	        STBOX1_PRINTF("PIPE: upload complete, %lu samples\r\n",
    	                      (unsigned long)g_upload_n);
    	        g_pipe_state = PIPE_STATE_SLEEP;   /* or PIPE_STATE_WAIT_TRIGGER if you want loop */
    	      }

    	      break;
    	    }




      //-------------------------------------------UPLOAD---------------------------------------------------







      //-------------------------------------------SLEEP---------------------------------------------------



    case PIPE_STATE_SLEEP:
    default:

    	 g_pipe_state = PIPE_STATE_WAIT_TRIGGER;


    	 /* For now: idle. Later we can re-arm trigger or go to STOP mode. */
    	 break;
  }
}
	//-------------------------------------------SLEEP---------------------------------------------------

/* USER CODE PATCH DHCX_DMA_PIPE END: dhcx_dma_pipe.c */
