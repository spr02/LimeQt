#ifndef LIMESDRDEVICE_H
#define LIMESDRDEVICE_H

#include <complex>
#include <QString>
#include <QThread>
#include <SoapySDR/Device.hpp>
#include <SoapySDR/Formats.hpp>
#include <SoapySDR/Types.hpp>
#include <SoapySDR/Formats.hpp>
#include <SoapySDR/Time.hpp>

#include "ring_buf_spsc.h"
#include "limerxstreamworker.h"
#include "limetxstreamworker.h"

class LimeSDRDevice
{
public:

    /////////////////////// FACTORY ////////////////////////
    LimeSDRDevice();
    ~LimeSDRDevice();

    void connect (QString p_argStr);
    void disconnect (void);

    /////////////////////// STREAMING //////////////////////

    void startRxStream (void);
    void pauseRxStream (void);
    void resumeRxStream (void);
    void stopRxStream (void);

    void startTxStream (void);
    void pauseTxStream (void);
    void resumeTxStream (void);
    void stopTxStream (void);

    RingBufferSPSC<std::complex<int16_t>>* getRxBuffer (int p_chan = 0); //get the queue which can be consumed
    RingBufferSPSC<std::complex<int16_t>>* getTxBuffer (int p_chan = 0); //get the queue which needs to be produced

    /////////////////////// CALIBRATION ////////////////////


    /////////////////////// Register ///////////////////////
    int readReg(std::string p_dev, int p_addr);
    void writeReg(std::string p_dev, int p_addr, int p_value);

    /////////////////////// RX PART ////////////////////////
    void setRxFIR_BW(double p_bw, int p_chan = 0);
    void setRxSampRate(double p_rate, int p_chan = 0);
    void setRxLoFreq(double p_freq, int p_chan = 0);

    void setLNA(int p_gain, int p_chan = 0);
    void setTIA(int p_gain, int p_chan = 0);
    void setPGA(int p_gain, int p_chan = 0);
    void setRxAntenna(std::string p_ant, int p_chan = 0);


    /////////////////////// TX PART ////////////////////////
    void setTxFIR_BW(double p_bw, int p_chan = 0);
    void setTxSampRate(double p_rate, int p_chan = 0);
    void setTxLoFreq(double p_freq, int p_chan = 0);

    void setPAD(int p_gain, int p_chan = 0);
    void setTxAntenna(std::string p_ant, int p_chan = 0);


private:
    //some parameters
    int m_rx_mtu, m_tx_mtu;
    bool m_connected, m_rx_started, m_tx_started;
    std::string m_tx_antenna;

    //sdr device and streams
    SoapySDR::Device *m_sdr_dev;
    SoapySDR::Stream *m_rx_stream;
    SoapySDR::Stream *m_tx_stream;

    //rx and tx worker + threads
    QThread* m_rx_worker_thread;
    QThread* m_tx_worker_thread;
    LimeRxStreamWorker* m_rx_stream_worker;
    LimeTxStreamWorker* m_tx_stream_worker;

    RingBufferSPSC<std::complex<int16_t>> *m_rx_buffer[2];
    RingBufferSPSC<std::complex<int16_t>> *m_tx_buffer[2];

    std::complex<int16_t> *m_txb[2];
    std::complex<int16_t> *m_rxb[2];

};

#endif // LIMESDRDEVICE_H
