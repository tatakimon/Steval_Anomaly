/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "app_blesensorspnpl.h"
#include "fx_api.h"     // <- gives you UINT, ULONG, etc.
#include "app_filex.h"
#include  <stdio.h>
#include  <stdlib.h>
#include "steval_stwinbx1.h"
#include "stbox1_config.h"          /* STBOX1_PRINTF */
#include "STWIN.box_sd.h"

#include "steval_stwinbx1.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "fx_stm32_sd_driver.h"


SD_HandleTypeDef hsd1;   /* SD handle, same as old project */
//extern SD_HandleTypeDef hsd_sdmmc[SD_INSTANCES_NBR];
//#define hsd1 (hsd_sdmmc[0])



/* FileX control blocks */
FX_MEDIA sd_media;
FX_FILE  log_file;    /* you can keep log_file if you prefer */
UCHAR    media_buffer[4096];   /* or uint8_t, but UCHAR from FileX is nice */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

CRC_HandleTypeDef hcrc;

TIM_HandleTypeDef htim1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void SystemPower_Config(void);
static void MX_CRC_Init(void);
static void MX_ICACHE_Init(void);
/* USER CODE BEGIN PFP */
//static void MX_SDMMC1_SD_Init(void);
 UINT DHCX_SD_EnsureFileOpen(void);
void SD_Debug_TestWrite(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static void MX_SDMMC1_SD_Init_Simple(void);

/**
  * @brief SDMMC1 Initialization Function
  * @param None
  * @retval None
  */












/**
  * @brief GPDMA1 Initialization Function
  * @param None
  * @retval None
  */


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the System Power */
  SystemPower_Config();

  /* Configure the system clock */
  SystemClock_Config();

  /* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */
 // MX_GPIO_Init();
  //MX_SPI2_Init();
  //MX_DMA_Init();

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_CRC_Init();
  MX_ICACHE_Init();
  MX_BLESensorsPnPL_Init();

 // MX_SDMMC1_SD_Init_Simple();


  /* USER CODE BEGIN 2 */



  /* USER CODE BEGIN 2 */


  /* --- FileX + SD initialization (old-project style) --- */

  /* 1) Initialize FileX core */
  fx_system_initialize();

  /* 2) Initialize SD peripheral (uses HAL_SD_MspInit in msp.c) */
  MX_SDMMC1_SD_Init_Simple();
  /* 3) Mount SD and open/create DHCX.BIN */
  {
      UINT status;

      status = fx_media_open(&sd_media, "SD_DISK",
                             fx_stm32_sd_driver, (VOID*)&hsd1,
                             media_buffer, sizeof(media_buffer));
      if (status != FX_SUCCESS)
      {
          STBOX1_PRINTF("SD: fx_media_open failed st=%u\r\n", (unsigned)status);
          Error_Handler();
      }

      status = fx_file_open(&sd_media,
                            &log_file,
                            "DHCX.BIN",
                            FX_OPEN_FOR_WRITE | FX_OPEN_FOR_READ);

      if (status == FX_NOT_FOUND)
      {
          fx_file_create(&sd_media, "DHCX.BIN");
          status = fx_file_open(&sd_media,
                                &log_file,
                                "DHCX.BIN",
                                FX_OPEN_FOR_WRITE | FX_OPEN_FOR_READ);
      }

      if (status != FX_SUCCESS)
      {
          STBOX1_PRINTF("SD: fx_file_open error=%u\r\n", (unsigned)status);
          Error_Handler();
      }

      fx_file_seek(&log_file, log_file.fx_file_current_file_size);

      STBOX1_PRINTF("SD: DHCX.BIN opened OK\r\n");
  }


  /* USER CODE END 2 */

  /* USER CODE END 2 */



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

  MX_BLESensorsPnPL_Process();
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}













/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSI
                              |RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
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
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK3;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the common periph clock
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_MDF1|RCC_PERIPHCLK_ADF1
                              |RCC_PERIPHCLK_ADCDAC;
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
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Power Configuration
  * @retval None
  */
static void SystemPower_Config(void)
{
  HAL_PWREx_EnableVddIO2();

  /*
   * Disable the internal Pull-Up in Dead Battery pins of UCPD peripheral
   */
  HAL_PWREx_DisableUCPDDeadBattery();

  /*
   * Switch to SMPS regulator instead of LDO
   */
  if (HAL_PWREx_ConfigSupply(PWR_SMPS_SUPPLY) != HAL_OK)
  {
    Error_Handler();
  }
/* USER CODE BEGIN PWR */
/* USER CODE END PWR */
}

/**
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
  hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
  hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
  hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
  hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}























/**
  * @brief SDMMC1 Initialization Function
  * @param None
  * @retval None
  */

static void MX_SDMMC1_SD_Init_Simple(void)
{
  /* USER CODE BEGIN SDMMC1_Init 0 */

  /* USER CODE END SDMMC1_Init 0 */

  /* USER CODE BEGIN SDMMC1_Init 1 */

  /* USER CODE END SDMMC1_Init 1 */
  hsd1.Instance = SDMMC1;
  hsd1.Init.ClockEdge           = SDMMC_CLOCK_EDGE_RISING;
  hsd1.Init.ClockPowerSave      = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  hsd1.Init.BusWide             = SDMMC_BUS_WIDE_4B;
  hsd1.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_ENABLE;  // like your old proj
  hsd1.Init.ClockDiv            = 0;  // old project used 0; if unstable we can tune later

  if (HAL_SD_Init(&hsd1) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN SDMMC1_Init 2 */

  /* USER CODE END SDMMC1_Init 2 */
}
























/**
  * @brief ICACHE Initialization Function
  * @param None
  * @retval None
  */
static void MX_ICACHE_Init(void)
{

  /* USER CODE BEGIN ICACHE_Init 0 */

  /* USER CODE END ICACHE_Init 0 */

  /* USER CODE BEGIN ICACHE_Init 1 */

  /* USER CODE END ICACHE_Init 1 */

  /** Enable instruction cache in 1-way (direct mapped cache)
  */
  if (HAL_ICACHE_ConfigAssociativityMode(ICACHE_1WAY) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_ICACHE_Enable() != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ICACHE_Init 2 */

  /* USER CODE END ICACHE_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_OC_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_TIMING;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_OC_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OC_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OC_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.BreakAFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.Break2AFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/* USER CODE BEGIN 4 */





void SD_Debug_TestWrite(void)
{
  UINT status;
  FX_FILE test_file;
  const CHAR *msg = "Hello SD from FileX!\r\n";
  UCHAR read_buf[64];
  ULONG actual = 0;

  STBOX1_PRINTF("SD_TEST: starting test...\r\n");

  /* 1) Make sure media is mounted */
  status = DHCX_SD_EnsureFileOpen();
  if (status != FX_SUCCESS)
  {
    STBOX1_PRINTF("SD_TEST: EnsureMediaOpen error=%u\r\n", (unsigned int)status);
    return;
  }

  /* 2) Create TEST.TXT if needed */
  status = fx_file_create(&sd_media, "TEST.TXT");
  if ((status != FX_SUCCESS) && (status != FX_ALREADY_CREATED))
  {
    STBOX1_PRINTF("SD_TEST: file_create error=%u\r\n", (unsigned int)status);
    return;
  }

  /* 3) Open for write (append) */
  status = fx_file_open(&sd_media, &test_file,
                        "TEST.TXT",
                        FX_OPEN_FOR_WRITE | FX_OPEN_FOR_READ);
  if (status != FX_SUCCESS)
  {
    STBOX1_PRINTF("SD_TEST: file_open error=%u\r\n", (unsigned int)status);
    return;
  }

  /* 4) Seek to end (append mode) */
  status = fx_file_seek(&test_file, test_file.fx_file_current_file_size);
  if (status != FX_SUCCESS)
  {
    STBOX1_PRINTF("SD_TEST: file_seek error=%u\r\n", (unsigned int)status);
    fx_file_close(&test_file);
    return;
  }

  /* 5) Write one line */
  status = fx_file_write(&test_file, (VOID *)msg, (ULONG)strlen(msg));
  if (status != FX_SUCCESS)
  {
    STBOX1_PRINTF("SD_TEST: file_write error=%u\r\n", (unsigned int)status);
    fx_file_close(&test_file);
    return;
  }

  /* 6) Close + flush media so it really lands on SD */
  fx_file_close(&test_file);
  fx_media_flush(&sd_media);

  STBOX1_PRINTF("SD_TEST: write OK, now read back...\r\n");

  /* 7) Re-open for read and dump a small chunk */
  status = fx_file_open(&sd_media, &test_file, "TEST.TXT", FX_OPEN_FOR_READ);
  if (status != FX_SUCCESS)
  {
    STBOX1_PRINTF("SD_TEST: reopen for read error=%u\r\n", (unsigned int)status);
    return;
  }

  memset(read_buf, 0, sizeof(read_buf));
  status = fx_file_read(&test_file, read_buf, sizeof(read_buf) - 1, &actual);
  fx_file_close(&test_file);

  if (status == FX_SUCCESS)
  {
    read_buf[actual] = 0; // null-terminate for printing
    STBOX1_PRINTF("SD_TEST: read %lu bytes: \"%s\"\r\n",
                  (unsigned long)actual, read_buf);
  }
  else
  {
    STBOX1_PRINTF("SD_TEST: file_read error=%u\r\n", (unsigned int)status);
  }

  STBOX1_PRINTF("SD_TEST: done.\r\n");
}


















/* ==== DHCX USER PATCH BEGIN SD logging helpers ==== */

/* Open DHCX.BIN once and seek to end for append */
UINT DHCX_SD_EnsureFileOpen(void)
{


	  UINT status;

	  /* Already mounted? */
	  if (sd_media.fx_media_id == FX_MEDIA_ID)
	  {
	    return FX_SUCCESS;
	  }

	  /* Make sure SD hardware is initialized.
	     If you already call BSP_SD_Init(0) somewhere else, you can skip this. */
	  if (BSP_SD_Init(0) != BSP_ERROR_NONE)
	  {
	    STBOX1_PRINTF("SD: BSP_SD_Init failed\r\n");
	    return FX_IO_ERROR;
	  }

	  /* Mount media with FileX */
	  status = fx_media_open(&sd_media,
	                         "SD_DISK",
	                         fx_stm32_sd_driver,
	                         (VOID *)&hsd1,          // or &hsd_sdmmc[0]
	                         media_buffer,
	                         sizeof(media_buffer));

	  if (status != FX_SUCCESS)
	  {
	    STBOX1_PRINTF("SD: fx_media_open error=%u\r\n", (unsigned int)status);
	    return status;
	  }

	  STBOX1_PRINTF("SD: media mounted OK\r\n");
	  return FX_SUCCESS;
	}





/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
