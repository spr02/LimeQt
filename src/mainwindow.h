#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>

#include <qwt/qwt_plot_curve.h>
#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_grid.h>

#include <libalglib/fasttransforms.h>

#include "limesdrconfig.h"
#include "limesdrdevice.h"
#include "siggenproducer.h"
#include "fftconsumer.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void stopFFT (void);
    void stopSigGen (void);
    void updateFFT(double *data);

private:
    Ui::MainWindow *ui;
    LimeSDRConfig *LimeConfigureUI;
    LimeSDRDevice *LimeSDR;

    //signal generator (producer)
    QThread* m_sine_gen_thread;
    SigGenProducer *m_sine_gen;

    //fft plot (consumer)
    QThread* m_fft_consume_thread;
    FFTConsumer *m_fft_plot;


    QwtPlot *plot;
    QwtPlotCurve *curve;

    alglib::complex_1d_array fft_window;
    double x_val[1024];
    double y_val[1024];



};

#endif // MAINWINDOW_H
