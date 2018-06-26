#pragma once
#include "common/audio_stream.h"

class device;

class mocked_player : public audio_stream {
public:
    mocked_player(unsigned int channels, double sampleRate, int sampleFormat, unsigned long framesPerBuffer,
            double frequency);
    mocked_player(const mocked_player& orig) = delete;
    void operator=(const mocked_player& orig) = delete;
    virtual ~mocked_player();

    void setup_stream(device* outputDevice);

    void debug_print() const;

    int on_play(float *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
            PaStreamCallbackFlags statusFlags);

private:
    static void play_finished_callback(void* userData);
    void on_play_finished();

    static int play_callback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
            const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData);
    //    int on_play(float *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
    //            PaStreamCallbackFlags statusFlags);

    const double mFrequency;
    int mPhase;

protected:
    void generate_wave_table();
    int mTableLength;
    float* mTable;

};