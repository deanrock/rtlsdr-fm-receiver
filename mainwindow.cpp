#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <rtl-sdr.h>
#include <fftw3.h>
#include <complex>
#include "datapointmodel.h"
using namespace std;
rtlsdr_dev_t *dev = NULL;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    myModel(0),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    //rtlsdr_read_sync(dev, buf, len, n_read);

    int r = rtlsdr_open(&dev, (uint32_t)0);
        if (r < 0) {
            fprintf(stderr, "Failed to open rtlsdr device #%d.\n", 10);
            exit(1);
        }


         r = rtlsdr_set_sample_rate(dev, 2500000);
            if (r < 0) {
                fprintf(stderr, "WARNING: Failed to set sample rate.\n");
                exit(1);
            } else {
                //fprintf(stderr, "Sampling at %u S/s.\n");
            }


            r = rtlsdr_set_center_freq(dev, 94000000);
                if (r < 0) {
                    fprintf(stderr, "WARNING: Failed to set center freq.\n");
                    exit(1);
                } else {
                    //fprintf(stderr, "Tuned to %u Hz.\n");
                }

                this->resample();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resample()
{
    int r = rtlsdr_reset_buffer(dev);
        if (r < 0) {
            fprintf(stderr, "WARNING: Failed to reset buffers.\n");
        exit(1);
        }



    int out_block_size = 16 * 16384;
    int n_read;
    uint8_t *buffer = (uint8_t*)malloc(out_block_size * sizeof(uint8_t));
    r = rtlsdr_read_sync(dev, buffer, out_block_size, &n_read);
    if (r < 0) {
                    fprintf(stderr, "WARNING: sync read failed.\n");
                    exit(1);
                }

    complex<double>com[5000];
    for(int i=0;i<5000;i++) {
        double a = (double)buffer[i*2]-127.5;
       double b = (double)buffer[i*2+1]-127.5;
        com[i] = complex<double>(a,b);

    }


    fftw_complex *in, *out;
        fftw_plan p;

        in = (fftw_complex*) reinterpret_cast<fftw_complex*>(com);//fftw_malloc(sizeof(fftw_complex) * 5000);
        out = (fftw_complex*) reinterpret_cast<fftw_complex*>(myModel.dataPoints);
        p = fftw_plan_dft_1d(5000, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

        fftw_execute(p); /* repeat as needed */




    /*for(int i = 0;i<out_block_size; i+=2) {
        uint8_t x = buffer[i];
        uint8_t y=  buffer[i+1];
        double a = (double)buffer[i]-127.5;
        double b = (double)buffer[i+1]-127.5;
        //printf("%f %f\n", a, b);
    }*/


    printf("read: %d", n_read);
//exit(1);
    ui->tableView->setModel(&myModel);
    ui->tableView->show();
    ui->tableView->reset();




    customPlot = ui->customPlot;
    // generate some data:
    QVector<double> x(5000), y(5000); // initialize with entries 0..100
    for (int i=0; i<5000; i++)
    {
        double a = out[i][0]; //(double)buffer[i*2]-127.5;
        double b = out[i][1];//(double)buffer[i*2+1]-127.5;

        x[i] = i;
       y[i] = b;
      //x[i] = i/50.0 - 1; // x goes from -1 to 1
      //y[i] = x[i]*x[i]; // let's plot a quadratic function
    }

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

    fftw_destroy_plan(p);
}

void MainWindow::on_pushButton_clicked()
{
    resample();
}
