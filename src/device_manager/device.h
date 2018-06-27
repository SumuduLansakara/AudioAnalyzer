#pragma once

#include <string>
#include <vector>
#include <portaudio.h>

class PaStreamParameters;

class device
{
public:
    device(int deviceId);
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

    std::vector<double> get_supported_sample_rates(const PaStreamParameters*, const PaStreamParameters*) const;


    void debug_print() const;

private:
    void load_device();

    static const double STANDARD_SAMPLE_RATES[];

    const int mDeviceIndex;

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
};

