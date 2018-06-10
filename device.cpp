# include "device.h"

#include <iostream>
#include <portaudio.h>

using std::cout;
using std::endl;
using std::vector;

Device::Device(unsigned int device_no, int sample_format):
    device_no{device_no},sample_format{sample_format}
{
    load_device();
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

    load_sample_rates(info);
}

void Device::load_sample_rates(const PaDeviceInfo* info)
{
    PaStreamParameters in_params;
    in_params.device = device_no;
    in_params.channelCount = info->maxInputChannels;
    in_params.sampleFormat = paInt16;
    in_params.suggestedLatency = 0;
    in_params.hostApiSpecificStreamInfo = nullptr;
    
    PaStreamParameters out_params;
    out_params.device = device_no;
    out_params.channelCount = info->maxOutputChannels;
    out_params.sampleFormat = paInt16;
    out_params.suggestedLatency = 0;
    out_params.hostApiSpecificStreamInfo = nullptr;

    get_supported_sample_rates(nullptr, nullptr);
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
}

vector<double> Device::get_supported_sample_rates(const PaStreamParameters *in_params,
        const PaStreamParameters *out_params) const
{
    static double standardSampleRates[] = {
        8000.0, 9600.0, 11025.0, 12000.0, 16000.0, 22050.0, 24000.0, 32000.0,
        44100.0, 48000.0, 88200.0, 96000.0, 192000.0, -1 /* negative terminated  list */
    };
    vector<double> supported_s_rates;
    for(int i=0; standardSampleRates[i] > 0; i++ )
    {
        PaError err = Pa_IsFormatSupported(in_params, out_params, standardSampleRates[i]);
        if( err == paFormatIsSupported )
        {
            supported_s_rates.push_back(standardSampleRates[i]);
        }
    }
    return supported_s_rates;
}
