#ifndef BUTTON_MANAGER_H
# define BUTTON_MANAGER_H

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <freertos/event_groups.h>
#include <esp_log.h>
#include "esp_timer.h"
#include "esp_attr.h"
#include "app_config.h"
#include "app_sync.h"

void    button_isr_handler(void *arg);
void    vTaskButtonManager(void *params);
#endif