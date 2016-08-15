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



                //this->resample();

                samplingThread = new SamplingThread();
                    connect(samplingThread, &SamplingThread::dataAvailable, this, &MainWindow::resample);
                    //connect(workerThread, &WorkerThread::finished, workerThread, &QObject::deleteLater);
                    samplingThread->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resample()
{
    ui->tableView->setModel(&myModel);
    ui->tableView->show();
    ui->tableView->reset();




    customPlot = ui->customPlot;
    // generate some data:
    QVector<double> x(5000), y(5000); // initialize with entries 0..100
     samplingThread->mutex.lock();
    for (int i=0; i<5000; i++)
    {
        double a = samplingThread->data->dataPoints[i].real(); //(double)buffer[i*2]-127.5;
        double b = samplingThread->data->dataPoints[i].imag();//(double)buffer[i*2+1]-127.5;

        x[i] = i;
       y[i] = b;
      //x[i] = i/50.0 - 1; // x goes from -1 to 1
      //y[i] = x[i]*x[i]; // let's plot a quadratic function
    }

     samplingThread->mutex.unlock();
     int minY =-1000, maxY=1000;
     for(int i= 0;i<5000;i++) {

         if (minY > y[i]) {
             y[i] = minY;
         }

         if (maxY < y[i]) {
             y[i] = maxY;
         }
     }

    // create graph and assign data to it:
    customPlot->addGraph();
    customPlot->graph(0)->setData(x, y);
    // give the axes some labels:
    customPlot->xAxis->setLabel("x");
    customPlot->yAxis->setLabel("y");
    // set axes ranges, so we see all data:
    customPlot->xAxis->setRange(0, 5000);
    customPlot->yAxis->setRange(minY, maxY);
    customPlot->replot();
}

void MainWindow::on_pushButton_clicked()
{
}
