#pragma once
#include <portaudio.h>
#include "device_manager/device.h"
#include "common/audio_stream.h"

class spectrum_analyzer;

class mocked_listener : public audio_stream {
public:
    mocked_listener(unsigned int channels, double sampleRate, int sampleFormat, unsigned long framesPerBuffer);
    mocked_listener(const mocked_listener& orig) = delete;
    void operator=(const mocked_listener& orig) = delete;
    virtual ~mocked_listener();

    void setup_non_blocking_stream(device* inputDevice, spectrum_analyzer* analyzer);

    void setup_blocking_stream(device* inputDevice, spectrum_analyzer* analyzer);
    void start_blocking_listen_loop();

    bool is_listening() const;

private:
    static int listen_callback(const void *inputBuffer, void *outputBuffer,
            unsigned long framesPerBuffer,
            const PaStreamCallbackTimeInfo* timeInfo,
            PaStreamCallbackFlags statusFlags,
            void *userData);

    int on_listen(const float* inputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
            PaStreamCallbackFlags statusFlags);

    static void listen_finished_callback(void* userData);
    void on_listen_finished();

    spectrum_analyzer* pAnalyzer;
};
