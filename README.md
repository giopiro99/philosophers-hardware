<div align="center">
  <video src="https://github.com/user-attachments/assets/c2ed1f97-16b9-4444-9d1a-c141c857b216" width="48%" controls></video>
  <video src="https://github.com/user-attachments/assets/edbcb21a-7bd8-44e8-af25-64642895c919" width="48%" controls></video>
</div>

# Problema dei Filosofi a Cena (ESP32 / FreeRTOS)

## Descrizione

Questo progetto implementa il classico problema di sincronizzazione dei "Filosofi a cena" (Dining Philosophers) su un microcontrollore ESP32, utilizzando il framework ESP-IDF e il sistema operativo real-time FreeRTOS. L'obiettivo è dimostrare la corretta gestione della concorrenza, la prevenzione dei deadlock e la comunicazione thread-safe con l'hardware esterno.

Il progetto simula 5 filosofi che alternano fasi di pensiero e fasi di pasto. Per mangiare, ogni filosofo deve acquisire due "forchette" (risorse condivise). Lo stato di alimentazione di ciascun filosofo è rappresentato visivamente tramite un LED fisico dedicato.

## Mappatura Hardware (GPIO)

Il sistema utilizza 5 LED collegati ai pin GPIO dell'ESP32 e un bottone fisico per il controllo. L'associazione hardware è la seguente:

* **Filosofo 0:** GPIO 13
* **Filosofo 1:** GPIO 12
* **Filosofo 2:** GPIO 14
* **Filosofo 3:** GPIO 27
* **Filosofo 4:** GPIO 26
* **Bottone (Controllo Simulazione):** GPIO 25

## Architettura e Meccanismi FreeRTOS

L'applicazione è strutturata in diversi file sorgente per separare la logica di sincronizzazione da quella di gestione hardware e comunicazione.

### Struttura dei Task

* **Task Filosofi (5 istanze):** Definiti in `philoTaskRoutine.c`. Ogni filosofo è un task indipendente. Per evitare deadlock circolari, è stata implementata un'acquisizione asimmetrica delle risorse (forchette): i filosofi con ID pari tentano di prendere prima la forchetta destra e poi la sinistra, mentre i dispari fanno l'opposto.
* **Task LedManager (1 istanza):** Definito in `ledManagerTask.c`. È un singolo task centralizzato dedicato esclusivamente al controllo dei GPIO in output. Intercetta i cambiamenti di stato dei filosofi e aggiorna i LED di conseguenza.
* **Task ButtonManager (1 istanza):** Definito in `buttonManagerTask.c`. Questo task gestisce lo stato di Start/Stop della simulazione. Vive in uno stato "dormiente" (bloccato) consumando zero cicli CPU finché non riceve un segnale dall'Interrupt Service Routine (ISR) del bottone fisico.
* **Task CommunicationManager (1 istanza):** Definito in `communicationManagerTask.c`. Gestisce la comunicazione seriale bidirezionale (UART). Legge in modo non bloccante i byte in arrivo dalla porta seriale per intercettare i comandi di controllo remoti ('S' per Stop, 'R' per Restart, 'G' per Go).

### Sincronizzazione e Comunicazione

* **Mutex (Semafori):** Le 5 forchette sono implementate utilizzando Mutex di FreeRTOS (`xSemaphoreCreateMutex`). Garantiscono l'accesso esclusivo alla risorsa.
* **Code (Queues):** Per evitare condizioni di race nell'accesso all'hardware, i task dei filosofi non accendono i LED direttamente. Inviano una struttura dati (`t_led_msg`) tramite una coda FreeRTOS (`xQueueSend`) al LedManager, elaborando le richieste in modo asincrono.
* **Interrupt e Semafori Binari (Deferred Interrupt Processing):** La pressione del bottone genera un interrupt hardware (ISR). Poiché le operazioni all'interno di un'ISR devono essere minime e velocissime, la routine si limita a eseguire un debouncing software (250ms) e a "sbloccare" un Semaforo Binario (`xSemaphoreGiveFromISR`). Il `Task ButtonManager`, in ascolto su quel semaforo, si risveglia istantaneamente, inverte lo stato della simulazione e torna a dormire.
* **Comunicazione UART:** L'interfaccia UART0 è configurata a 115200 baud rate. Il task di comunicazione permette di sovrascrivere o affiancare l'input fisico del bottone, abilitando il controllo remoto del sistema da un PC tramite un terminale o un'interfaccia grafica dedicata.

## Struttura del Codice

* `main/philosophers.c`: Entry point dell'applicazione (`app_main`). Crea la coda, inizializza le periferiche (UART, GPIO per ISR) e delega il setup dei task.
* `main/setupUtils.c`: Gestisce l'allocazione dei Mutex e l'inizializzazione delle strutture dati dei filosofi.
* `main/philoTaskRoutine.c`: Contiene la routine principale (Eat/Think) e la logica di gestione dei semafori.
* `main/ledManagerTask.c`: Task in ascolto passivo sulla coda per aggiornare i LED fisici.
* `main/buttonManagerTask.c`: Contiene l'Interrupt Service Routine (ISR) per il bottone e il task associato per il cambio di stato del sistema.
* `main/communicationManagerTask.c`: Contiene la logica per leggere e interpretare i comandi UART in ingresso.
* `main/include/philoLibrary.h`: Header principale contenente definizioni di costanti, macro e strutture dati condivise.

## Requisiti e Compilazione

Il progetto richiede un ambiente ESP-IDF funzionante.

Per compilare, flashare sul dispositivo e monitorare l'output seriale, eseguire i seguenti comandi nella radice del progetto:

```bash
# Impostare il target (es. esp32)
idf.py set-target esp32

# Compilare il progetto
idf.py build

# Eseguire il flash e aprire il monitor seriale
idf.py flash monitor
