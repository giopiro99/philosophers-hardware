#include "led_manager.h"

void    vTaskLedManager(void *params){

    ESP_LOGI(LED_TAG, "Task LED Manager avviato correttamente!");
    t_led_msg   recived;
    int pins[] = {13, 12, 14, 27, 26};

    for (int i = 0; i < PHILO_NUMBER; i++){

        if (gpio_reset_pin(pins[i]) != ESP_OK)
        {
            ESP_LOGE(LED_TAG, "Error in gpio_reset_pin");
            esp_restart();
        }
        if (gpio_set_direction(pins[i], GPIO_MODE_OUTPUT) != ESP_OK){
            ESP_LOGE(LED_TAG, "Error in gpio_set_direction");
            esp_restart();
        }
    }
    while (1){
        if(xQueueReceive(led_queue, &recived, portMAX_DELAY) == pdTRUE){
            gpio_set_level(pins[recived.philoId], recived.status);
            ESP_LOGI(LED_TAG, "Filosofo %d -> Pin %d -> Stato %d", 
                recived.philoId, pins[recived.philoId], recived.status);
        }
    }
}