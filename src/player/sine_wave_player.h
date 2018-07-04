#pragma once
#include <cmath>

#include "audio_player.h"

class sine_wave_player : public audio_player
{
public:

    sine_wave_player(int frequency): audio_player(frequency)
    {
    }

private:

    void generate_wave_table()
    {
        for (int i{0}; i < mTableLength; i++) {
            mTable[i] = static_cast<float> (sin(2 * M_PI * ((double) i / mTableLength)));
        }
    }
};
