#include "mainwindow.h"

void MainWindow::connectAll(){
    //connectionButton premuto:
    //settare porta,disabilitare connectionButton e portSelector
    //abilitare bottoni di gestione del progetto esp32 + disconnectionButton
    connect(this->connectPortButton, &QPushButton::clicked, this, [this](){
        QString currentText = this->portSelector->currentText();
        if (!currentText.isEmpty()){
            this->serialComunicator->setPort(currentText);
            this->connectPortButton->setEnabled(false);
            this->portSelector->setEnabled(false);
            this->disconnectButton->setEnabled(true);
            this->startSimulationButton->setEnabled(true);
            this->stopSimulationButton->setEnabled(true);
            this->resetSimulationButton->setEnabled(true);
        }
    });

    //disconnectionButton premuto:
    //disconnettere porta,abilitare connectionButton e portSelector
    //disabilitare bottoni di gestione del progetto esp32 + disconnectionButton
    connect(this->disconnectButton, &QPushButton::clicked, this, [this](){
        this->serialComunicator->disconnectPort();
        this->connectPortButton->setEnabled(true);
        this->portSelector->setEnabled(true);
        this->disconnectButton->setEnabled(false);
        this->startSimulationButton->setEnabled(false);
        this->stopSimulationButton->setEnabled(false);
        this->resetSimulationButton->setEnabled(false);
    });

    //connessione tra bottoni e espCommunicator che gestisce la logica
    connect(this->resetSimulationButton, &QPushButton::clicked, this, [this](){
        this->serialComunicator->sendCommand(SessionCommand::Reset);
    });
    connect(this->stopSimulationButton, &QPushButton::clicked, this, [this]{
        this->serialComunicator->sendCommand(SessionCommand::Stop);
    });
    connect(this->startSimulationButton, &QPushButton::clicked, this, [this]{
        this->serialComunicator->sendCommand(SessionCommand::Start);
    });

    //collego al segnale logsReady lo slot onLogsRecived
    connect(this->serialComunicator.get(), &EspCommunicator::logsReady, this, &MainWindow::onLogsRecived);
}

void MainWindow::addWidgetsToLayout(QVBoxLayout *layout){
    layout->addWidget(this->portSelector);
    layout->addWidget(this->connectPortButton);
    layout->addWidget(this->disconnectButton);
    layout->addWidget(this->resetSimulationButton);
    layout->addWidget(this->stopSimulationButton);
    layout->addWidget(this->startSimulationButton);
    layout->addWidget(this->displayLogs);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    this->portSelector = new QComboBox(this);
    this->connectPortButton = new QPushButton("Connect this port", this);
    this->disconnectButton = new QPushButton("Disconnect this port", this);
    this->resetSimulationButton = new QPushButton("Reset Simulation", this);
    this->stopSimulationButton = new QPushButton("Stop Simulation", this);
    this->startSimulationButton = new QPushButton("Start Simulation", this);
    this->displayLogs = new QPlainTextEdit(this);
    this->serialComunicator = std::make_unique<EspCommunicator>();

    this->displayLogs->setReadOnly(true);
    this->disconnectButton->setEnabled(false);
    this->startSimulationButton->setEnabled(false);
    this->stopSimulationButton->setEnabled(false);
    this->resetSimulationButton->setEnabled(false);

    addWidgetsToLayout(layout);
    setCentralWidget(centralWidget);

    this->portSelector->addItems(this->serialComunicator->getAvailablePorts());

    this->connectAll();
}

void MainWindow::onLogsRecived(const QString& logs){
    if (logs.isEmpty()) return ;

    this->displayLogs->appendPlainText(logs);
}

MainWindow::~MainWindow()
{}

