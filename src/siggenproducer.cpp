#include "siggenproducer.h"
#include "time.h"
#include "stdlib.h"
#include <random>

#define SINE_FREQ   2000000.0 // sine wave frequency in Hz

SigGenProducer::SigGenProducer(QObject *parent) : QObject(parent)
{

    m_j = std::complex<double>(0, 1);
    m_phase_acc = 0.0;
    m_phase_inc = 2.0 * M_PI * SINE_FREQ / 10e6;
    m_pause = false;

    for(int i=0;i<32;i++)
    {
        conv_array_real[i] = 0;
        conv_array_imag[i] = 0;
    }
}

void SigGenProducer::setBuffer(RingBufferSPSC<std::complex<int16_t>>* p_buf)
{
    m_produce_buffer = p_buf;
}

void SigGenProducer::stop(void)
{
    m_work = false;
}

void SigGenProducer::pause (void)
{
    m_pause_mutex.lock();
    m_pause = true;
    m_pause_mutex.unlock();
}

void SigGenProducer::resume (void)
{
    m_pause_mutex.lock();
    m_pause = false;
    m_pause_mutex.unlock();
    m_pause_cond.wakeAll();
}


void SigGenProducer::run(void)
{
    std::srand(time(NULL));


    const double mean = 0.0;
    const double stddev = 1.0;
    //std::default_random_engine generator;
    //std::normal_distribution<double> dist(mean, stddev);

    double tmp_real = 0;
    double tmp_imag = 0;

    int idx = 0;

    m_work = true;
    while(m_work)
    {

        m_pause_mutex.lock();
        if(m_pause) m_pause_cond.wait(&m_pause_mutex);
        m_pause_mutex.unlock();


        /*** generate lp-filtered noise ***/
//        conv_array_real[idx] = (std::rand() % 2 * 8192) - 8192;
//        conv_array_imag[idx] = (std::rand() % 2 * 8192) - 8192;

//        tmp_real = 0;
//        tmp_imag = 0;

//        //low pass filter (coefficients generated using matlab)
//        for(int i=0;i<32;i++)
//        {
//            tmp_real += conv_array_real[(i + idx) & 31] * coeff[i];
//            tmp_imag += conv_array_imag[(i + idx) & 31] * coeff[i];
//        }

//        idx = (idx + 1) & 31;

//        std::complex<int16_t> tmp(tmp_real, tmp_imag);

        //not needed
        /*
        //std::complex<int16_t> tmp((std::rand() % 2 * 8192) - 8192, (std::rand() % 2 * 8192) - 8192);
        //std::complex<int16_t> tmp((std::rand() % 2*8192)-8192, (std::rand() % 2*8192)-8192);
        //std::complex<int16_t> tmp(8192.0 * dist(generator), 8192.0 * dist(generator));
        */

        /*** generate sine-wave ***/
        std::complex<int16_t> tmp = 8192.0 * std::exp(m_j * m_phase_acc);
        m_phase_acc += m_phase_inc;
        while(m_phase_acc > 2 * M_PI) m_phase_acc -= 2 * M_PI;
        while(m_phase_acc < 2 * M_PI) m_phase_acc += 2 * M_PI;

        /*** fill buffer **/
        //m_produce_buffer->push(tmp);
        while(!m_produce_buffer->try_push(tmp) && m_work); //use some kind of non-blocking push

    }
    emit finished();
    std::cout << "sig worker finished" << std::endl;
}


/*
void SigGenProducer::run(void)
{
    std::cout << "sig worker" << std::endl;
    m_work = true;
    while(m_work)
    {

        m_pause_mutex.lock();
        if(m_pause) m_pause_cond.wait(&m_pause_mutex);
        m_pause_mutex.unlock();


        for(int i=0;i<1360;i++)
        {

            std::complex<int16_t> tmp = 8192.0 * std::exp(m_j * m_phase_acc);

            //m_produce_buffer->push(tmp);
            while(! m_produce_buffer->try_push(tmp) && m_work); //use some kind of non-blocking push

            m_phase_acc += m_phase_inc;
            while(m_phase_acc > 2 * M_PI) m_phase_acc -= 2 * M_PI;
            while(m_phase_acc < 2 * M_PI) m_phase_acc += 2 * M_PI;
        }
    }
    std::cout << "sig worker finished" << std::endl;
    emit finished();
}
*/

