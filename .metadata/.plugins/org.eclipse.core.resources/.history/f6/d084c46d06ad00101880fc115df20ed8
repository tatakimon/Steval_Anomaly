/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body (cleaned)
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_filex.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
// Simple 3-axis raw type
typedef struct {
  int16_t x;
  int16_t y;
  int16_t z;
} AxesRaw_t;

// One complete reading of all active sensors
typedef struct {
  uint32_t    timestamp_ms;   // HAL_GetTick() snapshot
  AxesRaw_t   ism330_accel;   // ISM330DHCX Accel
  AxesRaw_t   ism330_gyro;    // ISM330DHCX Gyro
} SensorSample_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// --- FileX media buffer (cache-line aligned) ---
#if defined(__GNUC__)
__attribute__((aligned(32)))
#endif
static UCHAR media_buffer[4096]; // 8 sectors

// --- SPI/ISM330DHCX framing ---
#define ISM330DHCX_DATA_SIZE 12   // 6 axes * 2 bytes
#define ISM330DHCX_DMA_LEN   13   // +1 command byte
#define MAX_SPI_BUFFER_SIZE  12

// --- Circular buffer for windowed logging ---
#define CIRCULAR_BUFFER_SIZE 100
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
// Keep only the handles that are actually used
RTC_HandleTypeDef   hrtc;
SD_HandleTypeDef    hsd1;
SPI_HandleTypeDef   hspi2;
UART_HandleTypeDef  huart2;

/* USER CODE BEGIN PV */
// FileX
FX_MEDIA  sd_media;
FX_FILE   log_file;

// Circular buffer state
static SensorSample_t circular_buffer[CIRCULAR_BUFFER_SIZE];
static volatile uint32_t buffer_head  = 0;
static volatile uint32_t buffer_count = 0;

// SPI DMA buffers
static uint8_t ism330dhcx_tx_dma_buf[ISM330DHCX_DMA_LEN] = {0};
static uint8_t ism330dhcx_rx_dma_buf[ISM330DHCX_DMA_LEN] = {0};
static uint8_t ism330dhcx_rx_buffer[ISM330DHCX_DATA_SIZE];

// Flags & timing
static volatile uint8_t g_data_ready = 0;   // Set in HAL_SPI_TxRxCpltCallback
static uint32_t         last_sample_time = 0; // 1 kHz pacing
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void SystemPower_Config(void);
static void MX_GPIO_Init(void);
static void MX_GPDMA1_Init(void);
static void MX_ICACHE_Init(void);
static void MX_RTC_Init(void);
static void MX_SDMMC1_SD_Init(void);
static void MX_SPI2_Init(void);
static void MX_TAMP_RTC_Init(void);
static void MX_USART2_UART_Init(void);

/* USER CODE BEGIN PFP */
static void Initialize_Sensors(void);
static void start_sensor_read(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure System Power & Clocks */
  SystemPower_Config();
  SystemClock_Config();
  PeriphCommonClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_GPDMA1_Init();
  MX_ICACHE_Init();
  MX_RTC_Init();

  // --- Manual RTC set (adjust to your build time) ---
  // NOTE: If you later sync from host/GPS/NTP, you can remove this block.
  {
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};

    sTime.Hours   = 16;    // HH
    sTime.Minutes = 39;    // MM
    sTime.Seconds = 0;     // SS
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;
    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) { Error_Handler(); }

    sDate.WeekDay = RTC_WEEKDAY_FRIDAY;
    sDate.Month   = RTC_MONTH_SEPTEMBER; // 09
    sDate.Date    = 19;                  // 19
    sDate.Year    = 25;                  // 2025
    if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK) { Error_Handler(); }
  }

  MX_SDMMC1_SD_Init();
  MX_TAMP_RTC_Init();
  MX_USART2_UART_Init();
  MX_SPI2_Init();
  MX_FileX_Init();

  /* USER CODE BEGIN 2 */
  // --- Sensor bring-up and first read ---
  Initialize_Sensors();
  start_sensor_read();

  // --- Mount SD and open log file once ---
  UINT fx_status = fx_media_open(&sd_media, "SD_DISK",
                                 fx_stm32_sd_driver, (VOID*)&hsd1,
                                 media_buffer, sizeof(media_buffer));
  if (fx_status != FX_SUCCESS) { Error_Handler(); }

  fx_status = fx_file_open(&sd_media, &log_file, "datalog.jsonl", FX_OPEN_FOR_WRITE);
  if (fx_status == FX_NOT_FOUND) {
    fx_file_create(&sd_media, "datalog.jsonl");
    fx_status = fx_file_open(&sd_media, &log_file, "datalog.jsonl", FX_OPEN_FOR_WRITE);
  }
  if (fx_status != FX_SUCCESS) { Error_Handler(); }

  // Seek to end for append mode
  fx_file_seek(&log_file, log_file.fx_file_current_file_size);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    // 1 kHz non-blocking start gate
    if ((HAL_GetTick() - last_sample_time) >= 1) {
      last_sample_time = HAL_GetTick();
      start_sensor_read();
    }

    // When DMA completes, parse and store one sample
    if (g_data_ready) {
      g_data_ready = 0; // clear asap

      SensorSample_t s = {0};
      s.timestamp_ms   = HAL_GetTick();

      // Gyro: OUTX_L_G .. OUTZ_H_G (0x22..0x27)
      s.ism330_gyro.x  = (int16_t)((ism330dhcx_rx_buffer[1]  << 8) | ism330dhcx_rx_buffer[0]);
      s.ism330_gyro.y  = (int16_t)((ism330dhcx_rx_buffer[3]  << 8) | ism330dhcx_rx_buffer[2]);
      s.ism330_gyro.z  = (int16_t)((ism330dhcx_rx_buffer[5]  << 8) | ism330dhcx_rx_buffer[4]);
      // Accel: OUTX_L_A .. OUTZ_H_A (0x28..0x2D)
      s.ism330_accel.x = (int16_t)((ism330dhcx_rx_buffer[7]  << 8) | ism330dhcx_rx_buffer[6]);
      s.ism330_accel.y = (int16_t)((ism330dhcx_rx_buffer[9]  << 8) | ism330dhcx_rx_buffer[8]);
      s.ism330_accel.z = (int16_t)((ism330dhcx_rx_buffer[11] << 8) | ism330dhcx_rx_buffer[10]);

      // Push into circular buffer
      circular_buffer[buffer_head] = s;
      buffer_head = (buffer_head + 1) % CIRCULAR_BUFFER_SIZE;
      if (buffer_count < CIRCULAR_BUFFER_SIZE) { buffer_count++; }
    }

    // When window is full, serialize and append to SD card
    if (buffer_count >= CIRCULAR_BUFFER_SIZE) {
      static char json_buffer[12800]; // ~128B * 100
      char *p = json_buffer;
      int total_bytes = 0;

      for (int i = 0; i < CIRCULAR_BUFFER_SIZE; i++) {
        // Read RTC for human-readable timestamp (date changes rarely, but safe to fetch both)
        RTC_TimeTypeDef t; RTC_DateTypeDef d;
        HAL_RTC_GetTime(&hrtc, &t, RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&hrtc, &d, RTC_FORMAT_BIN);

        uint32_t ms = circular_buffer[i].timestamp_ms % 1000U;
        int n = sprintf(p,
                        "{\"timestamp\":\"20%02d-%02d-%02d %02d:%02d:%02d.%03lu\"," \
                        "\"gyro\":{\"x\":%d,\"y\":%d,\"z\":%d}," \
                        "\"accel\":{\"x\":%d,\"y\":%d,\"z\":%d}}\r\n",
                        d.Year, d.Month, d.Date,
                        t.Hours, t.Minutes, t.Seconds, (unsigned long)ms,
                        circular_buffer[i].ism330_gyro.x,  circular_buffer[i].ism330_gyro.y,  circular_buffer[i].ism330_gyro.z,
                        circular_buffer[i].ism330_accel.x, circular_buffer[i].ism330_accel.y, circular_buffer[i].ism330_accel.z);
        p += n; total_bytes += n;
      }

      // Single large write + flush (forces media commit)
      uint32_t t0 = HAL_GetTick();
      fx_file_write(&log_file, json_buffer, (ULONG)total_bytes);
      fx_media_flush(&sd_media);
      uint32_t dt = HAL_GetTick() - t0;

      // Optional short UART note (kept commented to avoid overhead)
      // char dbg[96];
      // int len = snprintf(dbg, sizeof(dbg), "Wrote 100 lines in %lums\r\n", (unsigned long)dt);
      // HAL_UART_Transmit(&huart2, (uint8_t*)dbg, len, HAL_MAX_DELAY);

      buffer_count = 0; // reset window
      // __WFI(); // low power (optional)
    }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK) {
    Error_Handler();
  }

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSI|
                                     RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.LSIDiv = RCC_LSI_DIV1;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMBOOST = RCC_PLLMBOOST_DIV1;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 1;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLLVCIRANGE_1;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) { Error_Handler(); }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|
                                RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2|
                                RCC_CLOCKTYPE_PCLK3;
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK) { Error_Handler(); }
}

/**
  * @brief Peripherals Common Clock Configuration
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_MDF1|RCC_PERIPHCLK_ADF1|
                                       RCC_PERIPHCLK_ADCDAC;
  PeriphClkInit.Mdf1ClockSelection = RCC_MDF1CLKSOURCE_PLL3;
  PeriphClkInit.Adf1ClockSelection = RCC_ADF1CLKSOURCE_PLL3;
  PeriphClkInit.AdcDacClockSelection = RCC_ADCDACCLKSOURCE_PLL2;
  PeriphClkInit.PLL3.PLL3Source = RCC_PLLSOURCE_HSE;
  PeriphClkInit.PLL3.PLL3M = 2;
  PeriphClkInit.PLL3.PLL3N = 48;
  PeriphClkInit.PLL3.PLL3P = 2;
  PeriphClkInit.PLL3.PLL3Q = 25;
  PeriphClkInit.PLL3.PLL3R = 2;
  PeriphClkInit.PLL3.PLL3RGE = RCC_PLLVCIRANGE_1;
  PeriphClkInit.PLL3.PLL3FRACN = 0;
  PeriphClkInit.PLL3.PLL3ClockOut = RCC_PLL3_DIVQ;
  PeriphClkInit.PLL2.PLL2Source = RCC_PLLSOURCE_HSE;
  PeriphClkInit.PLL2.PLL2M = 2;
  PeriphClkInit.PLL2.PLL2N = 48;
  PeriphClkInit.PLL2.PLL2P = 2;
  PeriphClkInit.PLL2.PLL2Q = 7;
  PeriphClkInit.PLL2.PLL2R = 25;
  PeriphClkInit.PLL2.PLL2RGE = RCC_PLLVCIRANGE_1;
  PeriphClkInit.PLL2.PLL2FRACN = 0;
  PeriphClkInit.PLL2.PLL2ClockOut = RCC_PLL2_DIVR;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) { Error_Handler(); }
}

/**
  * @brief Power Configuration
  */
static void SystemPower_Config(void)
{
  HAL_PWREx_EnableVddIO2();
  HAL_PWREx_DisableUCPDDeadBattery();

  // Disable SRAM Stop retentions we don't use
  HAL_PWREx_DisableRAMsContentStopRetention(PWR_SRAM2_PAGE1_STOP_RETENTION);
  HAL_PWREx_DisableRAMsContentStopRetention(PWR_SRAM2_PAGE2_STOP_RETENTION);
  HAL_PWREx_DisableRAMsContentStopRetention(PWR_SRAM3_PAGE1_STOP_RETENTION);
  HAL_PWREx_DisableRAMsContentStopRetention(PWR_SRAM3_PAGE2_STOP_RETENTION);
  HAL_PWREx_DisableRAMsContentStopRetention(PWR_SRAM3_PAGE3_STOP_RETENTION);
  HAL_PWREx_DisableRAMsContentStopRetention(PWR_SRAM3_PAGE4_STOP_RETENTION);
  HAL_PWREx_DisableRAMsContentStopRetention(PWR_SRAM3_PAGE5_STOP_RETENTION);
  HAL_PWREx_DisableRAMsContentStopRetention(PWR_SRAM3_PAGE6_STOP_RETENTION);
  HAL_PWREx_DisableRAMsContentStopRetention(PWR_SRAM3_PAGE7_STOP_RETENTION);
  HAL_PWREx_DisableRAMsContentStopRetention(PWR_SRAM3_PAGE8_STOP_RETENTION);
  /* USER CODE BEGIN PWR */
  /* USER CODE END PWR */
}

/**
  * @brief GPDMA1 Initialization Function
  */
static void MX_GPDMA1_Init(void)
{
  __HAL_RCC_GPDMA1_CLK_ENABLE();

  // Enable channels we may use (exact mapping is configured in MSP)
  HAL_NVIC_SetPriority(GPDMA1_Channel0_IRQn, 0, 0); HAL_NVIC_EnableIRQ(GPDMA1_Channel0_IRQn);
  HAL_NVIC_SetPriority(GPDMA1_Channel1_IRQn, 0, 0); HAL_NVIC_EnableIRQ(GPDMA1_Channel1_IRQn);
  HAL_NVIC_SetPriority(GPDMA1_Channel2_IRQn, 0, 0); HAL_NVIC_EnableIRQ(GPDMA1_Channel2_IRQn);
  HAL_NVIC_SetPriority(GPDMA1_Channel3_IRQn, 0, 0); HAL_NVIC_EnableIRQ(GPDMA1_Channel3_IRQn);
  HAL_NVIC_SetPriority(GPDMA1_Channel4_IRQn, 0, 0); HAL_NVIC_EnableIRQ(GPDMA1_Channel4_IRQn);
  HAL_NVIC_SetPriority(GPDMA1_Channel5_IRQn, 0, 0); HAL_NVIC_EnableIRQ(GPDMA1_Channel5_IRQn);
}

/**
  * @brief ICACHE Initialization Function
  */
static void MX_ICACHE_Init(void)
{
  if (HAL_ICACHE_ConfigAssociativityMode(ICACHE_1WAY) != HAL_OK) { Error_Handler(); }
  if (HAL_ICACHE_Enable() != HAL_OK) { Error_Handler(); }
}

/**
  * @brief RTC Initialization Function
  */
static void MX_RTC_Init(void)
{
  RTC_PrivilegeStateTypeDef privilegeState = {0};
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  hrtc.Init.OutPutPullUp = RTC_OUTPUT_PULLUP_NONE;
  hrtc.Init.BinMode = RTC_BINARY_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK) { Error_Handler(); }

  privilegeState.rtcPrivilegeFull = RTC_PRIVILEGE_FULL_NO;
  privilegeState.backupRegisterPrivZone = RTC_PRIVILEGE_BKUP_ZONE_NONE;
  privilegeState.backupRegisterStartZone2 = RTC_BKP_DR0;
  privilegeState.backupRegisterStartZone3 = RTC_BKP_DR0;
  if (HAL_RTCEx_PrivilegeModeSet(&hrtc, &privilegeState) != HAL_OK) { Error_Handler(); }

  // Default epoch (00:00:00 01-01-2000) — immediately overwritten in main()
  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};
  sTime.Hours = 0; sTime.Minutes = 0; sTime.Seconds = 0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE; sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) { Error_Handler(); }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY; sDate.Month = RTC_MONTH_JANUARY; sDate.Date = 1; sDate.Year = 0;
  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK) { Error_Handler(); }
}

/**
  * @brief SDMMC1 Initialization Function
  */
static void MX_SDMMC1_SD_Init(void)
{
  hsd1.Instance = SDMMC1;
  hsd1.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
  hsd1.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  hsd1.Init.BusWide = SDMMC_BUS_WIDE_4B;
  hsd1.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd1.Init.ClockDiv = 0;
  if (HAL_SD_Init(&hsd1) != HAL_OK) { Error_Handler(); }
}

/**
  * @brief SPI2 Initialization Function (ISM330DHCX on STWIN.box)
  */
static void MX_SPI2_Init(void)
{
  SPI_AutonomousModeConfTypeDef auto_cfg = {0};

  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8; // tune if needed
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 0x7;
  hspi2.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  hspi2.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi2.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi2.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi2.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi2.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi2.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi2.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  hspi2.Init.ReadyMasterManagement = SPI_RDY_MASTER_MANAGEMENT_INTERNALLY;
  hspi2.Init.ReadyPolarity = SPI_RDY_POLARITY_HIGH;
  if (HAL_SPI_Init(&hspi2) != HAL_OK) { Error_Handler(); }

  auto_cfg.TriggerState = SPI_AUTO_MODE_DISABLE;
  auto_cfg.TriggerSelection = SPI_GRP1_GPDMA_CH1_TCF_TRG; // keep default; not used
  auto_cfg.TriggerPolarity = SPI_TRIG_POLARITY_RISING;
  if (HAL_SPIEx_SetConfigAutonomousMode(&hspi2, &auto_cfg) != HAL_OK) { Error_Handler(); }
}

/**
  * @brief TAMP/RTC tamper minimal configuration
  */
static void MX_TAMP_RTC_Init(void)
{
  RTC_PrivilegeStateTypeDef privilegeState = {0};
  RTC_TamperTypeDef sTamper = {0};

  privilegeState.tampPrivilegeFull = TAMP_PRIVILEGE_FULL_YES;
  privilegeState.MonotonicCounterPrivilege = TAMP_MONOTONIC_CNT_PRIVILEGE_NO;
  if (HAL_RTCEx_PrivilegeModeSet(&hrtc, &privilegeState) != HAL_OK) { Error_Handler(); }

  sTamper.Tamper = RTC_TAMPER_1;
  sTamper.Trigger = RTC_TAMPERTRIGGER_RISINGEDGE;
  sTamper.NoErase = RTC_TAMPER_ERASE_BACKUP_ENABLE;
  sTamper.MaskFlag = RTC_TAMPERMASK_FLAG_DISABLE;
  sTamper.Filter = RTC_TAMPERFILTER_DISABLE;
  sTamper.SamplingFrequency = RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV32768;
  sTamper.PrechargeDuration = RTC_TAMPERPRECHARGEDURATION_1RTCCLK;
  sTamper.TamperPullUp = RTC_TAMPER_PULLUP_ENABLE;
  sTamper.TimeStampOnTamperDetection = RTC_TIMESTAMPONTAMPERDETECTION_ENABLE;
  if (HAL_RTCEx_SetTamper(&hrtc, &sTamper) != HAL_OK) { Error_Handler(); }
}

/**
  * @brief USART2 Initialization Function (debug)
  */
static void MX_USART2_UART_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_RTS; // keep board default
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK) { Error_Handler(); }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK) { Error_Handler(); }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK) { Error_Handler(); }
  if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK) { Error_Handler(); }
}

/**
  * @brief GPIO Initialization Function
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();

  // Chip selects & board pins (as in original)
  HAL_GPIO_WritePin(GPIOH, CS_DHCX_Pin|CS_DLPC_Pin|WIFI_CS_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOI, EX_SPI_NSS_Pin|CS_ICLX_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOH, GPIO_PIN_12|GPIO_PIN_10, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(BLE_SPI_CS_GPIO_Port, BLE_SPI_CS_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(CS_DWB_GPIO_Port, CS_DWB_Pin, GPIO_PIN_SET);

  // Interrupts (unchanged)
  GPIO_InitStruct.Pin = INT_HTS_Pin; GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING; GPIO_InitStruct.Pull = GPIO_NOPULL; HAL_GPIO_Init(INT_HTS_GPIO_Port, &GPIO_InitStruct);

  // Analog (unused pins -> analog)
  GPIO_InitStruct.Pin = WIFI_FLOW_Pin|SD_DETECT_Pin|INT_EX_Pin|INT_EXG5_Pin; GPIO_InitStruct.Mode = GPIO_MODE_ANALOG; GPIO_InitStruct.Pull = GPIO_NOPULL; HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = CS_DHCX_Pin|GPIO_PIN_12|GPIO_PIN_10|CS_DLPC_Pin|WIFI_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; GPIO_InitStruct.Pull = GPIO_NOPULL; GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = DCDC_2_EN_Pin|SPI2_MISO_p2_Pin; GPIO_InitStruct.Mode = GPIO_MODE_ANALOG; GPIO_InitStruct.Pull = GPIO_NOPULL; HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = C_EN_Pin|INT_EXD15_Pin|BLE_RST_Pin|WIFI_BOOT_Pin; GPIO_InitStruct.Mode = GPIO_MODE_ANALOG; GPIO_InitStruct.Pull = GPIO_NOPULL; HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = EX_SPI_NSS_Pin|CS_ICLX_Pin; GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; GPIO_InitStruct.Pull = GPIO_NOPULL; GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = BLE_SPI_CS_Pin; GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; GPIO_InitStruct.Pull = GPIO_NOPULL; GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; HAL_GPIO_Init(BLE_SPI_CS_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = BOOT0_Pin|STSAFE_RESET_Pin; GPIO_InitStruct.Mode = GPIO_MODE_ANALOG; GPIO_InitStruct.Pull = GPIO_NOPULL; HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = USR_BUTTON_Pin|WIFI_NOTIFY_Pin; GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING; GPIO_InitStruct.Pull = GPIO_NOPULL; HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = INT1_DHCX_Pin|NFC_INT_Pin; GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING; GPIO_InitStruct.Pull = GPIO_NOPULL; HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO2_EX_Pin|BLE_TEST9_Pin|BLE_TEST8_Pin; GPIO_InitStruct.Mode = GPIO_MODE_ANALOG; GPIO_InitStruct.Pull = GPIO_NOPULL; HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = INT1_DLPC_Pin|INT2_DLPC_Pin|INT_MAG_Pin|INT_STT_Pin|INT1_ICLX_Pin|INT2_DHCX_Pin|BLE_INT_Pin|INT2_ICLX_Pin|INT1_DWB_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING; GPIO_InitStruct.Pull = GPIO_NOPULL; HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = SW_SEL_Pin|CHRG_Pin; GPIO_InitStruct.Mode = GPIO_MODE_ANALOG; GPIO_InitStruct.Pull = GPIO_NOPULL; HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = uC_ADC_BATT_Pin|ADC_EX_Pin; GPIO_InitStruct.Mode = GPIO_MODE_ANALOG; GPIO_InitStruct.Pull = GPIO_NOPULL; HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = CS_DWB_Pin; GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; GPIO_InitStruct.Pull = GPIO_NOPULL; GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; HAL_GPIO_Init(CS_DWB_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = BUTTON_PWR_Pin; GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING; GPIO_InitStruct.Pull = GPIO_NOPULL; HAL_GPIO_Init(BUTTON_PWR_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = SPI2_MOSI_p2_Pin|GPIO1_EX_Pin; GPIO_InitStruct.Mode = GPIO_MODE_ANALOG; GPIO_InitStruct.Pull = GPIO_NOPULL; HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LDO_EN_Pin|WIFI_EN_Pin; GPIO_InitStruct.Mode = GPIO_MODE_ANALOG; GPIO_InitStruct.Pull = GPIO_NOPULL; HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  // EXTI priorities (kept)
  HAL_NVIC_SetPriority(EXTI0_IRQn, 7, 0);  HAL_NVIC_EnableIRQ(EXTI0_IRQn);
  HAL_NVIC_SetPriority(EXTI1_IRQn, 7, 0);  HAL_NVIC_EnableIRQ(EXTI1_IRQn);
  HAL_NVIC_SetPriority(EXTI2_IRQn, 7, 0);  HAL_NVIC_EnableIRQ(EXTI2_IRQn);
  HAL_NVIC_SetPriority(EXTI3_IRQn, 7, 0);  HAL_NVIC_EnableIRQ(EXTI3_IRQn);
  HAL_NVIC_SetPriority(EXTI4_IRQn, 7, 0);  HAL_NVIC_EnableIRQ(EXTI4_IRQn);
  HAL_NVIC_SetPriority(EXTI5_IRQn, 7, 0);  HAL_NVIC_EnableIRQ(EXTI5_IRQn);
  HAL_NVIC_SetPriority(EXTI6_IRQn, 7, 0);  HAL_NVIC_EnableIRQ(EXTI6_IRQn);
  HAL_NVIC_SetPriority(EXTI7_IRQn, 7, 0);  HAL_NVIC_EnableIRQ(EXTI7_IRQn);
  HAL_NVIC_SetPriority(EXTI8_IRQn, 7, 0);  HAL_NVIC_EnableIRQ(EXTI8_IRQn);
  HAL_NVIC_SetPriority(EXTI9_IRQn, 7, 0);  HAL_NVIC_EnableIRQ(EXTI9_IRQn);
  HAL_NVIC_SetPriority(EXTI10_IRQn, 7, 0); HAL_NVIC_EnableIRQ(EXTI10_IRQn);
  HAL_NVIC_SetPriority(EXTI11_IRQn, 7, 0); HAL_NVIC_EnableIRQ(EXTI11_IRQn);
  HAL_NVIC_SetPriority(EXTI13_IRQn, 7, 0); HAL_NVIC_EnableIRQ(EXTI13_IRQn);
  HAL_NVIC_SetPriority(EXTI14_IRQn, 7, 0); HAL_NVIC_EnableIRQ(EXTI14_IRQn);
}

/* USER CODE BEGIN 4 */
// --- Sensor init & fast-read path -------------------------------------------------
static void Initialize_Sensors(void)
{
  uint8_t tx[2] = {0};
  uint8_t rx[2] = {0};

  // Read WHO_AM_I (0x0F)
  tx[0] = 0x0F | 0x80; // read bit
  tx[1] = 0x00;

  HAL_GPIO_WritePin(CS_DHCX_GPIO_Port, CS_DHCX_Pin, GPIO_PIN_RESET);
  HAL_SPI_TransmitReceive(&hspi2, tx, rx, 2, HAL_MAX_DELAY);
  HAL_GPIO_WritePin(CS_DHCX_GPIO_Port, CS_DHCX_Pin, GPIO_PIN_SET);

  if (rx[1] != 0x6B) { // ISM330DHCX ID
    Error_Handler();
  }

  // CTRL3_C (0x12): BDU=1, IF_INC=1
  uint8_t cfg[2];
  cfg[0] = 0x12; cfg[1] = 0x44;
  HAL_GPIO_WritePin(CS_DHCX_GPIO_Port, CS_DHCX_Pin, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi2, cfg, 2, HAL_MAX_DELAY);
  HAL_GPIO_WritePin(CS_DHCX_GPIO_Port, CS_DHCX_Pin, GPIO_PIN_SET);

  // CTRL1_XL (0x10): Accel ODR ~1.04 kHz (0x80)
  cfg[0] = 0x10; cfg[1] = 0x80;
  HAL_GPIO_WritePin(CS_DHCX_GPIO_Port, CS_DHCX_Pin, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi2, cfg, 2, HAL_MAX_DELAY);
  HAL_GPIO_WritePin(CS_DHCX_GPIO_Port, CS_DHCX_Pin, GPIO_PIN_SET);

  // CTRL2_G (0x11): Gyro ODR ~1.04 kHz (0x80)
  cfg[0] = 0x11; cfg[1] = 0x80;
  HAL_GPIO_WritePin(CS_DHCX_GPIO_Port, CS_DHCX_Pin, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi2, cfg, 2, HAL_MAX_DELAY);
  HAL_GPIO_WritePin(CS_DHCX_GPIO_Port, CS_DHCX_Pin, GPIO_PIN_SET);

  HAL_Delay(100);
}

// Begin a single DMA read of 12 data bytes starting at OUTX_L_G (0x22)
static void start_sensor_read(void)
{
  ism330dhcx_tx_dma_buf[0] = 0x22 | 0x80; // auto-increment read
  HAL_GPIO_WritePin(CS_DHCX_GPIO_Port, CS_DHCX_Pin, GPIO_PIN_RESET);
  if (HAL_SPI_TransmitReceive_DMA(&hspi2,
                                  ism330dhcx_tx_dma_buf,
                                  ism330dhcx_rx_dma_buf,
                                  ISM330DHCX_DMA_LEN) != HAL_OK) {
    Error_Handler();
  }
}

// SPI DMA complete callback: latch data, raise flag
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
  if (hspi->Instance == SPI2) {
    HAL_GPIO_WritePin(CS_DHCX_GPIO_Port, CS_DHCX_Pin, GPIO_PIN_SET);
    memcpy(ism330dhcx_rx_buffer, &ism330dhcx_rx_dma_buf[1], ISM330DHCX_DATA_SIZE);
    g_data_ready = 1;
  }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  __disable_irq();
  while (1) { }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  (void)file; (void)line;
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
