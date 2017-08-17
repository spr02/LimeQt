#include "limerxstreamworker.h"

LimeRxStreamWorker::LimeRxStreamWorker(SoapySDR::Device* p_device, SoapySDR::Stream* p_stream, int p_buffer_size, RingBufferSPSC<std::complex<int16_t>>* p_ring_buffer, QObject *p_parent) :
    QObject(p_parent), m_BUFF_SIZE(p_buffer_size)
{
    m_sample_count = 0;
    m_ret = 0;
    m_flags = 0;
    m_time = 0;

    //set device, stream and buffer
    m_device = p_device;
    m_rx_stream = p_stream;
    m_ring_buffer = p_ring_buffer;

    //create new buffer and set first entry of m_rxb to the buffer
    m_rx_buffer = new std::complex<int16_t>[m_BUFF_SIZE];
    m_rxb[0] = m_rx_buffer;

    //start unpause
    m_pause = false;
}

LimeRxStreamWorker::~LimeRxStreamWorker()
{
    delete m_rx_buffer;
}

void LimeRxStreamWorker::stop (void)
{
    m_work = false;
}

void LimeRxStreamWorker::pause (void)
{
    m_pause_mutex.lock();
    m_pause = true;
    m_pause_mutex.unlock();
}

void LimeRxStreamWorker::resume (void)
{
    m_pause_mutex.lock();
    m_pause = false;
    m_pause_mutex.unlock();
    m_pause_cond.wakeAll();
}

void LimeRxStreamWorker::run(void)
{
    m_work = true;
    while(m_work)
    {
        //check pause condition
        m_pause_mutex.lock();
        if(m_pause) m_pause_cond.wait(&m_pause_mutex);
        m_pause_mutex.unlock();

        //read buffer from sdr
        m_ret = m_device->readStream(m_rx_stream, m_rxb, m_BUFF_SIZE, m_flags, m_time, 1000000);
        if(m_ret != m_BUFF_SIZE)
        {
            std::cout << "Failed to read all samples. (" << m_BUFF_SIZE << ")" << std::endl;
        }

        //push buffer into queue
        for(int k=0;k<m_ret;k++){
            //m_ring_buffer->push(m_rx_buffer[k]);
            while(!m_ring_buffer->try_push(m_rx_buffer[k]) && m_work);
        }
    }
    emit finished();
}
