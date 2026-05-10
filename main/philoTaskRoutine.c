#include "philoLibrary.h"

static inline void    setForks(t_philo *philo, SemaphoreHandle_t *first_fork, SemaphoreHandle_t *second_fork){
    if (philo->id % 2 == 0){
        *first_fork = philo->dx_fork;
        *second_fork = philo->sx_fork;
        return ;
    }

    *first_fork = philo->sx_fork;
    *second_fork = philo->dx_fork;
    return ;
}

static inline void    eatRoutine(t_philo *philo, SemaphoreHandle_t first_fork, SemaphoreHandle_t second_fork){
    t_led_msg msg = {.philoId = philo->id, .status = 1};
    xQueueSend(led_queue, &msg, 0);
    philo->eat_count++;
    vTaskDelay(pdMS_TO_TICKS(TIME_TO_EAT + (philo->eat_count * 0.1)));
    xSemaphoreGive(first_fork);
    xSemaphoreGive(second_fork);
    ESP_LOGI(DEBUG_TAG, "philo number %d has eated n=%d", philo->id, philo->eat_count);
    msg.status = 0;
    xQueueSend(led_queue, &msg, 0);
    vTaskDelay(pdMS_TO_TICKS(TIME_TO_THINK));
}

static inline void    thinkRoutine(t_philo *philo){
    t_led_msg msg = {.philoId = philo->id, .status = 0};
    xQueueSend(led_queue, &msg, 0);
    vTaskDelay(pdMS_TO_TICKS(TIME_TO_THINK));
    ESP_LOGI(DEBUG_TAG, "philo number %d is thinking", philo->id);
}

void    vTaskRoutine(void *params){
    t_philo *philo = (t_philo *)params;
    SemaphoreHandle_t first_fork;
    SemaphoreHandle_t second_fork;

    setForks(philo, &first_fork, &second_fork);

    while (1){
        if (!is_running){
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        if (xSemaphoreTake(first_fork, pdMS_TO_TICKS(500)) == pdTRUE){
            if (xSemaphoreTake(second_fork, pdMS_TO_TICKS(500)) == pdTRUE){
                eatRoutine(philo, first_fork, second_fork);
            }
            else{
                xSemaphoreGive(first_fork);
                thinkRoutine(philo);
            }
        }
        else{
            thinkRoutine(philo);
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}