#pragma once

#include <portaudio.h>

class audio_stream {
public:
    audio_stream(unsigned int channels, double sampleRate, int sampleFormat);
    audio_stream(const audio_stream& orig) = delete;
    void operator=(const audio_stream& orig) = delete;
    virtual ~audio_stream();

    bool is_active() const;

    void start();
    void close();
    void stop();

protected:
    const unsigned int mChannels;
    const double mSampleRate;
    const int mSampleFormat;
    PaStream *mStream;
};
