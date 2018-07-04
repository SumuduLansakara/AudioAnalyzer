#include <cmath>
#include <exception>
#include <iostream>
#include <portaudio.h>

#include "device_manager/device.h"
#include "device_manager/device_manager.h"
#include "mocked_player.h"
#include "settings.h"

using std::cout;
using std::endl;

mocked_player::mocked_player() : mPhase{0},
mTableLength{static_cast<int> (PLAYER_SAMPLE_RATE / GENERATOR_SINE_WAVE_FREQUENCY)}, mTable{new float[mTableLength]}
{
    cout << "mocked player constructed" << endl;
    for (int i{0}; i < mTableLength; i++) {
        mTable[i] = static_cast<float> (sin(2 * M_PI * ((double) i / mTableLength)));
    }
}

mocked_player::~mocked_player()
{
    delete [] mTable;
}

void mocked_player::debug_print() const
{
    for (int i{0}; i < mTableLength; i++) {
        cout << mTable[i] << " ";
    }
    cout << endl;
}

int mocked_player::on_play(float *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags)
{
    (void) timeInfo;
    (void) statusFlags;

    for (unsigned long i{0}; i < framesPerBuffer; i++) {
        for (unsigned int c{0}; c < PLAYER_CHANNELS; ++c) {
            *outputBuffer++ = mTable[mPhase];
        }
        mPhase += 1;
        if (mPhase >= mTableLength) mPhase -= mTableLength;
    }
    return paContinue;
}
