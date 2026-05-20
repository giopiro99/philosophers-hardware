#ifndef PHILO_TASK_H
# define PHILO_TASK_H

#include "freertos/FreeRTOS.h"
#include <freertos/queue.h>
#include <esp_log.h>
#include "app_types.h"
#include "app_config.h"
#include "app_sync.h"
void    vTaskRoutine(void *params);

#endif