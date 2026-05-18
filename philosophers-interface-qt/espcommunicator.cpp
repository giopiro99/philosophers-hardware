#include "espcommunicator.h"
#include <QDebug>
EspCommunicator::EspCommunicator(QObject *parent)
    : QObject{parent}, serialPort(std::make_unique<QSerialPort>())
{
    port = "";
    serialPort->setBaudRate(QSerialPort::Baud115200);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);

    connect(this->serialPort.get(), &QSerialPort::readyRead, this, &EspCommunicator::onValueRecived);
}

void    EspCommunicator::sendCommand(const SessionCommand& cmd){
    if (cmd == SessionCommand::Start){
        this->writeSerialStart();
    }
    else if (cmd == SessionCommand::Stop){
        this->writeSerialStop();
    }
    else {
        this->writeSerialReset();
    }
}

void    EspCommunicator::writeSerialReset(){
    const char data = static_cast<char>(SessionCommand::Reset);
    serialPort->write(&data, sizeof(char));
}

void    EspCommunicator::writeSerialStop(){
    const char data = static_cast<char>(SessionCommand::Stop);
    serialPort->write(&data, sizeof(char));
}

void    EspCommunicator::writeSerialStart(){
    const char data = static_cast<char>(SessionCommand::Start);
    serialPort->write(&data, sizeof(char));
}

void    EspCommunicator::setPort(const QString& portName){
    if (portName.isEmpty()){
        qDebug() << "invalid port name, program in shutdown";
        exit(1);
    }

    this->port = portName;
    if (serialPort->isOpen()){
        serialPort->close();
    }

    serialPort->setPortName(this->port);
    if(serialPort->open(QIODevice::ReadWrite)){
        qDebug() << "connected to port: " << this->port;
    }
    else{
        qDebug() << "connection failed to port" << this->port;
        exit(1);
    }
}

QStringList    EspCommunicator::getAvailablePorts() const{
    QStringList             portList;

    for (const QSerialPortInfo &info : QSerialPortInfo::availablePorts()) {
        portList.append(info.portName());
    }
    return portList;
}

void    EspCommunicator::onValueRecived(){
    while(serialPort->canReadLine()){
        QByteArray data = serialPort->readLine();

        // La trasforma in testo
        QString value = QString::fromUtf8(data).trimmed();
        if (value.isEmpty()) continue;
        emit logsReady(value);
    }
}

void    EspCommunicator::disconnectPort(){
    if (this->serialPort->portName().isEmpty()) return;
    this->serialPort->close();
}

EspCommunicator::~EspCommunicator() {
    if (serialPort && serialPort->isOpen()) {
        serialPort->close();
    }
}
