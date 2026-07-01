#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    serial = new QSerialPort(this);
    // STM32 Connection Settings
    serial->setPortName("COM11"); //(example: COM4, COM5)
    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);

    // Open the port and write the connection status to the Label
    if(serial->open(QIODevice::ReadWrite)) {
        ui->lblDurum->setText("Bağlantı Başarılı (COM11)");
        ui->lblDurum->setStyleSheet("color: green;");
    } else {
        ui->lblDurum->setText("Bağlantı Başarısız!");
        ui->lblDurum->setStyleSheet("color: red;");
    }

    // Trigger the readSerial function as data arrives from the STM32
    connect(serial, &QSerialPort::readyRead, this, &MainWindow::readSerial);

}

MainWindow::~MainWindow()
{
    if(serial->isOpen()) {
        serial->close();
    }
    delete ui;
}

// PC -> STM32: Data Transmission (LED YAK)
void MainWindow::on_btnLedYak_clicked()
{
    if(serial->isOpen()) {
        serial->write("1");
    }
}

// PC -> STM32: Data Transmission (LED SÖNDÜR)
void MainWindow::on_btnLedSondur_clicked()
{
    if(serial->isOpen()) {
        serial->write("0");
    }
}

// STM32 -> PC: Data Acquisition and Display
void MainWindow::readSerial()
{
    // Add the new data to the buffer
    serialData.append(serial->readAll());

    // Wait until a line break (\n) is encountered (to ensure that all the data has been received)
    if(serialData.contains('\n')) {
        QString dataStr = QString::fromUtf8(serialData).trimmed();
        serialData.clear(); // Clean the pad for the next pack

        // Protocol check: Does the data begin with ‘S:’? (e.g. S:1024)
        if(dataStr.startsWith("S:")) {
            int sensorValue = dataStr.mid(2).toInt(); // Skip the "S:" part and convert the rest to a number
            ui->barSensor->setValue(sensorValue);     // Update the Progress Bar value
        }
    }
}

