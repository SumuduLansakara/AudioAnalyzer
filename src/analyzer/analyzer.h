#pragma once

#include <fftw3.h>
#include <portaudio.h>

using sampleType = double;

class spectrum_analyzer
{
public:
    spectrum_analyzer();
    spectrum_analyzer(const spectrum_analyzer& orig) = delete;
    void operator=(const spectrum_analyzer& orig) = delete;
    virtual ~spectrum_analyzer();

    unsigned int get_real_index(unsigned int index, unsigned int channel) const;

    void analyze_buffer(const float * inputBuffer, unsigned long framesPerBuffer,
                        const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags);

    void analyze_window(unsigned int channel, const float* buffer, unsigned int start_id, unsigned int len);

private:
    void debug_print_window(unsigned int channel, const float* buffer, unsigned int start_index, unsigned int len);
    inline float bin_to_freq(unsigned int bid) const;

    float* mShapingWindow;
    float* pAmplitudes;

    sampleType* mInput;
    fftw_complex* mOutput;
    fftw_plan mFFTPlan;
};