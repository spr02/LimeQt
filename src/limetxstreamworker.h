#ifndef LIMETXSTREAMWORKER_H
#define LIMETXSTREAMWORKER_H

#include <QObject>
#include <QMutex>
#include <QWaitCondition>
#include <SoapySDR/Version.hpp>
#include <SoapySDR/Modules.hpp>
#include <SoapySDR/Registry.hpp>
#include <SoapySDR/Device.hpp>
#include <SoapySDR/Formats.hpp>

#include "ring_buf_spsc.h"


class LimeTxStreamWorker : public QObject
{
    Q_OBJECT
public:
    explicit LimeTxStreamWorker(SoapySDR::Device* p_device, SoapySDR::Stream* p_stream, int p_buffer_size, RingBufferSPSC<std::complex<int16_t>>* p_ring_buffer, QObject *p_parent = nullptr);
    ~LimeTxStreamWorker();

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
    int m_time;
    const int m_BUFF_SIZE;

    //state control
    QMutex m_pause_mutex;
    QWaitCondition m_pause_cond;
    volatile bool m_pause;
    volatile bool m_work;

    //actual sdr device and stream
    SoapySDR::Device* m_device;
    SoapySDR::Stream* m_tx_stream;

    //pointer required for the writeStream function and a pointer to the actual ring buffer
    void *m_txb[2]; //required for the writeStream function
    std::complex<int16_t> *m_tx_buffer; //makes handling of the current buffer easier
    RingBufferSPSC<std::complex<int16_t>>* m_ring_buffer;
};

#endif // LIMETXSTREAMWORKER_H
