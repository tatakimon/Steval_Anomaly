/*
 * app_sd.h
 *
 *  Created on: Nov 30, 2025
 *      Author: kerem
 */


#ifndef APP_SD_H
#define APP_SD_H

#include "STWIN.box_sd.h"

/* STWIN.box BSP gives us an array of handles: hsd_sdmmc[0]
 * Many FileX examples expect a global "hsd1".
 * We just alias hsd1 to hsd_sdmmc[0] — no extra code, just a macro.
 */

extern SD_HandleTypeDef hsd_sdmmc[SD_INSTANCES_NBR];
#define hsd1 hsd_sdmmc[0]

#endif /* APP_SD_H */
