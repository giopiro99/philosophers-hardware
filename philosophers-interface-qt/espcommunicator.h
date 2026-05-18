#ifndef ESPCOMMUNICATOR_H
#define ESPCOMMUNICATOR_H

#include <QObject>
#include "sessioncommand.h"
#include <QtSerialPort>
#include <QStringList>

class EspCommunicator : public QObject
{
    Q_OBJECT
public:
    explicit        EspCommunicator(QObject *parent = nullptr);
    virtual         ~EspCommunicator();
    void            sendCommand( const SessionCommand& );
    void            setPort( const QString& );
    QStringList     getAvailablePorts() const;
    void            disconnectPort();
private:
    QString port;
    void    writeSerialReset();
    void    writeSerialStop();
    void    writeSerialStart();

    std::unique_ptr<QSerialPort>   serialPort;

signals:
    void    logsReady( const QString& );

public slots:
    void    onValueRecived();
};

#endif // ESPCOMMUNICATOR_H
