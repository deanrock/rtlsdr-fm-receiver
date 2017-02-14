#include "samplingthread.h"
#include <QFile>

void SamplingThread::run()
{
    this->data = new SampleData();

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


    r = rtlsdr_set_center_freq(dev, 94300000);
    if (r < 0) {
        fprintf(stderr, "WARNING: Failed to set center freq.\n");
        exit(1);
    } else {
        //fprintf(stderr, "Tuned to %u Hz.\n");
    }

    r = rtlsdr_reset_buffer(dev);
    if (r < 0) {
        fprintf(stderr, "WARNING: Failed to reset buffers.\n");
        exit(1);
    }

    /*QFile file(QString("/Users/dean/Desktop/sample5.dat"));
    if (!file.open(QIODevice::ReadOnly ))
    {printf("aaa\n");return ;}

    int size = file.size();*/

    int out_block_size = 16384;
    int n_read;

    vector<SampleData>samples;

    deque<complex<double>> temp;

    uint8_t *buffer = (uint8_t*)malloc(out_block_size * sizeof(uint8_t));

    for(;;) {
        r = rtlsdr_read_sync(dev, buffer, out_block_size, &n_read);
        if (r < 0) {
            fprintf(stderr, "WARNING: sync read failed.\n");
            exit(1);
        }

        /*qint64 bufSize = out_block_size;
        qint64 dataSize;

            dataSize = file.read((char*)buffer, bufSize);
printf("%d\n", dataSize);
            if (file.atEnd()) {
                printf("end\n");
                break;
            }*/



        for(int i =0;i<out_block_size/2;i++) {
            double a = (double)buffer[i*2]-127.5;
            double b = (double)buffer[i*2+1]-127.5;
            temp.push_back(complex<double>(a,b));
        }

        for(;;) {
            // we have at least 5000 data points
            if (temp.size() >= 10000) {
                // process those data points
                complex<double> *com = new complex<double>[10000];

                for (int i =0;i<10000;i++) {
                    com[i] = temp.front();
                    temp.pop_front();
                }

                fftw_complex *in, *out;
                fftw_plan p;

                // beware: fftw should modify input and output buffers
                complex<double>*outx = new complex<double>[10000];
                in = (fftw_complex*) reinterpret_cast<fftw_complex*>(com);
                out = (fftw_complex*) reinterpret_cast<fftw_complex*>(outx);
                p = fftw_plan_dft_1d(10000, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

                fftw_execute(p);

                fftw_destroy_plan(p);

                this->mutex.lock();
                if(this->data->dataPoints != NULL) {
                    delete[] this->data->dataPoints;
                }

                this->data->dataPoints = outx;
                this->mutex.unlock();
                emit dataAvailable();

                delete[] com;

                QThread::msleep(5);
            }else{
                break;
            }
        }
    }
}
