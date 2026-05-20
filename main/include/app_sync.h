#ifndef APP_SYNC_H
# define APP_SYNC_H

#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"

extern volatile bool                    have_to_restart;
extern SemaphoreHandle_t                restart_sync_semaphore;
extern SemaphoreHandle_t                go_sync_semaphore;
extern SemaphoreHandle_t                button_semaphore;
extern EventGroupHandle_t               system_events;
extern QueueHandle_t                    led_queue;

#endif