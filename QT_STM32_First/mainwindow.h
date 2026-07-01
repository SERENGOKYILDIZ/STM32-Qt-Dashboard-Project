#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QSerialPort> //For SerialPort

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_btnLedYak_clicked();

    void on_btnLedSondur_clicked();

    void readSerial(); // For STM32

private:
    Ui::MainWindow *ui;

    QSerialPort *serial;    // for object Seriport
    QByteArray serialData;  // For buffer
};
#endif // MAINWINDOW_H
