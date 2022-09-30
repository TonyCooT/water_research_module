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
 
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_serialPort = new QSerialPort(this);

    /* Constant settings */
    m_serialSettings = {QSerialPort::Baud115200, QSerialPort::Data8, QSerialPort::NoParity,
                        QSerialPort::OneStop, QSerialPort::NoFlowControl, QIODevice::ReadWrite};

    m_messageSettings = {.prefix = 0x53, .headerSize = 2, .msgInSize = 5, .msgOutSize = 6, .dataSize = 3};

    m_tempPlotSettings = {static_cast<quint8>(Ui::sensor_type::TEMP), "°C", -55, 125};
    m_phPlotSettings = {static_cast<quint8>(Ui::sensor_type::PH), "pH", 0, 14};
    m_tdsPlotSettings = {static_cast<quint8>(Ui::sensor_type::TDS), "ppm", 0, 1250};

    /* Communications */
    connect(m_serialPort, SIGNAL(readyRead()), this, SLOT(parse_data()));
    connect(ui->SerialOpen, SIGNAL(clicked(bool)), this, SLOT(open_serial()));
    connect(ui->SerialClose, SIGNAL(clicked(bool)), this, SLOT(close_serial()));
    connect(ui->ProbeSettingPhMode, SIGNAL(stateChanged(int)), this, SLOT(change_mode()));
    connect(ui->ProbeSettingPhCalEnter, SIGNAL(clicked(bool)), this, SLOT(calibrate_ph()));
    connect(ui->ProbeSettingTdsCalEnter, SIGNAL(clicked(bool)), this, SLOT(calibrate_tds()));
    connect(ui->ProbeSettingPhCalReset, &QPushButton::clicked, [this](){ reset_calibraion(static_cast<quint8>(Ui::sensor_type::PH)); });
    connect(ui->ProbeSettingTdsCalReset, &QPushButton::clicked, [this](){ reset_calibraion(static_cast<quint8>(Ui::sensor_type::TDS)); });

    /* Pre-configuration of data and GUI */
    set_serial();

    for(size_t i = 0; i < 100; ++i)
        m_t.push_back(i);

    m_temp.reserve(100);
    m_ph.reserve(100);
    m_tds.reserve(100);

    for(const auto& info : QSerialPortInfo::availablePorts())
        ui->SerialChoose->addItem(info.portName());

    for(const auto& type : {"Basic", "Low", "Middle", "High"})
        ui->ProbeSettingPhCalType->addItem(type);

    plot(ui->ProbePlotTemp, &m_temp, &m_tempPlotSettings);
    plot(ui->ProbePlotPh, &m_ph, &m_phPlotSettings);
    plot(ui->ProbePlotTds, &m_tds, &m_tdsPlotSettings);

    setWindowTitle("Water Research GUI");
}

MainWindow::~MainWindow()
{
    delete ui;

    if(m_serialPort->isOpen())
        m_serialPort->close();

    delete m_serialPort;
}

void MainWindow::open_serial()
{
    m_serialPort->setPortName(ui->SerialChoose->currentText());

    if (!m_serialPort->open(m_serialSettings.mode))
         QMessageBox::warning(this, "Warning", "The serial is unavailable.");
}

void MainWindow::close_serial()
{
    if(m_serialPort->isOpen())
    {
        m_serialPort->close();
        m_temp.clear();
        m_ph.clear();
        m_tds.clear();
    }
}

void MainWindow::calibrate_ph()
{
    QString text = ui->ProbeSettingPhCalType->currentText();
    quint8 cmd;

    if(text == "Basic")
        cmd = static_cast<quint8>(Ui::cmd_type::CAL);
    else if (text == "Low")
        cmd = static_cast<quint8>(Ui::cmd_type::CAL_LOW);
    else if (text == "Middle")
        cmd = static_cast<quint8>(Ui::cmd_type::CAL_MIDDLE);
    else if (text == "High")
        cmd = static_cast<quint8>(Ui::cmd_type::CAL_HIGH);

    quint8 data[m_messageSettings.dataSize] = {cmd, 0x00, 0x00};
    send_data(static_cast<quint8>(Ui::sensor_type::PH), data);
}

void MainWindow::calibrate_tds()
{
    QString text = ui->ProbeSettingTdsCalType->text();
    bool isCorrect = false;
    double value = text.toDouble(&isCorrect);

    if(text.isEmpty() || !isCorrect)
    {
        QMessageBox::warning(this, "Warning", "The value is incorrect.");
        ui->ProbeSettingTdsCalType->clear();
        return;
    }

    value *= 10.0;
    quint16 valueInt = value;

    quint8 data[m_messageSettings.dataSize] = {static_cast<quint8>(Ui::cmd_type::CAL),
                                               static_cast<quint8>(valueInt >> 8),
                                               static_cast<quint8>(valueInt & 0xFF)};
    send_data(static_cast<quint8>(Ui::sensor_type::TDS), data);
}

void MainWindow::change_mode()
{
    quint8 data[m_messageSettings.dataSize] = {static_cast<quint8>(Ui::cmd_type::MODE),
                                               static_cast<quint8>(ui->ProbeSettingPhMode->isChecked()),
                                               0x00};
    send_data(static_cast<quint8>(Ui::sensor_type::PH), data);
}

void MainWindow::reset_calibraion(quint8 type)
{
    quint8 data[m_messageSettings.dataSize] = {static_cast<quint8>(Ui::cmd_type::RESET), 0x00, 0x00};
    send_data(type, data);
}

void MainWindow::parse_data()
{
    quint8 message[m_messageSettings.msgInSize];
    m_serialPort->read(reinterpret_cast<char *>(message), m_messageSettings.msgInSize);

    if ((message[0] != m_messageSettings.prefix)
         || (message[m_messageSettings.msgInSize - 1] != get_sum(message, m_messageSettings.msgInSize - 1)))
        return;

    double value = (static_cast<quint16>(message[2]) << 8) + static_cast<quint16>(message[3]);
    value /= 10.0;

    if (message[1] == m_tempPlotSettings.typeSensor)
    {
        add_value(&m_temp, value);
        plot(ui->ProbePlotTemp, &m_temp, &m_tempPlotSettings);
    }
    else if (message[1] == m_phPlotSettings.typeSensor)
    {
        add_value(&m_ph, value);
        plot(ui->ProbePlotPh, &m_ph, &m_phPlotSettings);
    }
    else if (message[1] == m_tdsPlotSettings.typeSensor)
    {
        add_value(&m_tds, value);
        plot(ui->ProbePlotTds, &m_tds, &m_tdsPlotSettings);
    }
}

void MainWindow::set_serial()
{
    m_serialPort->setBaudRate(m_serialSettings.baudRate);
    m_serialPort->setDataBits(m_serialSettings.dataBits);
    m_serialPort->setParity(m_serialSettings.parity);
    m_serialPort->setStopBits(m_serialSettings.stopBits);
    m_serialPort->setFlowControl(m_serialSettings.flowControl);
}

void MainWindow::add_value(QVector<double> *arr, double value)
{
    if (arr->size() < 100)
        arr->push_back(value);
    else
    {
        std::rotate(arr->begin(), arr->begin() + 1, arr->end());
        (*arr)[arr->size() - 1] = value;
    }
}

void MainWindow::plot(QCustomPlot *plot, const QVector<double> *arr, const Ui::PlotSettings *settings)
{
    plot->addGraph();

    plot->graph(0)->setPen(QPen(Qt::black));
    plot->graph(0)->setData(m_t, *arr);

    plot->xAxis->setRange(0,100);
    plot->xAxis->setVisible(false);
    plot->yAxis->setLabel(settings->unit);
    plot->yAxis->setRange(settings->min, settings->max);
    plot->yAxis->ticker()->setTickCount(8);

    plot->replot();
}

quint8 MainWindow::get_sum(const quint8 *arr, quint8 len)
{
    quint8 sum = 0;
    for (size_t i = 0; i < len; ++i)
        sum += arr[i];

    return sum;
}

void MainWindow::send_data(quint8 typeSensor, const quint8 *data)
{
    if (!m_serialPort->isOpen())
    {
        QMessageBox::warning(this, "Warning", "The serial is unavailable.");
        return;
    }

    quint8 message[m_messageSettings.msgOutSize] = {m_messageSettings.prefix, typeSensor};

    memcpy((message + m_messageSettings.headerSize), data, m_messageSettings.dataSize);
    message[m_messageSettings.msgOutSize - 1] = get_sum(message, m_messageSettings.msgOutSize - 1);

    m_serialPort->write(reinterpret_cast<char *>(message), m_messageSettings.msgOutSize);
}
