#include "philoLibrary.h"

//Inizializzazione delle variabili "globali",
//led_queue utilizzata per cominicare tra i task e far capire al vTaskLedManager quale pin attivare e disattivare
//is_running verra' utilizzata sia per intercettare "l'evento" QT per stoppare la simulazione sia per il bottone fisico montato sulla breadboard gestito da un interrupt
//have_to_restart verra' utilizzata per "l'evento" QT che avra' come messaggio 'R' per resettare i valori della simulazione

QueueHandle_t                       led_queue = NULL;
volatile bool                       have_to_restart = false;
SemaphoreHandle_t                   restart_sync_semaphore = NULL;
SemaphoreHandle_t                   go_sync_semaphore = NULL;
SemaphoreHandle_t                   button_semaphore = NULL;

//CREATE PHILOS TASKS UTILS
static void initPhilosTasks(t_philo *philo){
    for (int i = 0; i < PHILO_NUMBER; i++){
        BaseType_t xReturned = xTaskCreate(
            (TaskFunction_t)vTaskRoutine,
            PHILO_TAG,
            2048,
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

static void initLedTask(){
    led_queue = xQueueCreate(10, sizeof(t_led_msg));
    if (!led_queue){
        ESP_LOGE(ERROR_TAG, "Error in led queue creation");
        esp_restart();
    }
    else{
        ESP_LOGI(SUCCESS_TAG, "Success led queue creation");
    }
    BaseType_t ledManager = xTaskCreate(
        (TaskFunction_t)vTaskLedManager,
        LED_TAG,
        2048,
        NULL,
        3,
        NULL
    );

    if (!ledManager){
        ESP_LOGE(ERROR_TAG, "Error in led manager creation");
        esp_restart();
    }
    else{
        ESP_LOGI(SUCCESS_TAG, "Success led manager creation");
    }
}

static void setupUARTcommunication() {
    uart_config_t uart_config = {
		//baud_rate: La velocità. 115200 bit al secondo. PC ed ESP32 devono essere sincronizzati
        .baud_rate = 115200,
		//il messaggio ricevuto sara' di 8 bit (1 byte = 1 char)
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    
    // Configura i parametri UART1 e controlla eventuali errori
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_0, &uart_config));
    
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    
    // Installa il driver UART. 
    // I parametri sono: Port, RX Buffer Size, TX Buffer Size, Queue Size, Queue Handle, Flags
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_0, 256, 0, 0, NULL, 0));
}

static void initCommunicationTask(){
    restart_sync_semaphore = xSemaphoreCreateCounting(PHILO_NUMBER, 0);
    if (!restart_sync_semaphore){
        ESP_LOGE(ERROR_TAG, "Error in philos_restarted mutex creation");
        esp_restart();
    }

    go_sync_semaphore = xSemaphoreCreateCounting(PHILO_NUMBER, 0);
    if (!go_sync_semaphore){
        ESP_LOGE(ERROR_TAG, "Error in philos_restarted mutex creation");
        esp_restart();
    }

    setupUARTcommunication();

    BaseType_t communicationManager = xTaskCreate(
        (TaskFunction_t)vTaskCommunicationManager,
        COMMUNICATION_TAG,
        2048,
        NULL,
        2,
        NULL
    );
    if (!communicationManager){
        ESP_LOGE(ERROR_TAG, "Error in communication manager creation");
        esp_restart();
    }
}

static void setupButton(){
    gpio_config_t   button_config = {
        .intr_type = GPIO_INTR_NEGEDGE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << BUTTON_PIN),
        .pull_down_en = 0,
        .pull_up_en = 1
    };

    gpio_config(&button_config);

    button_semaphore = xSemaphoreCreateBinary();
    if (!button_semaphore){
        ESP_LOGE(ERROR_TAG, "Error in philos_restarted mutex creation");
        esp_restart();
    }
    else{
        ESP_LOGI(SUCCESS_TAG, "Succes button_semaphore creation");
    }

    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    ESP_ERROR_CHECK(gpio_isr_handler_add(BUTTON_PIN, button_isr_handler, NULL));
}

static void initButtonTask(){
    setupButton();
    BaseType_t buttonManager = xTaskCreate(
        (TaskFunction_t)vTaskButtonManager,
        BUTTON_TAG,
        2048,
        NULL,
        5,
        NULL
    );
    if (!buttonManager){
        ESP_LOGE(ERROR_TAG, "Error in communication manager creation");
        esp_restart();
    }
    else{
        ESP_LOGI(SUCCESS_TAG, "Succes button task creation");
    }
}

//MAIN
void    app_main(){
    static t_philo             philos[PHILO_NUMBER];
    static SemaphoreHandle_t   forks[PHILO_NUMBER];

    //inizializzazione dei task ecc
    setup(philos, forks);
    initButtonTask();
    initLedTask();
    initCommunicationTask();

    //evento per far stoppare/partire i task senza spreco di CPU
    system_events = xEventGroupCreate();
    xEventGroupSetBits(system_events, RUNNING_BIT);

    initPhilosTasks(philos);
}