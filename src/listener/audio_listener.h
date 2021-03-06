#pragma once
#include <portaudio.h>
#include "device_manager/device.h"
#include "common/audio_stream.h"

class audio_listener : public audio_stream {
public:
    audio_listener(unsigned int channels, double sampleRate, int sampleFormat,
            unsigned long framesPerBuffer);
    audio_listener(const audio_listener& orig) = delete;
    void operator=(const audio_listener& orig) = delete;
    virtual ~audio_listener();

    void setup_stream(device* inputDevice);

    bool is_listening() const;

private:
    static int listen_callback(const void *inputBuffer, void *outputBuffer,
            unsigned long framesPerBuffer,
            const PaStreamCallbackTimeInfo* timeInfo,
            PaStreamCallbackFlags statusFlags,
            void *userData);

    int on_listen(const float * inputBuffer, unsigned long framesPerBuffer,
            const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags);

    static void listen_finished_callback(void* userData);
    void on_listen_finished();

    const unsigned long mFramesPerBuffer;
};