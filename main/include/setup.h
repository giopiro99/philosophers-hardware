#ifndef SETUP_H
# define SETUP_H

#include "app_config.h"
#include "app_types.h"
#include <esp_log.h>
#include <esp_system.h>

void  setup(t_philo *philos, SemaphoreHandle_t *forks);
#endif