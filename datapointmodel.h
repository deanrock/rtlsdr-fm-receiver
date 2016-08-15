#ifndef DATAPOINTMODEL_H
#define DATAPOINTMODEL_H

#include <QAbstractTableModel>
#include <complex>

using namespace std;

class DataPointModel : public QAbstractTableModel
{
public:
    DataPointModel(QObject *parent);
    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    complex<double>dataPoints[5000];
};

#endif // DATAPOINTMODEL_H
