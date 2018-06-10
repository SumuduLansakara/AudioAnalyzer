#include "device.h"

#include <iostream>
#include <portaudio.h>
#include <iterator>

using std::cout;
using std::endl;
using std::vector;

const double Device::STANDARD_SAMPLE_RATES[] = {
    8000.0, 9600.0, 11025.0, 12000.0, 16000.0, 22050.0, 24000.0, 32000.0,
    44100.0, 48000.0, 88200.0, 96000.0, 192000.0
};

Device::Device(unsigned int device_no, int sample_format) :
device_no{device_no},
sample_format{sample_format},
name{},
host_api{},
default_sampling_rate{},
max_input_channels{},
default_low_input_latency{},
default_high_input_latency{},
max_output_channels{},
default_low_output_latency{},
default_high_output_latency{},
half_duplex_in_s_rates{},
half_duplex_out_s_rates{},
full_duplex_s_rates{}
{
    load_device();
    load_sample_rates();
}

void Device::load_device()
{
    const PaDeviceInfo* info = Pa_GetDeviceInfo(device_no);
    name = info->name;
    host_api = Pa_GetHostApiInfo(info->hostApi)->name;
    default_sampling_rate = info->defaultSampleRate;

    max_input_channels = info->maxInputChannels;
    default_low_input_latency = info->defaultLowInputLatency;
    default_high_input_latency = info->defaultHighInputLatency;

    max_output_channels = info->maxOutputChannels;
    default_low_output_latency = info->defaultLowOutputLatency;
    default_high_output_latency = info->defaultHighOutputLatency;
}

void Device::load_sample_rates()
{
    PaStreamParameters in_params;
    in_params.device = device_no;
    in_params.channelCount = max_input_channels;
    in_params.sampleFormat = sample_format;
    in_params.suggestedLatency = 0;
    in_params.hostApiSpecificStreamInfo = nullptr;

    PaStreamParameters out_params;
    out_params.device = device_no;
    out_params.channelCount = max_output_channels;
    out_params.sampleFormat = sample_format;
    out_params.suggestedLatency = 0;
    out_params.hostApiSpecificStreamInfo = nullptr;

    half_duplex_in_s_rates = get_supported_sample_rates(&in_params, nullptr);
    half_duplex_out_s_rates = get_supported_sample_rates(nullptr, &out_params);
    full_duplex_s_rates = get_supported_sample_rates(&in_params, &out_params);
}

vector<double> Device::get_supported_sample_rates(const PaStreamParameters *in_params,
        const PaStreamParameters *out_params) const
{
    vector<double> supported_s_rates;
    for (unsigned int i = 0; i < std::size(STANDARD_SAMPLE_RATES); i++) {
        PaError err = Pa_IsFormatSupported(in_params, out_params, STANDARD_SAMPLE_RATES[i]);
        if (err == paFormatIsSupported) {
            supported_s_rates.push_back(STANDARD_SAMPLE_RATES[i]);
        }
    }
    return supported_s_rates;
}

void Device::debug_print() const
{
    cout << "name: " << name << endl;
    cout << "host API: " << host_api << endl;
    cout << "default sampling rate: " << default_sampling_rate << endl;

    cout << "max input channels: " << max_input_channels << endl;
    cout << "default low input latency: " << default_low_input_latency << endl;
    cout << "default high input latency: " << default_high_input_latency << endl;

    cout << "max output channels: " << max_output_channels << endl;
    cout << "default low output latency: " << default_low_output_latency << endl;
    cout << "default high output latency: " << default_high_output_latency << endl;

    cout << "half duplex input sample rates:" << endl;
    for (double sr : half_duplex_in_s_rates) {
        cout << sr << " ";
    }
    cout << endl;

    cout << "half duplex output sample rates:" << endl;
    for (double sr : half_duplex_out_s_rates) {
        cout << sr << " ";
    }
    cout << endl;

    cout << "full duplex sample rates:" << endl;
    for (double sr : full_duplex_s_rates) {
        cout << sr << " ";
    }
    cout << endl;
}

