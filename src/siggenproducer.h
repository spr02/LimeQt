#ifndef SIGGENPRODUCER_H
#define SIGGENPRODUCER_H

#include <QObject>
#include <QMutex>
#include <QWaitCondition>
#include <complex>
#include <cmath>
#include "ring_buf_spsc.h"

class SigGenProducer : public QObject
{
    Q_OBJECT
public:
    explicit SigGenProducer(QObject *parent = nullptr);

    void setBuffer(RingBufferSPSC<std::complex<int16_t>>* p_buf);

    void stop (void);
    void pause (void);
    void resume (void);

signals:
    void finished();
    void error(QString err);

public slots:
    void run (void);

private:

    QMutex m_pause_mutex;
    QWaitCondition m_pause_cond;
    volatile bool m_pause;
    volatile bool m_work;

    std::complex<double> m_j;
    double m_phase_acc;
    double m_phase_inc;

    //fc = 0.2
    /*
    double coeff[32] = {6.2371e-19, 0.001195, 0.0027311, 0.0040982, 0.0037943, -2.313e-18, -0.0079047, -0.017798, -0.024361, -0.020434,
                        5.7894e-18, 0.038428, 0.089712, 0.14262, 0.18315, 0.19953, 0.18666, 0.14817, 0.095078, 0.041584, 6.4052e-18,
                        -0.023156, -0.02834, -0.021316, -0.0097778, -2.9645e-18, 0.0050461, 0.0056276, 0.0037829, 0.0015673, 6.9712e-19, -0.00093549};

    */

    //fc = 0.75
    /*
    double coeff[32] = {-0.0012004, 0.002033, -0.0020305, 4.7494e-18, 0.0045645, -0.0094435, 0.0095093, -1.3751e-17, -0.018112, 0.034765, -0.033428,
                        2.402e-17, 0.066701, -0.14996, 0.22033, 0.74823, 0.22455, -0.1558, 0.07069, 2.5992e-17, -0.036984, 0.039395, -0.021071, -1.6469e-17,
                        0.011763, -0.012103, 0.0060705, 6.5219e-18, -0.0028126, 0.0026665, -0.0013417, -2.3389e-18};
    */

    //fc = 0.5
    double coeff[32] = {-0.0016977, 1.7428e-18, 0.0028716, -3.1663e-18, -0.0064552, 5.7825e-18, 0.013448, -9.1674e-18, -0.025615, 1.2772e-17, 0.047274,
                        -1.6013e-17, -0.094329, 1.8364e-17, 0.31159, 0.49882, 0.31756, 1.908e-17, -0.09997, -1.7328e-17, 0.052303, 1.4474e-17, -0.029798,
                        -1.0979e-17, 0.016635, 7.4112e-18, -0.008585, -4.3479e-18, 0.0039776, 2.2859e-18, -0.0018975, -1.5593e-18};


    double conv_array_real[32];
    double conv_array_imag[32];



    RingBufferSPSC<std::complex<int16_t>>* m_produce_buffer;
};

#endif // SIGGENPRODUCER_H
