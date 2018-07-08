#pragma once
#include <portaudio.h>
#include "device_manager/device.h"
#include "common/audio_stream.h"
#include"mocked_player.h"

class spectrum_analyzer;
class buffer_cache;

class mocked_listener : public audio_stream
{
public:
    mocked_listener(buffer_cache* cache);

    ~mocked_listener()
    {
    }

    mocked_listener(const mocked_listener& orig) = delete;
    void operator=(const mocked_listener& orig) = delete;

    void setup_non_blocking_stream(device* inputDevice, spectrum_analyzer* analyzer);

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
    mocked_player mPlayer;
    float* pFakeInputBuffer;
    buffer_cache* pCache;
};
