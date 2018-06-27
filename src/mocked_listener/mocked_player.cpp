#include <cmath>
#include <exception>
#include <iostream>
#include <portaudio.h>

#include "device_manager/device.h"
#include "device_manager/device_manager.h"
#include "mocked_player.h"

using std::cout;
using std::endl;

mocked_player::mocked_player(unsigned int channels, const double sampleRate, int sampleFormat,
                             unsigned long framesPerBuffer, double frequency) :
audio_stream(channels, sampleRate, sampleFormat, framesPerBuffer), mPhase{0},
mTableLength{static_cast<int> (sampleRate / frequency)}, mTable{new float[mTableLength]}
{
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
        for (unsigned int c{0}; c < mChannels; ++c) {
            *outputBuffer++ = mTable[mPhase];
        }
        mPhase += 1;
        if (mPhase >= mTableLength) mPhase -= mTableLength;
    }
    return paContinue;
}
