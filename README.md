# LimeQT - A simple SoapySDR wrapper with a Qt GUI
This project demonstrates a simple transceiver example using multiple threads for sending/receive and produce/consume. While this project definitely does not cover all features of the LimeSDR, it could still be used as a template/starting point for other applications.

## Dependencies
This project requires LimeSuite, SoapySDR, QWT and alglib (and of course QT and QtCreator)
Steps listed for arch linux using pacaur:

```sh
pacman -S qwt
pacaur -S limesuite-git
pacaur -S soapysdr-git
pacaur -S alglib
```

## Build
Simply fire up QtCreator, load the LimeQt.pro and hit build.

## Run
The application could be run directly or run through QTCreator. For the basic example application, first connect the LimeSDR using the connect button. Then hit the start button in order to start streaming and if done the stop button. 
The LimeSDR is automatically disconnected when the application is closed. Currently there is either a sine wave or LP filtered noise generated and the received signal is simply viewed in the fourier domain. I used an external loopback in order to look at the generated signal.

## Code structre/Data flow
Currently the application uses four threads, that communicate through the thread safe RingBufferSPSC. The LimeSDRDevice and FFTConsumer/SigGenProducer are instantiated in the mainwindow constructor. Further a LimeSDRConfig UI is instantiated and the LimeSDRDevice pointer is assigned to it. For the actual functionality of the LimeSDR, the config UI is not needed.

I think there are many improvements that can be made, but currently i lack of the time to further dig into the LimeSDR. Tough if anyone does implement some improvements, feel free to contact me/share them with the community.


### Block Schematic

                       +--------------------------------------------------+
                       |                                                  |
                       |                   LimeSDRConfig                  |
                       |                                                  |
                       +-------------------------+------------------------+
                                                 |
                       +-------------------------v------------------------+
                       |  LimeSDRDevice                                   |
                       |                                                  |
+----------------+     |  +-----------------+     +--------------------+  |
|                |     |  |                 |     |                    |  |
| FFTConsumer    <--------+ RingBufferSPSC  <-----+ LimeRxStreamWorker |  |
|                |     |  |                 |     |                    |  |
+----------------+     |  +-----------------+     +--------------------+  |
                       |                                                  |
+----------------+     |  +-----------------+     +--------------------+  |
|                |     |  |                 |     |                    |  |
| SigGenProducer +--------> RingBufferSPSC  +-----> LimeTxStreamWorker |  |
|                |     |  |                 |     |                    |  |
+----------------+     |  +-----------------+     +--------------------+  |
                       |                                                  |
                       +--------------------------------------------------+
