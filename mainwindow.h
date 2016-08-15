#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qcustomplot.h"
#include "datapointmodel.h"
#include "sampledata.h"
#include <complex>
#include "samplingthread.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QCustomPlot *customPlot;
    DataPointModel myModel;

    void resample();
    SamplingThread *samplingThread;
};

#endif // MAINWINDOW_H
