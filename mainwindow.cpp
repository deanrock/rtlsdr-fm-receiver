#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <complex>
#include "datapointmodel.h"
#include "samplingthread.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    myModel(0),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // set-up UI
    customPlot = ui->customPlot;

    ui->tableView->setModel(&myModel);
    ui->tableView->show();

    // start sampling thread
    samplingThread = new SamplingThread();
    connect(samplingThread, &SamplingThread::dataAvailable, this, &MainWindow::resample);
    samplingThread->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resample()
{
    if (count == 0) {
        QVector<double> x(5000), y(5000);
        samplingThread->mutex.lock();

        double mx = abs(samplingThread->data->dataPoints[0]);
        for (int i=0; i<5000; i++)
        {
            complex<double> b = samplingThread->data->dataPoints[i];;

            if (mx < abs(b)) {
                mx = abs(b);
            }
        }

        for (int i=0; i<5000; i++)
        {
            complex<double> b = samplingThread->data->dataPoints[i];

            double z = 20*log10(abs(b)/mx);

            x[i] = i;
            y[i] = z;

            myModel.dataPoints[i] = z;
        }

        samplingThread->mutex.unlock();

        ui->tableView->reset();

        int minY =-100, maxY=100;

        for(int i= 0;i<5000;i++) {

            if (minY > y[i]) {
                y[i] = minY;
            }

            if (maxY < y[i]) {
                y[i] = maxY;
            }
        }

        customPlot->addGraph();
        customPlot->graph(0)->setData(x, y);

        customPlot->xAxis->setLabel("x");
        customPlot->yAxis->setLabel("y");

        customPlot->xAxis->setRange(0, 5000);
        customPlot->yAxis->setRange(minY, maxY);
        customPlot->replot();
    }

    count++;

    if (count > 50) {
        count = 0;
    }
}

void MainWindow::on_pushButton_clicked()
{
}
