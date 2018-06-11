#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>

#include "analyzer.h"

using std::cout;
using std::endl;

spectrum_analyzer::spectrum_analyzer(unsigned int channels, double sampleRate,
                                     unsigned long framesPerBuffer, unsigned int windowLength) :
mChannels{channels}, mSampleRate{sampleRate}, mFramesPerBuffer{framesPerBuffer},
mWindowLength{windowLength}, mShapingWindow{new double[mWindowLength]},
mInput{static_cast<double*> (fftw_malloc(sizeof (double)*mWindowLength))},
mOutput{static_cast<fftw_complex*> (fftw_malloc(sizeof (fftw_complex) * mWindowLength))},
mFFTPlan{fftw_plan_dft_r2c_1d(mWindowLength, mInput, mOutput, FFTW_ESTIMATE)}
{
    for (unsigned int i = 0; i < mWindowLength; ++i) {
        mShapingWindow[i] = 0.5 * (1 - cos(2 * M_PI * i / (mWindowLength - 1)));
    }
}

spectrum_analyzer::~spectrum_analyzer()
{
}

unsigned int spectrum_analyzer::get_real_index(unsigned int index, unsigned int channel) const
{
    assert((index * mChannels) + channel < mFramesPerBuffer * mChannels);
    return (index * mChannels) +channel;
}

void spectrum_analyzer::analyze_buffer(const double * inputBuffer,
                                       unsigned long framesPerBuffer,
                                       const PaStreamCallbackTimeInfo* timeInfo,
                                       PaStreamCallbackFlags statusFlags)
{
    (void) framesPerBuffer;
    (void) timeInfo;
    (void) statusFlags;
    for (unsigned int i = 0; i < mFramesPerBuffer; i += mWindowLength) {
        // DEBUG_BUFFER(0, buffer, i, fft_window_length);
        analyze_window(0, inputBuffer, i, mWindowLength);
    }
}

void spectrum_analyzer::analyze_window(unsigned int channel, const double* buffer, unsigned int start_id, unsigned int len)
{
    for (unsigned int i{0}; i < len; ++i) {
        const double sample = buffer[get_real_index(i + start_id, channel)];
        mInput[i] = sample * mShapingWindow[i];
    }

    fftw_execute(mFFTPlan);

    double* amp = new double[mWindowLength];
    // double amp_db[N];     
    unsigned int high_index = 0;
    double high_val = -99;
    for (unsigned int i{0}; i < mWindowLength; i++) {
        double val = sqrt(mOutput[i][0] * mOutput[i][0] + mOutput[i][1] * mOutput[i][1]);
        if (val >= high_val) {
            high_val = val;
            high_index = i;
        }
        amp[i] = sqrt(mOutput[i][0] * mOutput[i][0] + mOutput[i][1] * mOutput[i][1]);
        //amp_db[i]=20*log(amp[i]);
    }

    const double binSize = mSampleRate / mWindowLength;
    cout << high_index << " " << (high_index * binSize) << endl;
}

void spectrum_analyzer::DEBUG_BUFFER(unsigned int channel, float* buffer, unsigned int start_index, unsigned int len)
{
    double sum = 0;
    float minimum = 999999;
    float maximum = -999999;
    for (unsigned int i = start_index; i < start_index + len; ++i) {
        const float sample = buffer[get_real_index(i, channel)];
        minimum = std::min(minimum, sample);
        maximum = std::max(maximum, sample);
        sum += sample;
    }
    cout << std::fixed << std::setprecision(7) << std::setfill(' ');
    cout << "[ "
            << std::setw(10) << (sum / len) << ", "
            << std::setw(10) << minimum << ", "
            << std::setw(10) << maximum << " ]";
}


