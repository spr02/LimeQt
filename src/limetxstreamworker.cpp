#include "limetxstreamworker.h"

LimeTxStreamWorker::LimeTxStreamWorker(SoapySDR::Device* p_device, SoapySDR::Stream* p_stream, int p_buffer_size, RingBufferSPSC<std::complex<int16_t>>* p_ring_buffer, QObject *p_parent) :
    QObject(p_parent), m_BUFF_SIZE(p_buffer_size)
{
    m_sample_count = 0;
    m_ret = 0;
    m_flags = 0;
    m_time = 0;

    //set device, stream and buffer
    m_device = p_device;
    m_tx_stream = p_stream;
    m_ring_buffer = p_ring_buffer;

    //create new buffer and set first entry of m_rxb to the buffer
    m_tx_buffer = new std::complex<int16_t>[m_BUFF_SIZE];
    m_pause = false;

    //start unpause
    m_txb[0] = m_tx_buffer;
}

LimeTxStreamWorker::~LimeTxStreamWorker()
{
    delete m_tx_buffer;
}

void LimeTxStreamWorker::stop (void)
{
    m_work = false;
}

void LimeTxStreamWorker::pause (void)
{
    m_pause_mutex.lock();
    m_pause = true;
    m_pause_mutex.unlock();
}

void LimeTxStreamWorker::resume (void)
{
    m_pause_mutex.lock();
    m_pause = false;
    m_pause_mutex.unlock();
    m_pause_cond.wakeAll();
}

void LimeTxStreamWorker::run(void)
{
    m_work = true;
    while(m_work)
    {
        //check pause condition
        m_pause_mutex.lock();
        if(m_pause) m_pause_cond.wait(&m_pause_mutex);
        m_pause_mutex.unlock();

        //fill buffer with values from queue
        for(int k=0;k<m_BUFF_SIZE;k++)
        {
            m_tx_buffer[k] = m_ring_buffer->pop();
        }

        //write buffer to sdr
        m_ret = m_device->writeStream(m_tx_stream, m_txb, m_BUFF_SIZE, m_flags);
        if(m_ret != m_BUFF_SIZE)
        {
            std::cerr << "Failed to write all samples. (" << m_ret << " of " << m_BUFF_SIZE << ")" << std::endl;
        }
    }
    emit finished();
}
