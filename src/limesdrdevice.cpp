#include "limesdrdevice.h"

#include <QDebug>
#include <string>
#include <bitset>

LimeSDRDevice::LimeSDRDevice()
{
    //initialize member variables;
    m_rx_mtu = m_tx_mtu = 0;
    m_connected = m_rx_started = m_tx_started = false;

    //create ring buffers
    m_rx_buffer[0] = new RingBufferSPSC<std::complex<int16_t>>();
    m_tx_buffer[0] = new RingBufferSPSC<std::complex<int16_t>>();

    //initialize workers and make threads
    m_tx_worker_thread = new QThread();
    m_rx_worker_thread = new QThread();
    m_rx_stream_worker = NULL;
    m_tx_stream_worker = NULL;
    m_tx_antenna = "BAND1";

    //initialize device pointer to null
    m_sdr_dev = NULL;

    /*
    m_sdr_dev->setSampleRate(SOAPY_SDR_RX, 0, 10.0e6);
    m_sdr_dev->setFrequency(SOAPY_SDR_RX, 0, 106.0e6);
    m_sdr_dev->setAntenna(SOAPY_SDR_RX, 0, "LNAL");
    m_sdr_dev->setGain(SOAPY_SDR_RX, 0, "LNA", 15.0);
    m_sdr_dev->setGain(SOAPY_SDR_RX, 0, "TIA", 3.0);
    m_sdr_dev->setGain(SOAPY_SDR_RX, 0, "PGA", 3.0);
    m_sdr_dev->setBandwidth(SOAPY_SDR_RX, 0, 5000000.0);

    m_sdr_dev->setSampleRate(SOAPY_SDR_TX, 0, 10.0e6);
    m_sdr_dev->setFrequency(SOAPY_SDR_TX, 0, 106.0e6);
    m_sdr_dev->setAntenna(SOAPY_SDR_TX, 0, "NONE");
    m_sdr_dev->setGain(SOAPY_SDR_TX, 0, "PAD", -50);
    m_sdr_dev->setBandwidth(SOAPY_SDR_TX, 0, 30000000.0);

    //setup and streams in order to get MTU size
    std::cout << "activate stream rx" << std::endl;
    m_rx_stream = m_sdr_dev->setupStream(SOAPY_SDR_RX, SOAPY_SDR_CS16, {0});
    m_tx_stream = m_sdr_dev->setupStream(SOAPY_SDR_TX, SOAPY_SDR_CS16, {0});

    m_rx_mtu = m_sdr_dev->getStreamMTU(m_rx_stream);
    m_tx_mtu = m_sdr_dev->getStreamMTU(m_tx_stream);

    m_sdr_dev->activateStream(m_rx_stream);
    m_sdr_dev->activateStream(m_tx_stream);

    std::cout << "RX Stream MTU: " << m_rx_mtu << ", TX Stream MTU: " << m_tx_mtu << std::endl;

    //create stream worker
    m_rx_stream_worker = new LimeRxStreamWorker(m_sdr_dev, m_rx_stream, 1360, m_rx_buffer[0]);
    m_tx_stream_worker = new LimeTxStreamWorker(m_sdr_dev, m_tx_stream, 1360, m_tx_buffer[0]);
    */
}

LimeSDRDevice::~LimeSDRDevice()
{
    std::cout << "LimeSDRDevice destructor" << std::endl;
    delete m_rx_buffer[0];
    delete m_tx_buffer[0];


    m_rx_worker_thread->wait();
    delete m_rx_worker_thread;
    m_tx_worker_thread->wait();
    delete m_tx_worker_thread;


    delete m_rx_stream_worker;
    delete m_tx_stream_worker;

    //try to close all streams (if device was instantiated in first place)
    if(m_sdr_dev != NULL)
    {
        try
        {
            m_sdr_dev->deactivateStream(m_rx_stream);
            m_sdr_dev->deactivateStream(m_tx_stream);
            m_sdr_dev->closeStream(m_rx_stream);
            m_sdr_dev->closeStream(m_tx_stream);
        }
        catch (const std::exception &ex)
        {
            std::cerr << "Error closing streams: " << ex.what() << std::endl;
        }

        //try to unmake soapysdr device
        try
        {
            SoapySDR::Device::unmake(m_sdr_dev);
        }
        catch (const std::exception &ex)
        {
            std::cerr << "Error unmaking device: " << ex.what() << std::endl;
        }
    }
}

void LimeSDRDevice::connect(QString p_argStr)
{
    if(m_connected) return; //only connect once/if no device ist currently connected

    SoapySDR::Kwargs devicesKwargs = SoapySDR::KwargsFromString(p_argStr.toStdString());
    devicesKwargs["cacheCalibrations"]="0"; //override cacheCalibrationsetting
    std::cout << "Make device: " << SoapySDR::KwargsToString(devicesKwargs) << std::endl; // p_argStr.toStdString()
    try
    {
        //m_sdr_dev = SoapySDR::Device::make(p_argStr.toStdString());
        m_sdr_dev = SoapySDR::Device::make(devicesKwargs);
        std::cout << "  driver=" << m_sdr_dev->getDriverKey() << std::endl;
        std::cout << "  hardware=" << m_sdr_dev->getHardwareKey() << std::endl;
        for (const auto &it : m_sdr_dev->getHardwareInfo())
        {
            std::cout << "  " << it.first << "=" << it.second << std::endl;
        }

    }
    catch (const std::exception &ex)
    {
        std::cerr << "Error making device: " << ex.what() << std::endl;
    }


    m_sdr_dev->setSampleRate(SOAPY_SDR_RX, 0, 5.0e6);
    m_sdr_dev->setFrequency(SOAPY_SDR_RX, 0, 106.0e6);
    m_sdr_dev->setAntenna(SOAPY_SDR_RX, 0, "LNAL");
    m_sdr_dev->setGain(SOAPY_SDR_RX, 0, "LNA", 15.0);
    m_sdr_dev->setGain(SOAPY_SDR_RX, 0, "TIA", 3.0);
    m_sdr_dev->setGain(SOAPY_SDR_RX, 0, "PGA", -4.0);
    m_sdr_dev->setBandwidth(SOAPY_SDR_RX, 0, 5000000.0);

    m_sdr_dev->setSampleRate(SOAPY_SDR_TX, 0, 5.0e6);
    m_sdr_dev->setFrequency(SOAPY_SDR_TX, 0, 106.0e6);
    m_sdr_dev->setAntenna(SOAPY_SDR_TX, 0, "NONE");
    m_sdr_dev->setGain(SOAPY_SDR_TX, 0, "PAD", -50);
    m_sdr_dev->setBandwidth(SOAPY_SDR_TX, 0, 30000000.0);



    //setup and streams in order to get MTU size
    m_rx_stream = m_sdr_dev->setupStream(SOAPY_SDR_RX, SOAPY_SDR_CS16, {0});
    m_tx_stream = m_sdr_dev->setupStream(SOAPY_SDR_TX, SOAPY_SDR_CS16, {0});

    m_rx_mtu = m_sdr_dev->getStreamMTU(m_rx_stream); //1360
    m_tx_mtu = m_sdr_dev->getStreamMTU(m_tx_stream); //1360

    m_sdr_dev->activateStream(m_rx_stream);
    m_sdr_dev->activateStream(m_tx_stream);

    std::cout << "RX Stream MTU: " << m_rx_mtu << ", TX Stream MTU: " << m_tx_mtu << std::endl;

    //create stream worker
    m_rx_stream_worker = new LimeRxStreamWorker(m_sdr_dev, m_rx_stream, m_rx_mtu, m_rx_buffer[0]);
    m_tx_stream_worker = new LimeTxStreamWorker(m_sdr_dev, m_tx_stream, m_tx_mtu, m_tx_buffer[0]);

    m_connected = true;

/*
    const int reg_addr_fpga = 26;
    const int led_val = (3<<3); //set FPGA_LED2_CTRL to 101
    //m_sdr_dev->writeRegister(reg_addr_fpga, led_val);
    int tmp = m_sdr_dev->readRegister(reg_addr_fpga); // read FPGA registers
    std::bitset<16> tmp_bin_fpga(tmp);
    std::cout << "reg addr: " << std::hex << reg_addr_fpga << ": " << std::dec << tmp << " - " << tmp_bin_fpga << std::endl;

    const int reg_addr = 0x002F;
    tmp = m_sdr_dev->readRegister("RFIC0", reg_addr); //read LMS7002M registers
    std::bitset<16> tmp_bin(tmp);
    std::cout << "reg addr: " << std::hex << reg_addr << ": " << std::dec << tmp << " - " << tmp_bin << std::endl;
*/
}

void LimeSDRDevice::disconnect (void)
{
    m_connected = false;
}

void LimeSDRDevice::startRxStream (void)
{
    //move to thread and connect signals for life time management
    m_rx_stream_worker->moveToThread(m_rx_worker_thread);
    QObject::connect(m_rx_worker_thread, SIGNAL(started()), m_rx_stream_worker, SLOT(run()));
    QObject::connect(m_rx_stream_worker, SIGNAL(finished()), m_rx_worker_thread, SLOT(quit()));
    //QObject::connect(m_rx_stream_worker, SIGNAL(finished()), m_rx_stream_worker, SLOT(deleteLater())); //automatically delete worker object
    //QObject::connect(m_rx_worker_thread, SIGNAL(finished()), m_rx_worker_thread, SLOT(deleteLater())); //automatically delete thread
    m_rx_worker_thread->start();
}

void LimeSDRDevice::pauseRxStream (void)
{
    m_rx_stream_worker->pause();
}

void LimeSDRDevice::resumeRxStream (void)
{
    m_rx_stream_worker->resume();
}

void LimeSDRDevice::stopRxStream (void)
{
    //TODO: join the worker thread to be sure that the thread has ended (should be able to restart again)
    m_rx_stream_worker->stop();
    /*
    try
    {
        m_sdr_dev->deactivateStream(m_rx_stream);
    }
    catch (const std::exception &ex)
    {
        std::cerr << "Error deactivation stream: " << ex.what() << std::endl;
    }
    */
}

void LimeSDRDevice::startTxStream (void)
{
    //set antenna from "NONE" to whatever is set in current config
    m_sdr_dev->setAntenna(SOAPY_SDR_TX, 0, m_tx_antenna);

    //move to thread and connect signals for life time management
    m_tx_stream_worker->moveToThread(m_tx_worker_thread);
    QObject::connect(m_tx_worker_thread, SIGNAL(started()), m_tx_stream_worker, SLOT(run()));
    QObject::connect(m_tx_stream_worker, SIGNAL(finished()), m_tx_worker_thread, SLOT(quit()));
    //QObject::connect(m_tx_stream_worker, SIGNAL(finished()), m_tx_stream_worker, SLOT(deleteLater())); //automatically delete worker object
    //QObject::connect(m_tx_worker_thread, SIGNAL(finished()), m_tx_worker_thread, SLOT(deleteLater())); //automatically delete thread
    m_tx_worker_thread->start();
}

void LimeSDRDevice::pauseTxStream (void)
{
    m_tx_stream_worker->pause();
}

void LimeSDRDevice::resumeTxStream (void)
{
    m_tx_stream_worker->resume();
}

void LimeSDRDevice::stopTxStream (void)
{
    //TODO: join the worker thread to be sure that the thread has ended (should be able to restart again)
    m_sdr_dev->setAntenna(SOAPY_SDR_TX, 0, "NONE");
    m_tx_stream_worker->stop();
    /*
    try
    {
        m_sdr_dev->deactivateStream(m_tx_stream);
    }
    catch (const std::exception &ex)
    {
        std::cerr << "Error deactivation stream: " << ex.what() << std::endl;
    }
    */
}


int LimeSDRDevice::readReg(std::string p_dev, int p_addr)
{
    if(!m_connected) return -1;
    return m_sdr_dev->readRegister(p_dev, p_addr);
}


void LimeSDRDevice::writeReg(std::string p_dev, int p_addr, int p_value)
{
    if(!m_connected) return;
    m_sdr_dev->writeRegister(p_dev, p_addr, p_value);
}



//setter for rx part
void LimeSDRDevice::setRxLoFreq(double p_freq, int p_chan)
{
    if(!m_connected) return;
    try
    {
        m_sdr_dev->setFrequency(SOAPY_SDR_RX, p_chan, p_freq);
    } catch (const std::exception& e)
    {
        qDebug()<<e.what();
    }
}

void LimeSDRDevice::setRxFIR_BW(double p_bw, int p_chan)
{
    if(!m_connected) return;
    try
    {
        m_sdr_dev->setBandwidth(SOAPY_SDR_RX, p_chan, p_bw);
    } catch (const std::exception& e)
    {
        qDebug()<<e.what();
    }
}

void LimeSDRDevice::setRxSampRate(double p_rate, int p_chan)
{
    if(!m_connected) return;
    try
    {
        m_sdr_dev->setSampleRate(SOAPY_SDR_RX, p_chan, p_rate);
    } catch (const std::exception& e)
    {
        qDebug()<<e.what();
    }
}

void LimeSDRDevice::setLNA(int p_gain, int p_chan)
{
    if(!m_connected) return;
    try
    {
        m_sdr_dev->setGain(SOAPY_SDR_RX, p_chan, "LNA", p_gain);
    } catch (const std::exception& e)
    {
        qDebug()<<e.what();
    }
}

void LimeSDRDevice::setTIA(int p_gain, int p_chan)
{
    if(!m_connected) return;
    try
    {
        m_sdr_dev->setGain(SOAPY_SDR_RX, p_chan, "TIA", p_gain);
    } catch (const std::exception& e)
    {
        qDebug()<<e.what();
    }
}

void LimeSDRDevice::setPGA(int p_gain, int p_chan)
{
    if(!m_connected) return;
    try
    {
        m_sdr_dev->setGain(SOAPY_SDR_RX, p_chan, "PGA", p_gain);
    } catch (const std::exception& e)
    {
        qDebug()<<e.what();
    }
}

void LimeSDRDevice::setRxAntenna(std::string p_ant, int p_chan)
{
    if(!m_connected) return;
    try
    {
        m_sdr_dev->setAntenna(SOAPY_SDR_RX, p_chan, p_ant);
    } catch (const std::exception& e)
    {
        qDebug()<<e.what();
    }
}

//getter for rx part
RingBufferSPSC<std::complex<int16_t>>* LimeSDRDevice::getRxBuffer (int p_chan)
{
    return m_rx_buffer[p_chan];
}


//setter for tx part
void LimeSDRDevice::setTxLoFreq(double p_freq, int p_chan)
{
    if(!m_connected) return;
    try
    {
        m_sdr_dev->setFrequency(SOAPY_SDR_TX, p_chan, p_freq);
    } catch (const std::exception& e)
    {
        qDebug()<<e.what();
    }
}

void LimeSDRDevice::setTxFIR_BW(double p_bw, int p_chan)
{
    if(!m_connected) return;
    try
    {
        m_sdr_dev->setBandwidth(SOAPY_SDR_TX, p_chan, p_bw);
    } catch (const std::exception& e)
    {
        qDebug()<<e.what();
    }
}

void LimeSDRDevice::setTxSampRate(double p_rate, int p_chan)
{
    if(!m_connected) return;
    try
    {
        m_sdr_dev->setSampleRate(SOAPY_SDR_TX, p_chan, p_rate);
    } catch (const std::exception& e)
    {
        qDebug()<<e.what();
    }
}

void LimeSDRDevice::setPAD(int p_gain, int p_chan)
{
    if(!m_connected) return;
    try
    {
        m_sdr_dev->setGain(SOAPY_SDR_TX, p_chan, "PAD", p_gain);
    } catch (const std::exception& e)
    {
        qDebug()<<e.what();
    }
}

void LimeSDRDevice::setTxAntenna(std::string p_ant, int p_chan)
{
    if(m_connected)
    {
        try
        {
            m_sdr_dev->setAntenna(SOAPY_SDR_TX, p_chan, p_ant);
        } catch (const std::exception& e)
        {
            qDebug()<<e.what();
        }
    }
    else
    {
        m_tx_antenna = p_ant;
    }
}

//getter for tx part
RingBufferSPSC<std::complex<int16_t>>* LimeSDRDevice::getTxBuffer(int p_chan)
{
    return m_tx_buffer[p_chan];
}
