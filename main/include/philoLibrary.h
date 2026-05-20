#ifndef PHILOLIBRARY_H
# define PHILOLIBRARY_H

#include <esp_task.h>
#include <esp_log.h>
#include "esp_timer.h" // Serve per il debounce
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"    
#include "freertos/queue.h"   
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern QueueHandle_t                    led_queue;
extern volatile bool                    have_to_restart;
extern SemaphoreHandle_t                restart_sync_semaphore;
extern SemaphoreHandle_t                go_sync_semaphore;
extern SemaphoreHandle_t                button_semaphore;
extern EventGroupHandle_t               system_events;

static const char *PHILO_TAG="PHILO";
static const char *LED_TAG="LED";
static const char *ERROR_TAG="ERROR";
static const char *SUCCESS_TAG="SUCCESS";
static const char *DEBUG_TAG="DEBUG";
static const char *COMMUNICATION_TAG="COMMUNICATION";
static const char *BUTTON_TAG="BUTTON";

#define PHILO_NUMBER 5
#define TIME_TO_EAT 100
#define TIME_TO_THINK 100
#define RUNNING_BIT (1 << 0)
#define BUTTON_PIN GPIO_NUM_25

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

void            setup(t_philo *philos, SemaphoreHandle_t *forks);
void            vTaskRoutine(void *params);
void            vTaskLedManager(void *params);
void            vTaskCommunicationManager(void *params);
void            vTaskButtonManager(void *params);
void            button_isr_handler(void *arg);

#endif