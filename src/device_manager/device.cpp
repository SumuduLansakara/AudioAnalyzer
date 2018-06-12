#include <iostream>
#include <iterator>
#include "device.h"

using std::cout;
using std::endl;
using std::vector;

void device::debug_print() const
{
    cout << "name: " << mName << endl;
    cout << "host API: " << mHostApi << endl;
    cout << "default sampling rate: " << mDefaultSampleRate << endl;

    cout << "max input channels: " << mMaxInputChannels << endl;
    cout << "default low input latency: " << mDefaultLowInputLatency << endl;
    cout << "default high input latency: " << mDefaultHighInputLatency << endl;

    cout << "max output channels: " << mMaxOutputChannels << endl;
    cout << "default low output latency: " << mDefaultLowOutputLatency << endl;
    cout << "default high output latency: " << mDefaultHighOutputLatency << endl;
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
