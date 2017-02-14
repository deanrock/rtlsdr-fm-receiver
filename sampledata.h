#ifndef SAMPLEDATA_H
#define SAMPLEDATA_H

#include <complex>
#include <QMetaType>

class SampleData
{
public:
    SampleData();
    std::complex<double>* dataPoints = NULL;
};

Q_DECLARE_METATYPE(SampleData);

#endif // SAMPLEDATA_H
