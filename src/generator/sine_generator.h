#pragma once

class device;

class sine_generator
{
public:
    sine_generator(unsigned int channels, double frequency);
    sine_generator(const sine_generator& orig) = delete;
    void operator=(const sine_generator& orig) = delete;
    ~sine_generator();

    void setup_stream(device* outputDevice);
    void start();
    void close();
    void stop();

    void debug_print() const;
private:
    static void stream_finished_callback(void* userData);
    void on_stream_finish();

    static int stream_data_callback(const void *inputBuffer, void *outputBuffer,
                                    unsigned long framesPerBuffer,
                                    const PaStreamCallbackTimeInfo* timeInfo,
                                    PaStreamCallbackFlags statusFlags,
                                    void *userData);
    int on_stream_data(const void *inputBuffer, void *outputBuffer,
                       unsigned long framesPerBuffer,
                       const PaStreamCallbackTimeInfo* timeInfo,
                       PaStreamCallbackFlags statusFlags);

    const unsigned int mChannels;
    const double mFrequency;
    PaStream *mStream;
    int mTableLength;
    int mTableIndex;
    float* mTable;
};