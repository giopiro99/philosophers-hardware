#ifndef APP_TYPES_H
# define APP_TYPES_H

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

typedef struct s_led_msg{
    int philoId;
    int status; // 0 spento 1 acceso
}   t_led_msg;

typedef struct s_philo
{
    int id;
    int eat_count;
    SemaphoreHandle_t   sx_fork;
    SemaphoreHandle_t   dx_fork;
}   t_philo;

#endif