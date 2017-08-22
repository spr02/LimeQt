#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <iostream>

#include "limesdrconfig.h"
#include "siggenproducer.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //make new lime sdr device
    LimeSDR = new LimeSDRDevice();


    //setup configurator
    LimeConfigureUI = new LimeSDRConfig();
    LimeConfigureUI->show();
    LimeConfigureUI->setDevPtr(LimeSDR);
    //QObject::connect(LimeConfigureUI, SIGNAL(stop_consumer()), SLOT(stopFFT()));    //stop signal is emitted, when stop button is clicked
    //QObject::connect(LimeConfigureUI, SIGNAL(stop_producer()), SLOT(stopSigGen()));


    //make sig gen producer + thread
    m_sine_gen_thread = new QThread();
    m_sine_gen = new SigGenProducer();
    m_sine_gen->setBuffer(LimeSDR->getTxBuffer());
    m_sine_gen->moveToThread(m_sine_gen_thread);
    QObject::connect(m_sine_gen_thread, SIGNAL(started()), m_sine_gen, SLOT(run()));
    //QObject::connect(m_sine_gen, SIGNAL(finished()), m_sine_gen_thread, SLOT(quit()));
    //QObject::connect(m_sine_gen, SIGNAL(finished()), m_sine_gen, SLOT(deleteLater())); //automatically delete worker class
    //QObject::connect(m_sine_gen_thread, SIGNAL(finished()), m_sine_gen_thread, SLOT(deleteLater())); //automatically delete thread
    m_sine_gen_thread->start();

    //make fft plot (consumer) + thread and connect it to the updatePlot function
    m_fft_consume_thread = new QThread();
    m_fft_plot = new FFTConsumer();
    m_fft_plot->setBuffer(LimeSDR->getRxBuffer());
    m_fft_plot->moveToThread(m_fft_consume_thread);
    QObject::connect(m_fft_consume_thread, SIGNAL(started()), m_fft_plot, SLOT(run()));
    //QObject::connect(m_fft_plot, SIGNAL(finished()), m_fft_consume_thread, SLOT(quit()));
    //QObject::connect(m_fft_plot, SIGNAL(finished()), m_fft_plot, SLOT(deleteLater())); //automatically delete worker class
    //QObject::connect(m_fft_consume_thread, SIGNAL(finished()), m_fft_consume_thread, SLOT(deleteLater())); //automatically delete thread
    m_fft_consume_thread->start();

    QObject::connect(m_fft_plot, SIGNAL(updatePlot(double*)), SLOT(updateFFT(double*))); //updatePlot is generate from fft consumer


    //generate some sample datat for fft plot
    fft_window.setlength(1024);
    double samp_rate = 10.0e6;
    for(int i=0;i<1024;i++) {
        x_val[i] =  (i - 512.0) * 10.0/1024.0 + 106.0;
        y_val[i] = std::cos(2* M_PI * 400000 / samp_rate * i);
        //fft_window[i] = alglib::complex(y_val[i], 0);
        fft_window[i] = alglib::complex((std::rand() % 2 * 8192) - 8192, (std::rand() % 2 * 8192) - 8192);
    }
    alglib::fftc1d(fft_window, 1024);
    for(int i=0;i<1024;i++) y_val[i] = fft_window[i].x * fft_window[i].x + fft_window[i].y * fft_window[i].y;

    //make new plot
    plot = new QwtPlot();
    curve = new QwtPlotCurve();
    curve->setRawSamples(x_val, y_val, 1024);
    curve->setPen( Qt::blue, 1 );
    curve->attach(plot);

    plot->setTitle( "FFT Plot" );
    plot->setCanvasBackground( Qt::white );
    //plot->setAxisScale( QwtPlot::yLeft, 0.0, 10.0);
    plot->show();

}

MainWindow::~MainWindow()
{
    delete ui;

    m_sine_gen->stop();
    m_sine_gen_thread->quit();  //call quit here as unfortunately the connect of worker finished signal an QThread quit slot does not work in the constructor
    m_sine_gen_thread->wait();
    delete m_sine_gen_thread;
    delete m_sine_gen;

    m_fft_plot->stop();
    m_fft_consume_thread->quit();
    m_fft_consume_thread->wait();
    delete m_fft_consume_thread;
    delete m_fft_plot;

    std::cout << "deleted fft thread"<< std::endl;


    std::cout << "delete limesdr2" << std::endl;
    delete LimeSDR;
    delete LimeConfigureUI;
}


void MainWindow::updateFFT(double *data)
{
    curve->setRawSamples(x_val, data, 1024);
    plot->replot();
}

void MainWindow::stopFFT (void)
{
    m_fft_plot->stop();
}

void MainWindow::stopSigGen (void)
{
    m_sine_gen->stop();
}



