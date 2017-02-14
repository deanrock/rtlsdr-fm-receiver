#include "datapointmodel.h"

DataPointModel::DataPointModel(QObject *parent) : QAbstractTableModel(parent)
{

}


int DataPointModel::rowCount(const QModelIndex &parent) const
{
    return 100000;
}

int DataPointModel::columnCount(const QModelIndex &parent) const
{
    return 2;
}

QVariant DataPointModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
        {
        if (index.column() == 0) {
                return QString::number(this->dataPoints[index.row()].real());
            }else{
                return QString::number(this->dataPoints[index.row()].imag());
            }
        }
        return QVariant();
}
