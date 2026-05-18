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
```

## Interfaccia Grafica di Controllo (Qt)

Per monitorare e controllare la simulazione da PC in tempo reale, è stata sviluppata un'applicazione desktop multipiattaforma scritta in C++ utilizzando il framework Qt. L'interfaccia si connette e comunica in modo bidirezionale con l'ESP32 tramite la porta seriale (UART), interfacciandosi direttamente con il modulo di comunicazione del microcontrollore.

### Architettura del Frontend (`MainWindow`)

L'interfaccia grafica è stata interamente sviluppata tramite codice (senza l'ausilio dello strumento visivo Qt Designer), garantendo una gestione flessibile del ciclo di vita dei widget e un'architettura modulare. Il codice divide nettamente la gestione del layout visivo dalla logica dei segnali:

* **`addWidgetsToLayout`**: Funzione dedicata alla disposizione sequenziale dei componenti all'interno di un'impaginazione verticale (`QVBoxLayout`). Coordina l'ordine visivo posizionando i controlli di connessione in cima, i controlli operativi della simulazione al centro e il terminale dei log in basso.
* **`connectAll`**: Centralizza tutte le connessioni Signal/Slot di Qt, sfruttando la sintassi moderna con funzioni Lambda per inoltrare i comandi dell'utente al modulo di comunicazione e per aggiornare dinamicamente gli stati dell'interfaccia grafica.
* **Terminale di Log (`QPlainTextEdit`)**: Configurato in modalità di sola lettura (`setReadOnly(true)`) per ricevere e accodare i messaggi asincroni trasmessi dall'ESP32 senza bloccare il thread principale della GUI.

### Gestione della Comunicazione Seriale (`EspCommunicator`)

La logica di interazione con l'hardware e la gestione del protocollo seriale sono interamente incapsulate all'interno di un backend dedicato che adotta i paradigmi del C++ moderno e la gestione sicura delle risorse:

* **Rilevamento hardware dinamico**: Sfrutta la classe `QSerialPortInfo` per scansionare le periferiche del sistema operativo, estraendo i nomi delle porte attive (es. COM o ttyUSB) per popolare automaticamente la selezione a tendina (`QComboBox`) del frontend.
* **Gestione della memoria e RAII**: L'istanza della porta seriale viene gestita tramite un puntatore intelligente (`std::unique_ptr<QSerialPort>`). Il ciclo di vita della risorsa è protetto mediante l'implementazione esplicita del distruttore della classe, assicurando la chiusura della porta seriale logica in caso di arresto improvviso del programma per evitare file descriptor rimasti aperti nel sistema host.
* **Lettura asincrona orientata alla riga**: Lo slot agganciato al segnale nativo `readyRead` esegue una lettura paranoica del buffer tramite un ciclo basato su `canReadLine()`. Questo meccanismo accumula i byte in ingresso e li estrae tramite `readLine()` solo quando è presente un carattere di terminazione (`\n`). I dati grezzi vengono poi convertiti in formato stringa UTF-8 e trasmessi al frontend tramite il segnale personalizzato `logsReady`, evitando la frammentazione del testo a schermo.
* **Astrazione dei comandi**: I tre pulsanti operativi interagiscono con il backend tramite un'enumerazione fortemente tipizzata (`enum class SessionCommand`). La classe si occupa di tradurre i tipi astratti (`Start`, `Stop`, `Reset`) nei corrispettivi codici binari a singolo byte (`'G'`, `'S'`, `'R'`) attesi dal firmware.

### Gestione degli Stati e Sicurezza dell'Interfaccia

Per prevenire errori di interazione o crash dell'applicazione dovuti all'invio di comandi su canali non ancora configurati, l'applicazione implementa una macchina a stati logica sui widget attraverso la funzione `setEnabled(bool)`:

* **Stato Disconnesso (Avvio dell'applicazione)**: Il selettore della porta e il pulsante "Connect" sono abilitati. Al contrario, il pulsante "Disconnect" e i tre pulsanti della simulazione (Start, Stop, Reset) sono disattivati e visualizzati in grigio. L'utente è guidato a eseguire la connessione come prima mossa.
* **Stato Connesso**: Una volta stabilito il collegamento logico con la porta seriale, lo stato visivo si inverte. Il selettore di porta e il pulsante di connessione vengono bloccati per impedire modifiche a sessione attiva. Simultaneamente, vengono sbloccati il pulsante di disconnessione e i comandi operativi, consentendo il pieno controllo remoto dei task dei filosofi in esecuzione sull'ESP32.
