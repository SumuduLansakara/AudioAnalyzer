#pragma once
#include <cmath>
#include "wave_player.h"

class sine_wave_player : public wave_player
{
public:
    sine_wave_player(unsigned int channels, double frequency) :
    wave_player{channels, frequency}
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
