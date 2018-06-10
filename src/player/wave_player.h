#pragma once

class device;

class wave_player
{
public:
    wave_player(unsigned int channels, double sample_rate, double frequency);
    wave_player(const wave_player& orig) = delete;
    void operator=(const wave_player& orig) = delete;
    virtual ~wave_player();

    void setup_stream(device* outputDevice);
    void play();
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
    const double mSampleRate;
    const double mFrequency;
    PaStream *mStream;
    int mTableIndex;

protected:
    virtual void generate_wave_table() = 0;
    int mTableLength;
    float* mTable;

};