#pragma once
#include "common/audio_stream.h"

class device;

class audio_player : public audio_stream {
public:
    audio_player(unsigned int channels, double sampleRate, int sampleFormat, double frequency);
    audio_player(const audio_player& orig) = delete;
    void operator=(const audio_player& orig) = delete;
    virtual ~audio_player();

    void setup_stream(device* outputDevice);

    void debug_print() const;

private:
    static void play_finished_callback(void* userData);
    void on_play_finished();

    static int play_callback(const void *inputBuffer, void *outputBuffer,
            unsigned long framesPerBuffer,
            const PaStreamCallbackTimeInfo* timeInfo,
            PaStreamCallbackFlags statusFlags,
            void *userData);
    int on_play(const void *inputBuffer, void *outputBuffer,
            unsigned long framesPerBuffer,
            const PaStreamCallbackTimeInfo* timeInfo,
            PaStreamCallbackFlags statusFlags);

    const double mFrequency;
    int mPhase;

protected:
    virtual void generate_wave_table() = 0;
    int mTableLength;
    float* mTable;

};