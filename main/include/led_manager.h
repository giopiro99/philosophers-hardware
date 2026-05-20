#ifndef LED_MANAGER_H
# define LED_MANAGER_H

#include "freertos/FreeRTOS.h"
#include "app_types.h"
#include "app_config.h"
#include "app_sync.h"
#include "driver/gpio.h"
#include <esp_log.h>

void    vTaskLedManager(void *params);
#endif