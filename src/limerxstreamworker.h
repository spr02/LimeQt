#ifndef LIMERXSTREAMWORKER_H
#define LIMERXSTREAMWORKER_H

#include <QObject>
#include <QMutex>
#include <QWaitCondition>
#include <complex>
#include <SoapySDR/Version.hpp>
#include <SoapySDR/Modules.hpp>
#include <SoapySDR/Registry.hpp>
#include <SoapySDR/Device.hpp>
#include <SoapySDR/Formats.hpp>

#include "ring_buf_spsc.h"

class LimeRxStreamWorker : public QObject
{
    Q_OBJECT
public:
    explicit LimeRxStreamWorker(SoapySDR::Device* p_device, SoapySDR::Stream* p_stream, int p_buffer_size, RingBufferSPSC<std::complex<int16_t>>* p_ring_buffer, QObject *p_parent = nullptr);
    ~LimeRxStreamWorker();

    void stop (void);
    void pause (void);
    void resume (void);

signals:
    void finished();
    void error(QString err);

public slots:
    void run (void);

private:

    int m_sample_count;
    int m_ret;
    int m_flags;
    long long m_time;
    const int m_BUFF_SIZE;

    //state control
    QMutex m_pause_mutex;
    QWaitCondition m_pause_cond;
    volatile bool m_pause;
    volatile bool m_work;

    //actual sdr device and stream
    SoapySDR::Device* m_device;
    SoapySDR::Stream* m_rx_stream;

    //pointer required for the readStream function and a pointer to the actual ring buffer
    void *m_rxb[2]; //required for the readStream function
    std::complex<int16_t> *m_rx_buffer; //makes handling of the current buffer easier
    RingBufferSPSC<std::complex<int16_t>>* m_ring_buffer;
};

#endif // LIMERXSTREAMWORKER_H
