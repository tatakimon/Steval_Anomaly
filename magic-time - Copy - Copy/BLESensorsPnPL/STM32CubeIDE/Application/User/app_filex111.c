/*
 * app_filex.c
 *
 *  Created on: Nov 28, 2025
 *      Author: kerem
 */

#include "app_filex.h"
#include "STWIN.box_motion_sensors.h" /* CS_DHCX_* */
#include "steval_stwinbx1.h"
#include  <stdio.h>
#include  <stdlib.h>

#include "fx_api.h"
#include "fx_stm32_sd_driver.h"
#include "stm32u5xx_hal.h"
#include "stbox1_config.h"



//extern SD_HandleTypeDef hsd1;

/* ✅ Correct prototype */
//extern HAL_StatusTypeDef MX_SDMMC1_SD_Init(SD_HandleTypeDef *hsd);




//FX_MEDIA sd_media;              // this is the global media object
//static uint8_t media_buffer[4096];




#if 0
UINT MX_FileX_Init(void)
{
	UINT           status;
	    HAL_StatusTypeDef sd_ret;

	    STBOX1_PRINTF("MX_FileX_Init: ENTER\r\n");

	    fx_system_initialize();
	    STBOX1_PRINTF("MX_FileX_Init: after fx_system_initialize\r\n");

	    /* ✅ Pass &hsd1 to the pointer version */
	    sd_ret = MX_SDMMC1_SD_Init(&hsd1);
	    STBOX1_PRINTF("MX_FileX_Init: MX_SDMMC1_SD_Init ret=%d\r\n", sd_ret);
	    if (sd_ret != HAL_OK)
	    {
	        STBOX1_PRINTF("MX_FileX_Init: SD init FAILED\r\n");
	        return FX_IO_ERROR;
	    }

	    STBOX1_PRINTF("MX_FileX_Init: calling fx_media_open...\r\n");
	    status = fx_media_open(&sd_media, "SD_DISK",
	                           fx_stm32_sd_driver, (VOID*)&hsd1,
	                           media_buffer, sizeof(media_buffer));

	    STBOX1_PRINTF("MX_FileX_Init: fx_media_open -> %u\r\n",
	                  (unsigned int)status);

	    return status;
}
#endif
