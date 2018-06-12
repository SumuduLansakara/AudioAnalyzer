#include <exception>
#include <iostream>
#include <portaudio.h>

#include "device_manager/device.h"
#include "device_manager/device_manager.h"
#include "audio_player.h"

using std::cout;
using std::endl;

audio_player::audio_player(unsigned int channels, const double sampleRate, int sampleFormat,
                           unsigned long framesPerBuffer, double frequency) :
audio_stream(channels, sampleRate, sampleFormat, framesPerBuffer), mFrequency{frequency}, mPhase{0},
mTableLength{static_cast<int> (sampleRate / frequency)}, mTable{new float[mTableLength]}
{
}

audio_player::~audio_player()
{
    delete [] mTable;
}

void audio_player::debug_print() const
{
    for (int i{0}; i < mTableLength; i++) {
        cout << mTable[i] << " ";
    }
    cout << endl;
}

void audio_player::setup_stream(device* outputDevice)
{
    generate_wave_table();

    PaStreamParameters outputParameters{outputDevice->output_parameters()};
    outputParameters.channelCount = mChannels;
    outputParameters.sampleFormat = mSampleFormat;
    outputParameters.suggestedLatency = outputDevice->default_low_output_latency();
    outputParameters.hostApiSpecificStreamInfo = nullptr;

    PaError err{Pa_OpenStream(&mStream, nullptr, &outputParameters, mSampleRate, mFramesPerBuffer, paClipOff,
                              &audio_player::play_callback, this)};
    device_manager::get_instance()->check_error(err);

    err = Pa_SetStreamFinishedCallback(mStream, &audio_player::play_finished_callback);
    if (err != paNoError) {
        Pa_CloseStream(mStream);
        device_manager::get_instance()->check_error(err);
    }
}

int audio_player::on_play(float *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
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

int audio_player::play_callback(const void *, void *outputBuffer, unsigned long framesPerBuffer,
                                const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags,
                                void *userData)
{
    return static_cast<audio_player*> (userData)->on_play(static_cast<float*> (outputBuffer), framesPerBuffer,
                                                          timeInfo, statusFlags);
}

void audio_player::play_finished_callback(void* userData)
{
    return ((audio_player*) userData)->on_play_finished();
}

void audio_player::on_play_finished()
{
    cout << "play finished" << endl;
}
