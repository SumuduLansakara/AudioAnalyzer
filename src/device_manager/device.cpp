#include <iostream>
#include <iterator>
#include "device.h"
#include "utilities/logger.h"

using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::to_string;

void device::debug_print()
{
    logger::debug("name: " + mName);
    logger::debug("host API: " + mHostApi);
    logger::debug("default sampling rate: " + to_string(mDefaultSampleRate));
    logger::debug("max input channels: " + to_string(mMaxInputChannels));
    logger::debug("default low input latency: " + to_string(mDefaultLowInputLatency));
    logger::debug("default high input latency: " + to_string(mDefaultHighInputLatency));
    logger::debug("max output channels: " + to_string(mMaxOutputChannels));
    logger::debug("default low output latency: " + to_string(mDefaultLowOutputLatency));
    logger::debug("default high output latency: " + to_string(mDefaultHighOutputLatency));
}

const double device::STANDARD_SAMPLE_RATES[] = {
    8000.0, 9600.0, 11025.0, 12000.0, 16000.0, 22050.0, 24000.0, 32000.0,
    44100.0, 48000.0, 88200.0, 96000.0, 192000.0
};

device::device(int deviceId) :
mDeviceIndex{deviceId},
mName{},
mHostApi{},
mDefaultSampleRate{},
mInputParams{},
mOutputParams{},
mMaxInputChannels{},
mDefaultLowInputLatency{},
mDefaultHighInputLatency{},
mMaxOutputChannels{},
mDefaultLowOutputLatency{},
mDefaultHighOutputLatency{}
{
    load_device();
}

void device::load_device()
{
    const PaDeviceInfo * info{Pa_GetDeviceInfo(mDeviceIndex)};
    mName = info->name;
    mHostApi = Pa_GetHostApiInfo(info->hostApi)->name;
    mDefaultSampleRate = info->defaultSampleRate;

    mMaxInputChannels = info->maxInputChannels;
    mDefaultLowInputLatency = info->defaultLowInputLatency;
    mDefaultHighInputLatency = info->defaultHighInputLatency;

    mMaxOutputChannels = info->maxOutputChannels;
    mDefaultLowOutputLatency = info->defaultLowOutputLatency;
    mDefaultHighOutputLatency = info->defaultHighOutputLatency;
}

vector<double> device::get_supported_sample_rates(const PaStreamParameters *inputParams,
                                                  const PaStreamParameters *outputParams) const
{
    vector<double> supportedSampleRates;
    for (double sr : STANDARD_SAMPLE_RATES) {
        if (Pa_IsFormatSupported(inputParams, outputParams, sr) == paFormatIsSupported) {
            supportedSampleRates.push_back(sr);
        }
    }
    return supportedSampleRates;
}
