#ifndef COMMUNICATION_MANAGER_H
# define COMMUNICATION_MANAGER_H

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "app_config.h"
#include <freertos/event_groups.h>
#include "app_sync.h"
#include "driver/uart.h"
#include <esp_log.h>

void vTaskCommunicationManager(void *params);
#endif