#-------------------------------------------------
#
# Project created by QtCreator 2016-08-15T12:33:50
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = rtlsdr-fm-receiver
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp \
    datapointmodel.cpp \
    sampledata.cpp \
    samplingthread.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h \
    datapointmodel.h \
    sampledata.h \
    samplingthread.h

FORMS    += mainwindow.ui

LIBS += /usr/local/lib/librtlsdr.dylib /usr/local/lib/libfftw3.dylib
INCLUDEPATH += /usr/local/include/
