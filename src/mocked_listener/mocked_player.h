#pragma once
#include "common/audio_stream.h"

class device;

class mocked_player : public audio_stream
{
public:
    mocked_player(int frequency);
    mocked_player(const mocked_player& orig) = delete;
    void operator=(const mocked_player& orig) = delete;
    virtual ~mocked_player();

    void debug_print() const;

    int on_play(float *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
                PaStreamCallbackFlags statusFlags);

private:
    int mFrequency;
    int mPhase;
    int mTableLength;
    float* mTable;
};
