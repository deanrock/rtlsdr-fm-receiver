#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <complex>
#include "datapointmodel.h"
#include "samplingthread.h"
#include <gnuradio/filter/firdes.h>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    myModel(0),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // set-up UI
    customPlot = ui->customPlot;
    customPlot->addGraph();
    customPlot->xAxis->setLabel("x");
    customPlot->yAxis->setLabel("y");

    ui->tableView->setModel(&myModel);
    ui->tableView->show();

    // start sampling thread
    samplingThread = new SamplingThread();
    connect(samplingThread, &SamplingThread::dataAvailable, this, &MainWindow::resample);
    samplingThread->start();

    //gr::filter::firdes::low_pass()
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resample()
{
    if (count == 0) {
        int size = 10000;
        QVector<double> x(size), y(size);
        samplingThread->mutex.lock();

        double mx = abs(samplingThread->data->dataPoints[0]);
        for (int i=0; i<size; i++)
        {
            complex<double> b = samplingThread->data->dataPoints[i];;

            if (mx < abs(b)) {
                mx = abs(b);
            }
        }

        int minY =-100, maxY=100;
        for (int i=0; i<size; i++)
        {
            complex<double> b = samplingThread->data->dataPoints[i];

            double z = 20*log10(abs(b)/mx);

            x[i] = i;
            y[i] = z;

            myModel.dataPoints[i] = z;

            // find min/max y[i]
            if (minY > y[i]) {
                y[i] = minY;
            }

            if (maxY < y[i]) {
                y[i] = maxY;
            }
        }

        for (int i=0; i<size; i++)
        {
            y[i] = y[i] * 2 + 50;
        }

        samplingThread->mutex.unlock();

        ui->tableView->reset();

        customPlot->graph(0)->setData(x, y);

        customPlot->xAxis->setRange(0, size);
        customPlot->yAxis->setRange(minY, maxY);
        customPlot->replot();
    }

    count++;
    if (count > 10) {
        count = 0;
    }
}

void MainWindow::on_pushButton_clicked()
{
}
