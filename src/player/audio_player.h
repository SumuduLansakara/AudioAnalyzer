#pragma once
#include "common/audio_stream.h"

class device;

class audio_player : public audio_stream
{
public:
    audio_player(int frequency);
    audio_player(const audio_player& orig) = delete;
    void operator=(const audio_player& orig) = delete;
    virtual ~audio_player();

    void setup_stream(device* outputDevice);

private:
    static void play_finished_callback(void* userData);
    void on_play_finished();

    static int play_callback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
                             const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData);
    int on_play(float *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
                PaStreamCallbackFlags statusFlags);

    int mFrequency;
    int mPhase;

protected:
    virtual void generate_wave_table() = 0;

    int mTableLength;
    float* mTable;
};
