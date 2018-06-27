#pragma once

#include <portaudio.h>

class audio_stream
{
public:
    audio_stream();
    audio_stream(const audio_stream& orig) = delete;
    void operator=(const audio_stream& orig) = delete;
    virtual ~audio_stream();

    bool is_active() const;

    void start();
    void close();
    void stop();

protected:
    PaStream *mStream;
};
