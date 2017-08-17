#include "limesdrconfig.h"
#include "ui_limesdrconfig.h"
#include <iostream>

LimeSDRConfig::LimeSDRConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LimeSDRConfig)
{
    ui->setupUi(this);

    //initialize member variables
    m_connected = false;
    m_started = false;
    m_sdr_dev = NULL;
    m_rx_head = m_rx_tail = m_tx_head = m_tx_tail = 0;

    //refresh device list (combo box)
    this->on_refresh_but_clicked();

    //make new timer and connect the update slot
    m_update_timer = new QTimer;
    QObject::connect(m_update_timer, SIGNAL(timeout()),SLOT(update_rate()));
    m_update_timer->start(1000);

    //connect sliders and spin boxes, so values change if one of them is changed
    QObject::connect(ui->lna_slider,SIGNAL(valueChanged(int)), ui->lna_spin, SLOT(setValue(int))); //lna
    QObject::connect(ui->lna_spin,SIGNAL(valueChanged(int)), ui->lna_slider, SLOT(setValue(int)));
    QObject::connect(ui->tia_slider,SIGNAL(valueChanged(int)), ui->tia_spin, SLOT(setValue(int))); //tia
    QObject::connect(ui->tia_spin,SIGNAL(valueChanged(int)), ui->tia_slider, SLOT(setValue(int)));
    QObject::connect(ui->pga_slider,SIGNAL(valueChanged(int)), ui->pga_spin, SLOT(setValue(int))); //pga
    QObject::connect(ui->pga_spin,SIGNAL(valueChanged(int)), ui->pga_slider, SLOT(setValue(int)));
    QObject::connect(ui->pad_slider,SIGNAL(valueChanged(int)), ui->pad_spin, SLOT(setValue(int))); //pad
    QObject::connect(ui->pad_spin,SIGNAL(valueChanged(int)), ui->pad_slider, SLOT(setValue(int)));
}

LimeSDRConfig::~LimeSDRConfig()
{
    delete ui;
    delete m_update_timer;
}

//////////////////// setter for rate and buffer usage ////////////////////

void LimeSDRConfig::update_rate (void)
{
    //get current heads and tails
    uint64_t rx_head = m_sdr_dev->getRxBuffer()->get_head();
    uint64_t rx_tail = m_sdr_dev->getRxBuffer()->get_tail();
    uint64_t tx_head = m_sdr_dev->getTxBuffer()->get_head();
    uint64_t tx_tail = m_sdr_dev->getTxBuffer()->get_tail();

    //set labels
    ui->rx_rate_lbl->setText(QString::number((rx_head - m_rx_head) / 1.0e6, 'g', 4));
    ui->tx_rate_lbl->setText(QString::number((tx_head - m_tx_head) / 1.0e6, 'g', 4));
    ui->rx_buf_progress->setValue((rx_head - rx_tail) / 65536.0 * 100.0);
    ui->tx_buf_progress->setValue((tx_head - tx_tail) / 65536.0 * 100.0);

    //save current values for next call
    m_rx_head = rx_head;
    m_rx_tail = rx_tail;
    m_tx_head = tx_head;
    m_tx_tail = tx_tail;
}


//////////////////// setter and getter for device pointer ////////////////////
LimeSDRDevice* LimeSDRConfig::getDevPtr (void)
{
    return m_sdr_dev;
}

void LimeSDRConfig::setDevPtr(LimeSDRDevice* p_dev)
{
    m_sdr_dev = p_dev;
}


//////////////////// SLOTS for buttons and value changed ////////////////////

//connect and start refresh button
void LimeSDRConfig::on_refresh_but_clicked()
{
    int id=0;
    ui->dev_combo->clear();
    QString devicename;

    devicesKwargs = SoapySDR::Device::enumerate(SoapySDR::Kwargs());

    for(SoapySDR::Kwargs& dev : devicesKwargs)
    {
        for(auto const &e : dev)
        {
            devicename = devicename + e.first.c_str() + "=" + e.second.c_str() + ", ";
        }
        ui->dev_combo->addItem(devicename,QVariant(id++));
    }
}

void LimeSDRConfig::on_connect_but_clicked()
{
    //TODO: generate some arg string
    m_sdr_dev->connect("");
    ui->dev_param_lbl->setText(QString("Connected to LimeSDR"));
}

//set buttons + rate test
void LimeSDRConfig::on_set_lo_freq_but_clicked()
{
    m_sdr_dev->setRxLoFreq(ui->rx_lo_freq_spin->value());
    m_sdr_dev->setTxLoFreq(ui->tx_lo_freq_spin->value());
}

void LimeSDRConfig::on_set_fir_bw_but_clicked()
{
    std::cout << "Changing of FIR bandwidth currently not implemented!" << std::endl;
}

void LimeSDRConfig::on_set_rate_clicked()
{
    std::cout << "Changing of sample rate currently not implemented!" << std::endl;
}

void LimeSDRConfig::on_test_rate_clicked()
{
    std::cout << "Rate test currently not implemented!" << std::endl;
}

//lower right buttons (six)
void LimeSDRConfig::on_start_button_clicked()
{
    if(!m_started)
    {
        m_started = true;
        m_sdr_dev->startRxStream();
        m_sdr_dev->startTxStream();
        ui->start_button->setText("Stop");
    }
    else
    {
        m_started = false;
        emit stop_consumer();
        m_sdr_dev->stopRxStream();
        m_sdr_dev->stopTxStream();
        emit stop_producer();
        ui->start_button->setText("Start");
    }

}

void LimeSDRConfig::on_save_button_clicked()
{
    std::cout << "Save currently not implemented!" << std::endl;
    m_sdr_dev->pauseRxStream();
    m_sdr_dev->pauseTxStream();
}

void LimeSDRConfig::on_load_button_clicked()
{
    std::cout << "Load currently not implemented!" << std::endl;
    m_sdr_dev->resumeRxStream();
    m_sdr_dev->resumeTxStream();
}

void LimeSDRConfig::on_calib_button_clicked()
{
    std::cout << "Calibration currently not implemented!" << std::endl;
}

void LimeSDRConfig::on_calib_tx_button_clicked()
{
    std::cout << "Calibration currently not implemented!" << std::endl;
}

void LimeSDRConfig::on_calib_rx_button_clicked()
{
    std::cout << "Calibration currently not implemented!" << std::endl;
}

//value changed slots
void LimeSDRConfig::on_lna_slider_valueChanged(int value)
{
    m_sdr_dev->setLNA(value);
}

void LimeSDRConfig::on_tia_slider_valueChanged(int value)
{
    m_sdr_dev->setTIA(value);
}

void LimeSDRConfig::on_pga_slider_valueChanged(int value)
{
    m_sdr_dev->setPGA(value);
}

void LimeSDRConfig::on_pad_slider_valueChanged(int value)
{
    m_sdr_dev->setPAD(value);
}

void LimeSDRConfig::on_rx_antenna_combo_currentTextChanged(const QString &arg1)
{
    m_sdr_dev->setRxAntenna(arg1.toStdString());
}

void LimeSDRConfig::on_tx_antenna_combo_currentTextChanged(const QString &arg1)
{
    m_sdr_dev->setTxAntenna(arg1.toStdString());
}






