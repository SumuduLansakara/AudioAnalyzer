#pragma once

#include <fftw3.h>
#include <portaudio.h>

class spectrum_analyzer {
public:
    spectrum_analyzer(unsigned int channels, double mSampleRate, unsigned long framesPerBuffer, unsigned int windowLength);
    spectrum_analyzer(const spectrum_analyzer& orig) = delete;
    void operator=(const spectrum_analyzer& orig) = delete;
    virtual ~spectrum_analyzer();

    unsigned int get_real_index(unsigned int index, unsigned int channel) const;

    void analyze_buffer(const double * inputBuffer,
            unsigned long framesPerBuffer,
            const PaStreamCallbackTimeInfo* timeInfo,
            PaStreamCallbackFlags statusFlags);

    void analyze_window(unsigned int channel, const double* buffer, unsigned int start_id, unsigned int len);

private:
    void debug_print_window(unsigned int channel, const double* buffer, unsigned int start_index, unsigned int len);

    const unsigned int mChannels;
    const double mSampleRate;
    const unsigned long mFramesPerBuffer;
    const unsigned int mWindowLength;
    double* mShapingWindow;

    double* mInput;
    fftw_complex* mOutput;
    fftw_plan mFFTPlan;
};