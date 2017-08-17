#ifndef LIMESDRCONFIG_H
#define LIMESDRCONFIG_H

#include <QWidget>
#include <QThread>
#include <QTimer>

#include <SoapySDR/Version.hpp>
#include <SoapySDR/Modules.hpp>
#include <SoapySDR/Registry.hpp>
#include <SoapySDR/Device.hpp>
#include <SoapySDR/Formats.hpp>

#include "limesdrdevice.h"

namespace Ui {
class LimeSDRConfig;
}

class LimeSDRConfig : public QWidget
{
    Q_OBJECT

public:
    //constructor + desctructor
    explicit LimeSDRConfig(QWidget *parent = 0);
    ~LimeSDRConfig();

    //setter method for the device pointer
    void setDevPtr(LimeSDRDevice* p_dev);
    LimeSDRDevice* getDevPtr (void);


private slots:

    //refresh and connect button
    void on_refresh_but_clicked();
    void on_connect_but_clicked();

    //antenna selectors and sliders/spin boxes
    void on_rx_antenna_combo_currentTextChanged(const QString &arg1);
    void on_tx_antenna_combo_currentTextChanged(const QString &arg1);

    void on_lna_slider_valueChanged(int value);
    void on_tia_slider_valueChanged(int value);
    void on_pga_slider_valueChanged(int value);
    void on_pad_slider_valueChanged(int value);

    //set buttons from middle
    void on_set_lo_freq_but_clicked();
    void on_set_fir_bw_but_clicked();
    void on_set_rate_clicked();
    void on_test_rate_clicked();

    //lower right buttons (six)
    void on_calib_button_clicked();
    void on_calib_tx_button_clicked();
    void on_calib_rx_button_clicked();

    void on_start_button_clicked();
    void on_save_button_clicked();
    void on_load_button_clicked();

    //rate and buffer usage update
    void update_rate (void);

signals:
    void stop_consumer (void);
    void stop_producer (void);

private:
    //UI for the configurator
    Ui::LimeSDRConfig *ui;

    //bools for connect and streaming status
    bool m_started, m_connected;

    //arglist and actual sdr pointer
    SoapySDR::KwargsList devicesKwargs;
    LimeSDRDevice* m_sdr_dev;

    //timer to update the rate + progress and current head + tail
    QTimer *m_update_timer;
    uint64_t m_rx_head;
    uint64_t m_rx_tail;
    uint64_t m_tx_head;
    uint64_t m_tx_tail;

};

#endif // LIMESDRCONFIG_H
