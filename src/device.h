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

    const std::string& name() const
    {
        return mName;
    }

    double default_sample_rate() const
    {
        return mDefaultSampleRate;
    }

    const PaStreamParameters& input_parameters() const
    {
        return mInputParams;
    }

    const PaStreamParameters& output_parameters() const
    {
        return mOutputParams;
    }

    int max_input_channels() const
    {
        return mMaxInputChannels;
    }

    double default_low_input_latency() const
    {
        return mDefaultLowOutputLatency;
    }

    double default_high_input_latency() const
    {
        return mDefaultHighOutputLatency;
    }

    int max_output_channels() const
    {
        return mMaxOutputChannels;
    }

    double default_low_output_latency() const
    {
        return mDefaultLowOutputLatency;
    }

    double default_high_output_latency() const
    {
        return mDefaultHighOutputLatency;
    }

    const std::vector<double> half_duplex_input_sample_rates() const
    {
        return mHalfDuplexInputSampleRates;
    }

    const std::vector<double> half_duplex_output_sample_rates() const
    {
        return mHalfDuplexOutputSampleRates;
    }

    const std::vector<double> full_duplex_sample_rates() const
    {
        return mFullDuplexSampleRates;
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

    std::string mName;
    std::string mHostApi;
    double mDefaultSampleRate;

    PaStreamParameters mInputParams;
    PaStreamParameters mOutputParams;

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

