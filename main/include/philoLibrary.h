#ifndef PHILOLIBRARY_H
# define PHILOLIBRARY_H

#include <esp_task.h>
#include <esp_log.h>
#include "driver/gpio.h"
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"    
#include "freertos/queue.h"   
#include "freertos/semphr.h"

extern QueueHandle_t led_queue;
extern volatile bool is_running;
static const char *PHILO_TAG="PHILO";
static const char *LED_TAG="LED";
static const char *ERROR_TAG="ERROR";
static const char *SUCCESS_TAG="SUCCESS";
static const char *DEBUG_TAG="DEBUG";

#define PHILO_NUMBER 5
#define TIME_TO_EAT 0.5
#define TIME_TO_THINK 0.5

typedef struct s_philo
{
    int id;
    int eat_count;
    SemaphoreHandle_t   sx_fork;
    SemaphoreHandle_t   dx_fork;
}   t_philo;

typedef struct s_led_msg{
    int philoId;
    int status; // 0 spento 1 acceso
}   t_led_msg;

void  setup(t_philo *philos, SemaphoreHandle_t *forks);
void    vTaskRoutine(void *params);
void    vTaskLedManager(void *params);

#endif