#include "philoLibrary.h"

QueueHandle_t led_queue = NULL;
volatile bool is_running = true;

static inline void createTasks(t_philo *philo){
    for (int i = 0; i < PHILO_NUMBER; i++){
        BaseType_t xReturned = xTaskCreate(
            (TaskFunction_t)vTaskRoutine,
            PHILO_TAG,
            2000,
            &philo[i],
            3,
            NULL
        );
        if (!xReturned){
            ESP_LOGE(ERROR_TAG, "Error in philo task creation");
            esp_restart();
        }
    }
}

void    app_main(){
    led_queue = xQueueCreate(10, sizeof(t_led_msg));
    BaseType_t xReturned = xTaskCreate(
        (TaskFunction_t)vTaskLedManager,
        LED_TAG,
        2000,
        NULL,
        3,
        NULL
    );

    if (!xReturned){
        ESP_LOGE(ERROR_TAG, "Error in led manager creation");
        esp_restart();
    }
    else{
        ESP_LOGI(SUCCESS_TAG, "Success led manager creation");
    }

    static t_philo             philos[PHILO_NUMBER];
    static SemaphoreHandle_t   forks[PHILO_NUMBER];

    
    setup(philos, forks);
    createTasks(philos);
}