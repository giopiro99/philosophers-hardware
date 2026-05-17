#include "philoLibrary.h"

IRAM_ATTR void button_isr_handler(void *arg){
    //debauncing per il rumore elettrico legato al bottone fisico
    static uint32_t last_isr_time = 0;
    uint32_t current_time = esp_timer_get_time() / 1000;

    if (current_time - last_isr_time > 250){
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(button_semaphore, &xHigherPriorityTaskWoken);
        if (xHigherPriorityTaskWoken){
            portYIELD_FROM_ISR();
        }
        last_isr_time = current_time;
    }
}

void    vTaskButtonManager(void *params){
    while(1)
    {
        //il task dorme finche' non viene rilasciato il semaforo dall INTERRUPT
        if (xSemaphoreTake(button_semaphore, portMAX_DELAY) == pdTRUE){
            is_running = !is_running;
            if(is_running) {
                ESP_LOGI(BUTTON_TAG, "Bottone premuto: SIMULAZIONE RIPARTITA");
            }
            else{
                ESP_LOGI(BUTTON_TAG, "Bottone premuto: SIMULAZIONE FERMATA");
            }
        }
    }
}