# pragma once

# include <string>
# include <vector>

class PaStreamParameters;
class PaDeviceInfo;
class Device
{
public:
    Device(unsigned int device_no, int sample_format);

    void debug_print() const;
private:
    void load_device();
    void load_sample_rates(const PaDeviceInfo*);

    std::vector<double> get_supported_sample_rates(const PaStreamParameters*, const PaStreamParameters*) const;

    const unsigned int device_no;
    const int sample_format;

    std::string name;
    std::string host_api;
    float default_sampling_rate;

    int max_input_channels;
    float default_low_input_latency;
    float default_high_input_latency;

    int max_output_channels;
    float default_low_output_latency;
    float default_high_output_latency;
};

