#pragma once

#include<fstream>
#include <fftw3.h>
#include <portaudio.h>
#include <cmath>

#include "ChannelStatus.h"
#include "cyclic_buffer_container.h"

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

    inline float get_RMS(unsigned int startIndex, unsigned int endIndex) const;
    inline float get_mean(float* buffer, unsigned int startIndex, unsigned int endIndex) const;
    inline float get_std(float* buffer, float mean, unsigned int startIndex, unsigned int endIndex) const;

    void save_raw_audio(const std::string& tag) const;
    void save_wav_audio(const std::string& tag) const;

    float* mShapingWindow;
    float mReferenceAmplitude;
    float mDefaultNoiseDB;
    float* pAmplitudesDB;
    unsigned int mNextHistoryIndex;
    float* pHistNoiseRMS;
    float* pHistSignalRMS;
    ChannelStatus mStatus;

    cyclic_buffer_container mCyclicBuffer;
    float* mInput;
    fftwf_complex* mOutput;
    fftwf_plan mFFTPlan;
    std::ofstream mFile;
};
