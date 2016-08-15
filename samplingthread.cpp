#include "samplingthread.h"

void SamplingThread::run()
{
    printf("lal\n");

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


    r = rtlsdr_set_center_freq(dev, 94500000);
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



    int out_block_size = 16384;
    int n_read;

    vector<SampleData>samples;

    deque<complex<double>> temp;

    uint8_t *buffer = (uint8_t*)malloc(out_block_size * sizeof(uint8_t));

    int averageCount = 0;
    complex<double>averageDataPoints[5000];

    for(;;) {
        r = rtlsdr_read_sync(dev, buffer, out_block_size, &n_read);
        if (r < 0) {
            fprintf(stderr, "WARNING: sync read failed.\n");
            exit(1);
        }

        for(int i =0;i<out_block_size/2;i++) {
            double a = (double)buffer[i*2]-127.5;
            double b = (double)buffer[i*2+1]-127.5;
            temp.push_back(complex<double>(a,b));
        }


        for(;;) {
            // we have at least 5000 data points
            if (temp.size() >= 5000) {

                // process those data points
                complex<double>com[5000];

                for (int i =0;i<5000;i++) {
                    com[i] = temp.front();
                    temp.pop_front();
                }

                fftw_complex *in, *out;
                fftw_plan p;
                complex<double>outx[5000];
                in = (fftw_complex*) reinterpret_cast<fftw_complex*>(com);//fftw_malloc(sizeof(fftw_complex) * 5000);
                out = (fftw_complex*) reinterpret_cast<fftw_complex*>(outx);
                p = fftw_plan_dft_1d(5000, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

                fftw_execute(p); /* repeat as needed */

                for (int i =0;i<5000;i++) {
                    double x =  outx[i].real() *  outx[i].real() +  outx[i].imag() *  outx[i].imag();
                    averageDataPoints[i] += complex<double>(1, x);
                }

                 fftw_destroy_plan(p);

                if (averageCount > 500) {
                     complex<double>com[5000];
                    for (int i =0;i<5000;i++) {
                        com[i] = complex<double>(1, averageDataPoints[i].imag() / averageCount);
                        averageDataPoints[i]=0;
                        averageCount = 0;
                    }





                    this->mutex.lock();
                    std::copy(std::begin(outx), std::end(outx), std::begin(this->data->dataPoints));
                    this->mutex.unlock();
                    emit dataAvailable();


                    printf("read: %d", n_read);


                }else{
                    averageCount++;
                }
            }else{
                break;
            }
        }
    }
}
