#pragma once

#include <fstream>
#include <fftw3.h>
#include <portaudio.h>
#include <cmath>
#include <string>

#include "ChannelStatus.h"

class buffer_cache;

class spectrum_analyzer
{
public:
    spectrum_analyzer(buffer_cache* cache);
    spectrum_analyzer(const spectrum_analyzer& orig) = delete;
    void operator=(const spectrum_analyzer& orig) = delete;
    virtual ~spectrum_analyzer();

    void notify();

private:
    inline float bin_to_freq(unsigned int bid) const;

    void analyze_window(unsigned int channel, const float* buffer);

    inline float get_RMS(unsigned int startIndex, unsigned int endIndex) const;
    inline float get_mean(float* buffer, unsigned int startIndex, unsigned int endIndex) const;
    inline float get_std(float* buffer, float mean, unsigned int startIndex, unsigned int endIndex) const;

    void save_wav_audio(unsigned int channel, const std::string& tag) const;

    float* mShapingWindow;
    float mReferenceAmplitude;
    float mDefaultNoiseDB;
    float* pAmplitudesDB;
    unsigned int mNextHistoryIndex;
    float* pHistNoiseRMS;
    float* pHistSignalRMS;
    ChannelStatus mStatus;

    float* mInput;
    fftwf_complex* mOutput;
    fftwf_plan mFFTPlan;
    buffer_cache* pCache;
};
