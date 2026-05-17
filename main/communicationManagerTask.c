#include "philoLibrary.h"

void vTaskCommunicationManager(void *params){
	uint8_t ch = 0;
     while(1) {
		//questa funzione non bloccante serve per appunto non impedire il proseguio del task, ritorna il numeri di byte letti
		//mette cio' che e' stato letto dentro ch
	    int rxBytes = uart_read_bytes(UART_NUM_0, &ch, 1, pdMS_TO_TICKS(20));
	    if (rxBytes > 0)
	    {
			if (is_running){
				if (ch == 'S'){
					is_running = false;
					ESP_LOGI(COMMUNICATION_TAG, "Sessione stoppata via COMUNICAZIONE");
				}
				else if (ch == 'R'){
					have_to_restart = true;
					while(philos_restarted < PHILO_NUMBER){
						vTaskDelay(pdMS_TO_TICKS(25));
					}

					philos_restarted = 0;
					have_to_restart = false;
					ESP_LOGI(COMMUNICATION_TAG, "Sessione restartata con successo via COMUNICAZIONE");
				}
			}
			else if (!is_running && ch == 'G'){
				is_running = true;
				ESP_LOGI(COMMUNICATION_TAG, "Sessione attivata via COMUNICAZIONE");
			}

			uart_flush(UART_NUM_0);
	    }
		vTaskDelay(pdMS_TO_TICKS(50));
    }
}