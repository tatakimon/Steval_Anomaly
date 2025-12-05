/* USER CODE PATCH DHCX_DMA_PIPE BEGIN: dhcx_dma_pipe.h */
#ifndef DHCX_DMA_PIPE_H
#define DHCX_DMA_PIPE_H

#include "stm32u5xx_hal.h"

/* Initialize DHCX DMA + state machine */
void DHCX_Pipeline_Init(void);

/* Run one non-blocking step of the pipeline (call often) */
void DHCX_Pipeline_Step(void);

#endif /* DHCX_DMA_PIPE_H */
/* USER CODE PATCH DHCX_DMA_PIPE END: dhcx_dma_pipe.h */
