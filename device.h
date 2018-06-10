#pragma once

#include <string>
#include <vector>

class PaStreamParameters;

class Device {
public:
    Device(unsigned int device_no, int sample_format);

    void debug_print() const;
private:
    void load_device();
    void load_sample_rates();

    std::vector<double> get_supported_sample_rates(const PaStreamParameters*,
            const PaStreamParameters*) const;

    static const double STANDARD_SAMPLE_RATES[];

    const unsigned int device_no;
    const int sample_format;

    std::string name;
    std::string host_api;
    double default_sampling_rate;

    int max_input_channels;
    double default_low_input_latency;
    double default_high_input_latency;

    int max_output_channels;
    double default_low_output_latency;
    double default_high_output_latency;

    std::vector<double> half_duplex_in_s_rates;
    std::vector<double> half_duplex_out_s_rates;
    std::vector<double> full_duplex_s_rates;
};

