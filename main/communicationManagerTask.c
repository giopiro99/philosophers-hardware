#include "communication_manager.h"

void vTaskCommunicationManager(void *params){
	uint8_t ch = 0;
     while(1) {
		//questa funzione non bloccante serve per appunto non impedire il proseguio del task, ritorna il numeri di byte letti
		//mette cio' che e' stato letto dentro ch
	    int rxBytes = uart_read_bytes(UART_NUM_0, &ch, 1, pdMS_TO_TICKS(20));
	    if (rxBytes > 0)
	    {
			if (ch == 'S'){
				xEventGroupClearBits(system_events, RUNNING_BIT);
				ESP_LOGI(COMMUNICATION_TAG, "Sessione stoppata via COMUNICAZIONE");
			}
			else if (ch == 'G'){
				xEventGroupSetBits(system_events, RUNNING_BIT);
				ESP_LOGI(COMMUNICATION_TAG, "Sessione attivata via COMUNICAZIONE");
			}
			else if (ch == 'R'){
				have_to_restart = true;
				EventBits_t bits_value = xEventGroupGetBits(system_events);
				if ((bits_value & RUNNING_BIT) == 0)
					xEventGroupSetBits(system_events, RUNNING_BIT);
				
				for (int i = 0; i < PHILO_NUMBER; i++){
					xSemaphoreTake(restart_sync_semaphore, portMAX_DELAY);
				}

				have_to_restart = false;
				xEventGroupClearBits(system_events, RUNNING_BIT);

				for (int i = 0; i < PHILO_NUMBER; i++){
					xSemaphoreGive(go_sync_semaphore);
				}
			}

			uart_flush(UART_NUM_0);
	    }
		vTaskDelay(pdMS_TO_TICKS(50));
    }
}