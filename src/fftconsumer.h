#ifndef FFTCONSUMER_H
#define FFTCONSUMER_H

#include <QObject>
#include <QMutex>
#include <QWaitCondition>
#include <complex>
#include <cmath>

#include <qwt/qwt_plot_curve.h>
#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_grid.h>

#include <libalglib/fasttransforms.h>

#include "ring_buf_spsc.h"

class FFTConsumer : public QObject
{
    Q_OBJECT
public:
    explicit FFTConsumer(QObject *parent = nullptr);
    ~FFTConsumer();

    void setBuffer(RingBufferSPSC<std::complex<int16_t>>* p_buf);

    void stop (void);
    void pause (void);
    void resume (void);

    //circular shift
    void circshift(double *in, double *out, const int size, const int shift);

    //some window functions
    void hann(double *out, int size);
    void blackman_harris_4(double *out, int size);
    void flat_top(double *out, int size);

signals:
    void finished();
    void updatePlot(double *data);
    void error(QString err);

public slots:
    void run (void);

private:


    double w_window[1024];
    double y_plt[1024];
    int cnt;

    QMutex m_pause_mutex;
    QWaitCondition m_pause_cond;
    volatile bool m_pause;
    volatile bool m_work;


    double x_val[1024], y_val[1024];

    alglib::complex_1d_array m_fft_window;

    RingBufferSPSC<std::complex<int16_t>>* m_consume_buffer;
};

#endif // FFTCONSUMER_H
