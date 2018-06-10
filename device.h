#pragma once

#include <string>
#include <vector>

class PaStreamParameters;

class device
{
public:
    device(unsigned int device_no, int sample_format);

    void debug_print() const;

private:
    void load_device();
    void load_sample_rates();

    std::vector<double> get_supported_sample_rates(const PaStreamParameters*,
                                                   const PaStreamParameters*) const;

    static const double STANDARD_SAMPLE_RATES[];

    const unsigned int mDeviceIndex;
    const int mSampleFormat;

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

