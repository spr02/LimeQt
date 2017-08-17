#include "fftconsumer.h"

#define AVG_COUNT 256

FFTConsumer::FFTConsumer(QObject *parent) : QObject(parent)
{
    m_fft_window.setlength(1024);
    m_pause = false;
    cnt = 0;

    //hann_window(w_window, 1024);
    //blackman_harris_4(w_window, 1024);
    flat_top(w_window, 1024);

    for(int i=0;i<1024;i++)
    {
        x_val[i] = i;
        y_val[i] = 0.0;
    }

}

FFTConsumer::~FFTConsumer()
{

}

void FFTConsumer::setBuffer(RingBufferSPSC<std::complex<int16_t>>* p_buf)
{
    m_consume_buffer = p_buf;
}

void FFTConsumer::stop(void)
{
    m_work = false;
}

void FFTConsumer::pause (void)
{
    m_pause_mutex.lock();
    m_pause = true;
    m_pause_mutex.unlock();
}

void FFTConsumer::resume (void)
{
    m_pause_mutex.lock();
    m_pause = false;
    m_pause_mutex.unlock();
    m_pause_cond.wakeAll();
}

void FFTConsumer::hann(double *out, int size)
{
    for (int i = 0; i < size; i++) {
        out[i] = 0.5 * (1 - std::cos(2*M_PI*i/(size-1)));
    }
}


void FFTConsumer::blackman_harris_4(double *out, int size)
{
    const int N = size-1;
    const double a[] = {0.35875, 0.48829, 0.14128, 0.01168};
    for(int i=0;i<size;i++)
    {
        out[i] = a[0] - a[1] * std::cos(2*M_PI*i/N) + a[2] * std::cos(4*M_PI*i/N) - a[3] * std::cos(6*M_PI*i/N);
    }
}

void FFTConsumer::flat_top(double *out, int size)
{
    const int N = size-1;
    const double a[] = {1.0, 1.93, 1.29, 0.388, 0.028};
    for(int i=0;i<size;i++)
    {
        out[i] = a[0] - a[1] * std::cos(2*M_PI*i/N) + a[2] * std::cos(4*M_PI*i/N) - a[3] * std::cos(6*M_PI*i/N) + a[4] * std::cos(8*M_PI*i/N);
    }
}


void FFTConsumer::run(void)
{
    std::cout << "fft worker" << std::endl;
    m_work = true;
    while(m_work)
    {

        m_pause_mutex.lock();
        if(m_pause) m_pause_cond.wait(&m_pause_mutex);
        m_pause_mutex.unlock();

        for(int k=0;k<1024;k++)
        {
            std::complex<int16_t> tmp = m_consume_buffer->pop();
            m_fft_window[k] = alglib::complex(w_window[k]*tmp.real(), w_window[k]*tmp.imag());
        }

        alglib::fftc1d(m_fft_window, 1024);


        for(int i=0;i<1024;i++)
        {
            y_val[i] += (m_fft_window[i].x*m_fft_window[i].x + m_fft_window[i].y * m_fft_window[i].y)/4096;
        }

        if(cnt == AVG_COUNT)
        {
            for(int i=0;i<1024;i++)
            {
                y_val[i] = std::log10(y_val[i] / 8);
            }
            circshift(y_val, y_plt, 1024, 512); // circular shift to get centered around 0 with +-Fs/2
            for(int i=0;i<1024;i++)
            {
                y_val[i] = 0;
            }
            emit updatePlot(y_plt);
            cnt = 0;
        }
        cnt++;

    }
    std::cout << "fft worker finished" << std::endl;
    emit finished();

}


void FFTConsumer::circshift(double *in, double *out, const int size, const int shift)
{
    for(int i=0;i<size;i++)
    {
        out[i] = in[(i + shift) % size];
    }
}

