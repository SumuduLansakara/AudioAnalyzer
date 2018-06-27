#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>

#include "analyzer.h"
#include "settings.h"

using std::cout;
using std::endl;

spectrum_analyzer::spectrum_analyzer() : mShapingWindow{new float[ANALYZER_FFT_WINDOW_LENGTH]},
pAmplitudes{new float[ANALYZER_FFT_WINDOW_LENGTH]},
mInput{static_cast<sampleType*> (fftw_malloc(sizeof (sampleType) * ANALYZER_FFT_WINDOW_LENGTH))},
mOutput{static_cast<fftw_complex*> (fftw_malloc(sizeof (fftw_complex) * ANALYZER_FFT_WINDOW_LENGTH))},
mFFTPlan{fftw_plan_dft_r2c_1d(ANALYZER_FFT_WINDOW_LENGTH, mInput, mOutput, FFTW_ESTIMATE)}
{
    cout << "listener sample rate: " << LISTENER_SAMPLE_RATE << endl;
    throw 1;
    for (unsigned int i = 0; i < ANALYZER_FFT_WINDOW_LENGTH; ++i) {
        mShapingWindow[i] = 0.5 * (1 - cos(2 * M_PI * i / (ANALYZER_FFT_WINDOW_LENGTH - 1)));
    }
}

spectrum_analyzer::~spectrum_analyzer()
{
}

unsigned int spectrum_analyzer::get_real_index(unsigned int index, unsigned int channel) const
{
    assert((index * LISTENER_CHANNELS) + channel < LISTENER_FRAMES_PER_BUFFER * LISTENER_CHANNELS);
    return (index * LISTENER_CHANNELS) +channel;
}

void spectrum_analyzer::analyze_buffer(const float * inputBuffer,
                                       unsigned long framesPerBuffer,
                                       const PaStreamCallbackTimeInfo* timeInfo,
                                       PaStreamCallbackFlags statusFlags)
{
    (void) framesPerBuffer;
    (void) timeInfo;
    (void) statusFlags;
    for (unsigned int i = 0; i < LISTENER_FRAMES_PER_BUFFER; i += ANALYZER_FFT_WINDOW_LENGTH) {
        //        debug_print_window(0, inputBuffer, i, ANALYZER_FFT_WINDOW_LENGTH);
        analyze_window(0, inputBuffer, i, ANALYZER_FFT_WINDOW_LENGTH);
    }
}

void spectrum_analyzer::analyze_window(unsigned int channel, const float* buffer, unsigned int start_id,
                                       unsigned int len)
{
    for (unsigned int i{0}; i < len; ++i) {
        const float sample = buffer[get_real_index(i + start_id, channel)];
        mInput[i] = sample * mShapingWindow[i];
    }

    fftw_execute(mFFTPlan);

    unsigned int high_index = 0;
    float high_val = -99;
    for (unsigned int i{0}; i < ANALYZER_FFT_WINDOW_LENGTH; i++) {
        float val = sqrt(mOutput[i][0] * mOutput[i][0] + mOutput[i][1] * mOutput[i][1]);
        if (val >= high_val) {
            high_val = val;
            high_index = i;
        }
        pAmplitudes[i] = sqrt(mOutput[i][0] * mOutput[i][0] + mOutput[i][1] * mOutput[i][1]);
    }

    const float binSize = LISTENER_SAMPLE_RATE / ANALYZER_FFT_WINDOW_LENGTH;
    cout << high_index << " " << (high_index * binSize) << endl;
}

float spectrum_analyzer::bin_to_freq(unsigned int bid) const
{
    return bid * LISTENER_SAMPLE_RATE / ANALYZER_FFT_WINDOW_LENGTH;
}

void spectrum_analyzer::debug_print_window(unsigned int channel, const float* buffer, unsigned int start_index, unsigned int len)
{
    float sum = 0;
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