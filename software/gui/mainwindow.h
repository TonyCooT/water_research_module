/***************************************************
 Water Research Module
 <https://github.com/TonyCooT/water_research_module>
 
 ***************************************************
 The code below is a application designed to work as part of a water research module. 
 The application communicates with the microcontroller via the serial port and allows monitoring the operation of sensors.

 Created 2022-09-28
 By TonyCooT <https://github.com/TonyCooT>
 
 Apache License 2.0.
 See <https://www.apache.org/licenses/> for details.
 All above must be included in any redistribution.
 ****************************************************/
 
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <algorithm>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;

    struct SerialSettings
    {
        QSerialPort::BaudRate baudRate;
        QSerialPort::DataBits dataBits;
        QSerialPort::Parity parity;
        QSerialPort::StopBits stopBits;
        QSerialPort::FlowControl flowControl;
        QIODeviceBase::OpenMode mode;
    };

    struct MessageSettings
    {
        quint8 prefix;
        quint8 headerSize;
        quint8 msgInSize;
        quint8 msgOutSize;
        quint8 dataSize;
    };

    struct PlotSettings
    {
        quint8 typeSensor;
        QString unit;
        qint16 min;
        qint16 max;
    };

    enum class sensor_type: quint8 {TEMP,PH,TDS};
    enum class cmd_type: quint8 {RESET,CAL,CAL_LOW,CAL_MIDDLE,CAL_HIGH,MODE};
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void open_serial();
    void close_serial();
    void calibrate_ph();
    void calibrate_tds();
    void change_mode();
    void reset_calibraion(quint8 type);
    void parse_data();

private:
    void set_serial();
    void add_value(QVector<double> *arr, double value);
    void plot(QCustomPlot *plot, const QVector<double> *arr, const Ui::PlotSettings *settings);
    quint8 get_sum(const quint8 *arr, quint8 len);
    void send_data(quint8 typeSensor, const quint8 *data);

private:
    Ui::MainWindow *ui;

private:
    QSerialPort *m_serialPort;
    Ui::SerialSettings m_serialSettings;
    Ui::MessageSettings m_messageSettings;

private:
    QVector<double> m_t;
    QVector<double> m_temp;
    QVector<double> m_ph;
    QVector<double> m_tds;
    Ui::PlotSettings m_tempPlotSettings;
    Ui::PlotSettings m_phPlotSettings;
    Ui::PlotSettings m_tdsPlotSettings;
};
#endif // !MAINWINDOW_H
