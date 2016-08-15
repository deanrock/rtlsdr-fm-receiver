#ifndef SAMPLINGTHREAD_H
#define SAMPLINGTHREAD_H

#include <QThread>
#include <rtl-sdr.h>
#include <vector>
#include <complex>
#include <fftw3.h>
#include <qmutex.h>
#include <deque>
#include "sampledata.h"

using namespace std;

class SamplingThread : public QThread
{
    Q_OBJECT
private:
    void run();
    rtlsdr_dev_t *dev = NULL;
public:
    QMutex mutex;
    SampleData *data;
signals:
    void dataAvailable();
};

#endif // SAMPLINGTHREAD_H
