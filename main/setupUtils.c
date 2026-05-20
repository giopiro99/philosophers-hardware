#include "setup.h"

static inline void  createForks(SemaphoreHandle_t *forks){
    for (int i = 0; i < PHILO_NUMBER; i++){
        forks[i] = xSemaphoreCreateMutex();
        if (!forks[i]){
            ESP_LOGE(ERROR_TAG, "error in mutex creation");
            esp_restart();
        }
        else{
            ESP_LOGI(SUCCESS_TAG, "MUTEX CREATION");
        }
    }
}

static inline void  initPhilosValues(t_philo *philos, SemaphoreHandle_t *forks){
    for (int i = 0; i < PHILO_NUMBER; i++){
        philos[i].id = i;
        philos[i].eat_count = 0;
        philos[i].sx_fork = forks[i];
        philos[i].dx_fork = forks[((i + 1) % PHILO_NUMBER)];
    }
    ESP_LOGI(SUCCESS_TAG, "philosophers initialization");

}

void  setup(t_philo *philos, SemaphoreHandle_t *forks){
    createForks(forks);
    initPhilosValues(philos, forks);
}