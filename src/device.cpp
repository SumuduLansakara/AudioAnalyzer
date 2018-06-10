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

    if (not mHalfDuplexInputSampleRates.empty()) {
        cout << "half duplex input sample rates:" << endl;
        for (double sr : mHalfDuplexInputSampleRates) {
            cout << sr << " ";
        }
        cout << endl;
    }

    if (not mHalfDuplexOutputSampleRates.empty()) {
        cout << "half duplex output sample rates:" << endl;
        for (double sr : mHalfDuplexOutputSampleRates) {
            cout << sr << " ";
        }
        cout << endl;
    }

    if (not mFullDuplexSampleRates.empty()) {
        cout << "full duplex sample rates:" << endl;
        for (double sr : mFullDuplexSampleRates) {
            cout << sr << " ";
        }
        cout << endl;
    }
}

const double device::STANDARD_SAMPLE_RATES[] = {
    8000.0, 9600.0, 11025.0, 12000.0, 16000.0, 22050.0, 24000.0, 32000.0,
    44100.0, 48000.0, 88200.0, 96000.0, 192000.0
};

device::device(unsigned int deviceNo, int sampleFormat) :
mDeviceIndex{deviceNo},
mSampleFormat{sampleFormat},
mInputParams{},
mOutputParams{},
mName{},
mHostApi{},
mDefaultSampleRate{},
mMaxInputChannels{},
mDefaultLowInputLatency{},
mDefaultHighInputLatency{},
mMaxOutputChannels{},
mDefaultLowOutputLatency{},
mDefaultHighOutputLatency{},
mHalfDuplexInputSampleRates{},
mHalfDuplexOutputSampleRates{},
mFullDuplexSampleRates{}
{
    load_device();
    load_sample_rates();
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

void device::load_sample_rates()
{
    mInputParams.device = mDeviceIndex;
    mInputParams.channelCount = mMaxInputChannels;
    mInputParams.sampleFormat = mSampleFormat;
    mInputParams.suggestedLatency = 0;
    mInputParams.hostApiSpecificStreamInfo = nullptr;

    mOutputParams.device = mDeviceIndex;
    mOutputParams.channelCount = mMaxOutputChannels;
    mOutputParams.sampleFormat = mSampleFormat;
    mOutputParams.suggestedLatency = 0;
    mOutputParams.hostApiSpecificStreamInfo = nullptr;

    mHalfDuplexInputSampleRates = get_supported_sample_rates(&mInputParams, nullptr);
    mHalfDuplexOutputSampleRates = get_supported_sample_rates(nullptr, &mOutputParams);
    mFullDuplexSampleRates = get_supported_sample_rates(&mInputParams, &mOutputParams);
}

vector<double> device::get_supported_sample_rates(
        const PaStreamParameters *inputParams,
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
