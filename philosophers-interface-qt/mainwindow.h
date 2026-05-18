#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QComboBox>
#include "espcommunicator.h"
#include <QVBoxLayout>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QPushButton     *connectPortButton = nullptr;
    QComboBox       *portSelector = nullptr;
    QPushButton     *resetSimulationButton = nullptr;
    QPushButton     *stopSimulationButton = nullptr;
    QPushButton     *startSimulationButton = nullptr;
    QPushButton     *disconnectButton = nullptr;
    QPlainTextEdit  *displayLogs = nullptr;

    std::unique_ptr<EspCommunicator> serialComunicator;

    void    connectAll();
    void    addWidgetsToLayout(QVBoxLayout *layout);

public slots:
    void    onLogsRecived( const QString& );
};
#endif // MAINWINDOW_H
