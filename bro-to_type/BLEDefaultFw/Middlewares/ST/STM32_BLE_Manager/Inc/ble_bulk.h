

/* Define to prevent recursive inclusion -------------------------------------*/



#pragma once
#include "ble_manager.h"
ble_char_object_t* ble_init_bulk_service(void);
ble_status_t ble_bulk_update(uint8_t *data, uint16_t len);
void notify_event_bulk(ble_notify_event_t event);
