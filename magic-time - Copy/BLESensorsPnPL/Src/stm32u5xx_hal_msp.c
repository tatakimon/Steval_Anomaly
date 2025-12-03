/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file         stm32u5xx_hal_msp.c
  * @brief        This file provides code for the MSP Initialization
  *               and de-Initialization codes.
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
/* USER CODE BEGIN Includes */
/* USER CODE BEGIN Includes */
#include "stm32u5xx_hal.h"
/* USER CODE END Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Define */

/* USER CODE END Define */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN Macro */

/* USER CODE END Macro */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
extern DMA_HandleTypeDef hdma_spi2_rx;
extern DMA_HandleTypeDef hdma_spi2_tx;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* External functions --------------------------------------------------------*/
/* USER CODE BEGIN ExternalFunctions */

/* USER CODE END ExternalFunctions */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{

  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWREx_EnableVddIO2();
  HAL_PWREx_EnableVddA();

  /* System interrupt init*/

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

/**
  * @brief CRC MSP Initialization
  * This function configures the hardware resources used in this example
  * @param hcrc: CRC handle pointer
  * @retval None
  */
void HAL_CRC_MspInit(CRC_HandleTypeDef* hcrc)
{
  if(hcrc->Instance==CRC)
  {
    /* USER CODE BEGIN CRC_MspInit 0 */

    /* USER CODE END CRC_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_CRC_CLK_ENABLE();
    /* USER CODE BEGIN CRC_MspInit 1 */

    /* USER CODE END CRC_MspInit 1 */

  }

}

/**
  * @brief CRC MSP De-Initialization
  * This function freeze the hardware resources used in this example
  * @param hcrc: CRC handle pointer
  * @retval None
  */
void HAL_CRC_MspDeInit(CRC_HandleTypeDef* hcrc)
{
  if(hcrc->Instance==CRC)
  {
    /* USER CODE BEGIN CRC_MspDeInit 0 */

    /* USER CODE END CRC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CRC_CLK_DISABLE();
    /* USER CODE BEGIN CRC_MspDeInit 1 */

    /* USER CODE END CRC_MspDeInit 1 */
  }

}

/**
  * @brief TIM_OC MSP Initialization
  * This function configures the hardware resources used in this example
  * @param htim_oc: TIM_OC handle pointer
  * @retval None
  */
void HAL_TIM_OC_MspInit(TIM_HandleTypeDef* htim_oc)
{
  if(htim_oc->Instance==TIM1)
  {
    /* USER CODE BEGIN TIM1_MspInit 0 */

    /* USER CODE END TIM1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_TIM1_CLK_ENABLE();
    /* TIM1 interrupt Init */
    HAL_NVIC_SetPriority(TIM1_CC_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);
    /* USER CODE BEGIN TIM1_MspInit 1 */

    /* USER CODE END TIM1_MspInit 1 */

  }

}

/**
  * @brief TIM_OC MSP De-Initialization
  * This function freeze the hardware resources used in this example
  * @param htim_oc: TIM_OC handle pointer
  * @retval None
  */
void HAL_TIM_OC_MspDeInit(TIM_HandleTypeDef* htim_oc)
{
  if(htim_oc->Instance==TIM1)
  {
    /* USER CODE BEGIN TIM1_MspDeInit 0 */

    /* USER CODE END TIM1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM1_CLK_DISABLE();

    /* TIM1 interrupt DeInit */
    HAL_NVIC_DisableIRQ(TIM1_CC_IRQn);
    /* USER CODE BEGIN TIM1_MspDeInit 1 */

    /* USER CODE END TIM1_MspDeInit 1 */
  }

}

/* USER CODE BEGIN SPI_MSP_INIT */

/**
  * @brief SPI MSP Initialization
  * This function configures the hardware resources used for SPI2 (DHCX).
  * @param hspi: SPI handle pointer
  * @retval None
  */
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
  if (hspi->Instance == SPI2)
  {
    /* 1) Enable clocks for SPI2 and GPDMA1 */
    __HAL_RCC_SPI2_CLK_ENABLE();
    __HAL_RCC_GPDMA1_CLK_ENABLE();

    /* 2) (Optional) GPIO config for SPI2 is already done by board code.
          We don't touch it here to avoid conflicts. */

    /* 3) Configure GPDMA1 Channel2 for SPI2_RX */
    hdma_spi2_rx.Instance                    = GPDMA1_Channel2;
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
      Error_Handler();
    }

    __HAL_LINKDMA(hspi, hdmarx, hdma_spi2_rx);

    /* 4) Configure GPDMA1 Channel3 for SPI2_TX */
    hdma_spi2_tx.Instance                    = GPDMA1_Channel3;
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

    __HAL_LINKDMA(hspi, hdmatx, hdma_spi2_tx);

    /* 5) NVIC for these DMA channels */
    HAL_NVIC_SetPriority(GPDMA1_Channel2_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(GPDMA1_Channel2_IRQn);

    HAL_NVIC_SetPriority(GPDMA1_Channel3_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(GPDMA1_Channel3_IRQn);
  }
}

/* (Optional) DeInit if you ever deinit SPI2 */
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{
  if (hspi->Instance == SPI2)
  {
    __HAL_RCC_SPI2_CLK_DISABLE();

    HAL_DMA_DeInit(&hdma_spi2_rx);
    HAL_DMA_DeInit(&hdma_spi2_tx);

    HAL_NVIC_DisableIRQ(GPDMA1_Channel2_IRQn);
    HAL_NVIC_DisableIRQ(GPDMA1_Channel3_IRQn);
  }
}

/* USER CODE END SPI_MSP_INIT */


/* USER CODE BEGIN 1 */

void HAL_SD_MspInit(SD_HandleTypeDef* hsd)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(hsd->Instance==SDMMC1)
  {
    /* USER CODE BEGIN SDMMC1_MspInit 0 */

    /* USER CODE END SDMMC1_MspInit 0 */

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_SDMMC|RCC_PERIPHCLK_CLK48;
    PeriphClkInit.IclkClockSelection   = RCC_CLK48CLKSOURCE_HSI48;
    PeriphClkInit.SdmmcClockSelection  = RCC_SDMMCCLKSOURCE_CLK48;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_RCC_SDMMC1_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**SDMMC1 GPIO Configuration
    PC11     ------> SDMMC1_D3
    PC10     ------> SDMMC1_D2
    PC12     ------> SDMMC1_CK
    PD2      ------> SDMMC1_CMD
    PC9      ------> SDMMC1_D1
    PC8      ------> SDMMC1_D0
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_10|GPIO_PIN_12|GPIO_PIN_9
                          |GPIO_PIN_8;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDMMC1;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin       = GPIO_PIN_2;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDMMC1;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(SDMMC1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(SDMMC1_IRQn);

    /* USER CODE BEGIN SDMMC1_MspInit 1 */

    /* USER CODE END SDMMC1_MspInit 1 */
  }
}

void HAL_SD_MspDeInit(SD_HandleTypeDef* hsd)
{
  if(hsd->Instance==SDMMC1)
  {
    /* USER CODE BEGIN SDMMC1_MspDeInit 0 */

    /* USER CODE END SDMMC1_MspDeInit 0 */

    __HAL_RCC_SDMMC1_CLK_DISABLE();

    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_11|GPIO_PIN_10|GPIO_PIN_12|GPIO_PIN_9
                          |GPIO_PIN_8);

    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_2);

    HAL_NVIC_DisableIRQ(SDMMC1_IRQn);

    /* USER CODE BEGIN SDMMC1_MspDeInit 1 */

    /* USER CODE END SDMMC1_MspDeInit 1 */
  }
}





/* USER CODE END 1 */
