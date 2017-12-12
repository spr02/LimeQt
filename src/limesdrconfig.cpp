#include "limesdrconfig.h"
#include "ui_limesdrconfig.h"
#include <iostream>
#include <sstream>
#include <bitset>

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


    //register access
    ui->reg_addr_line->setText("0");
    m_reg_addr_format = dec;
    m_reg_addr = 0;
    ui->reg_value_line->setText("0000000000000000");
    m_reg_value_format = bin;
    m_reg_value = 0;

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





//////////////////// SLOTS for buttons and value changed for register access ////////////////////

/////// Helpers ///////

QString LimeSDRConfig::dec2bin (const QString &p_str, int *p_int)
{
    *p_int = std::stoi(p_str.toStdString().c_str(), nullptr, 10);
    std::bitset<16> tmp(*p_int);
    return QString(tmp.to_string().c_str());
}
QString LimeSDRConfig::dec2hex (const QString &p_str, int *p_int)
{
    *p_int = std::stoi(p_str.toStdString().c_str(), nullptr, 10);
    std::stringstream tmp;
    tmp << std::hex << *p_int;
    return QString(tmp.str().c_str());
}

QString LimeSDRConfig::hex2dec (const QString &p_str, int *p_int)
{
    *p_int = std::stoi(p_str.toStdString().c_str(), nullptr, 16);
    return QString(std::to_string(*p_int).c_str());
}
QString LimeSDRConfig::hex2bin (const QString &p_str, int *p_int)
{
    *p_int = std::stoi(p_str.toStdString().c_str(), nullptr, 16);
    std::bitset<16> tmp(*p_int);
    return QString(tmp.to_string().c_str());
}

QString LimeSDRConfig::bin2dec (const QString &p_str, int *p_int)
{
    *p_int = std::stoi(p_str.toStdString().c_str(), nullptr, 2);
    return QString(std::to_string(*p_int).c_str());
}

QString LimeSDRConfig::bin2hex (const QString &p_str, int *p_int)
{
    *p_int = std::stoi(p_str.toStdString().c_str(), nullptr, 2);
    std::stringstream tmp;
    tmp << std::hex << *p_int;
    return QString(tmp.str().c_str());
}


/////// Buttons //////
void LimeSDRConfig::on_reg_read_clicked()
{
    //read register value
    int reg_addr = m_reg_addr;
    std::string reg_dev = ui->reg_dev_combo->currentText().toStdString();
    int reg_val = m_sdr_dev->readReg(reg_dev, reg_addr);

    //set value line edit
    QString tmp(std::to_string(reg_val).c_str());
    if(m_reg_value_format == dec)
    {
        ui->reg_value_line->setText(tmp);
    }
    else if(m_reg_value_format == hex)
    {
        ui->reg_value_line->setText(dec2hex(tmp, &reg_val));
    }
    else
    {
        ui->reg_value_line->setText(dec2bin(tmp, &reg_val));
    }
    std::bitset<16> tmp_bit(reg_val);
    std::cout << "Reading from: " << reg_dev << " at addr: " << reg_addr << " the value: " << tmp_bit.to_string() << " - " << reg_val << std::endl;

    //save register value in member variable
    m_reg_value = reg_val;
}

void LimeSDRConfig::on_reg_write_clicked()
{
    int reg_addr = m_reg_addr;
    int reg_val = m_reg_value & 0xFFFF;
    std::string reg_dev = ui->reg_dev_combo->currentText().toStdString();
    std::bitset<16> tmp(reg_val);

    std::cout << "Writing to: " << reg_dev << " at addr: " << reg_addr << " the value: " << tmp.to_string() << " - " << reg_val << std::endl;
    m_sdr_dev->writeReg(reg_dev, reg_addr, reg_val);
}

/////// Address Radio Buttons ///////

void LimeSDRConfig::on_reg_addr_hex_clicked()
{
    ui->reg_addr_hex->setChecked(true);
    ui->reg_addr_dec->setChecked(false);
    if(m_reg_addr_format == dec)
    {
        ui->reg_addr_line->setText(dec2hex(ui->reg_addr_line->text(), &m_reg_addr));
    }
    m_reg_addr_format = hex;
}

void LimeSDRConfig::on_reg_addr_dec_clicked()
{
    ui->reg_addr_hex->setChecked(false);
    ui->reg_addr_dec->setChecked(true);
    if(m_reg_addr_format == hex)
    {
        //m_reg_addr = std::stoi(ui->reg_value_line->text().toStdString(), nullptr, 16);
        ui->reg_addr_line->setText(hex2dec(ui->reg_addr_line->text(), &m_reg_addr));
    }
    m_reg_addr_format = dec;
}

/////// Value Radio Buttons ///////

void LimeSDRConfig::on_reg_value_hex_clicked()
{
    ui->reg_value_hex->setChecked(true);
    ui->reg_value_bin->setChecked(false);
    ui->reg_value_dec->setChecked(false);
    if(m_reg_value_format == dec)
    {
        ui->reg_value_line->setText(dec2hex(ui->reg_value_line->text(), &m_reg_value));
    }
    else if (m_reg_value_format == bin)
    {
        ui->reg_value_line->setText(bin2hex(ui->reg_value_line->text(), &m_reg_value));
    }
    m_reg_value_format = hex;
}

void LimeSDRConfig::on_reg_value_bin_clicked()
{
    ui->reg_value_hex->setChecked(false);
    ui->reg_value_bin->setChecked(true);
    ui->reg_value_dec->setChecked(false);
    if(m_reg_value_format == dec)
    {
        ui->reg_value_line->setText(dec2bin(ui->reg_value_line->text(), &m_reg_value));
    }
    else if (m_reg_value_format == hex)
    {
        ui->reg_value_line->setText(hex2bin(ui->reg_value_line->text(), &m_reg_value));
    }
    m_reg_value_format = bin;
}

void LimeSDRConfig::on_reg_value_dec_clicked()
{
    ui->reg_value_hex->setChecked(false);
    ui->reg_value_bin->setChecked(false);
    ui->reg_value_dec->setChecked(true);
    if(m_reg_value_format == bin)
    {
        ui->reg_value_line->setText(bin2dec(ui->reg_value_line->text(), &m_reg_value));
    }
    else if (m_reg_value_format == hex)
    {
        ui->reg_value_line->setText(hex2dec(ui->reg_value_line->text(), &m_reg_value));
    }
    m_reg_value_format = dec;
}

/////// Line Edit ///////

void LimeSDRConfig::on_reg_addr_line_textEdited(const QString &arg1)
{
    if(m_reg_addr_format == dec)
    {
        dec2bin(arg1, &m_reg_addr);
    }
    else
    {
        hex2bin(arg1, &m_reg_addr);
    }
}

void LimeSDRConfig::on_reg_value_line_textEdited(const QString &arg1)
{
    if(m_reg_value_format == dec)
    {
        dec2bin(arg1, &m_reg_value);
    }
    else if(m_reg_value_format == hex)
    {
        hex2bin(arg1, &m_reg_value);
    }
    else
    {
        bin2dec(arg1, &m_reg_value);
    }
}
