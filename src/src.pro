#-------------------------------------------------
#
# Project created by QtCreator 2017-07-19T00:09:16
#
#-------------------------------------------------

QT       += core gui concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LimeQt
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been markeLimeQt.pro.userd as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QMAKE_CXXFLAGS += -O1

LIBS += -lSoapySDR -lqwt -lalglib

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    limesdrdevice.cpp \
    limesdrconfig.cpp \
    limerxstreamworker.cpp \
    limetxstreamworker.cpp \
    siggenproducer.cpp \
    fftconsumer.cpp

HEADERS += \
        mainwindow.h \
    ring_buf_spsc.h \
    limesdrdevice.h \
    limesdrconfig.h \
    limerxstreamworker.h \
    limetxstreamworker.h \
    siggenproducer.h \
    fftconsumer.h

FORMS += \
        mainwindow.ui \
    limesdrconfig.ui



