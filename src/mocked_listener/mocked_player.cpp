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
audio_stream(channels, sampleRate, sampleFormat, framesPerBuffer), mFrequency{frequency}, mPhase{0},
mTableLength{static_cast<int> (sampleRate / frequency)}, mTable{new float[mTableLength]}
{
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

void mocked_player::setup_stream(device* outputDevice)
{
    generate_wave_table();

    PaStreamParameters outputParameters{outputDevice->output_parameters()};
    outputParameters.channelCount = mChannels;
    outputParameters.sampleFormat = mSampleFormat;
    outputParameters.suggestedLatency = outputDevice->default_low_output_latency();
    outputParameters.hostApiSpecificStreamInfo = nullptr;

    PaError err{Pa_OpenStream(&mStream, nullptr, &outputParameters, mSampleRate, mFramesPerBuffer, paClipOff,
                              &mocked_player::play_callback, this)};
    device_manager::get_instance()->check_error(err);

    err = Pa_SetStreamFinishedCallback(mStream, &mocked_player::play_finished_callback);
    if (err != paNoError) {
        Pa_CloseStream(mStream);
        device_manager::get_instance()->check_error(err);
    }
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

int mocked_player::play_callback(const void *, void *outputBuffer, unsigned long framesPerBuffer,
                                 const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags,
                                 void *userData)
{
    return static_cast<mocked_player*> (userData)->on_play(static_cast<float*> (outputBuffer), framesPerBuffer,
                                                           timeInfo, statusFlags);
}

void mocked_player::play_finished_callback(void* userData)
{
    return ((mocked_player*) userData)->on_play_finished();
}

void mocked_player::on_play_finished()
{
    cout << "play finished" << endl;
}

void mocked_player::generate_wave_table()
{
    for (int i{0}; i < mTableLength; i++) {
        mTable[i] = static_cast<float> (sin(2 * M_PI * ((double) i / mTableLength)));
    }
}