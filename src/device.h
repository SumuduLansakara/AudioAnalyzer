#pragma once

#include <string>
#include <vector>
#include <portaudio.h>

class PaStreamParameters;

class device
{
public:
    device(unsigned int device_no, int sample_format);
    device(const device&) = delete;
    void operator=(const device&) = delete;

    const PaStreamParameters& input_parameters() const
    {
        return mInputParams;
    }

    const PaStreamParameters& output_parameters() const
    {
        return mOutputParams;
    }

    void debug_print() const;

private:
    void load_device();
    void load_sample_rates();

    std::vector<double> get_supported_sample_rates(const PaStreamParameters*,
                                                   const PaStreamParameters*) const;

    static const double STANDARD_SAMPLE_RATES[];

    const unsigned int mDeviceIndex;
    const int mSampleFormat;

    PaStreamParameters mInputParams;
    PaStreamParameters mOutputParams;

    std::string mName;
    std::string mHostApi;
    double mDefaultSampleRate;

    int mMaxInputChannels;
    double mDefaultLowInputLatency;
    double mDefaultHighInputLatency;

    int mMaxOutputChannels;
    double mDefaultLowOutputLatency;
    double mDefaultHighOutputLatency;

    std::vector<double> mHalfDuplexInputSampleRates;
    std::vector<double> mHalfDuplexOutputSampleRates;
    std::vector<double> mFullDuplexSampleRates;
};

