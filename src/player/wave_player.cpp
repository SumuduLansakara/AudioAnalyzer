#include <exception>
#include <iostream>
#include <portaudio.h>

#include "device_manager/device.h"
#include "device_manager/device_manager.h"
#include "wave_player.h"

using std::cout;
using std::endl;

wave_player::wave_player(unsigned int channels, const double sample_rate, double frequency) :
mChannels{channels}, mSampleRate{sample_rate}, mFrequency{frequency},
mStream{nullptr}, mTableIndex{0},
mTableLength{static_cast<int> (sample_rate / frequency)},
mTable{new float[mTableLength]}
{
}

wave_player::~wave_player()
{
    delete [] mTable;
}

void wave_player::debug_print() const
{
    for (int i{0}; i < mTableLength; i++) {
        cout << mTable[i] << " ";
    }
    cout << endl;
}

void wave_player::setup_stream(device* outputDevice)
{
    generate_wave_table();

    PaStreamParameters outputParameters{outputDevice->output_parameters()};
    outputParameters.channelCount = mChannels;
    outputParameters.suggestedLatency = outputDevice->default_low_output_latency();
    outputParameters.hostApiSpecificStreamInfo = nullptr;

    PaError err{Pa_OpenStream(
                              &mStream,
                              nullptr, /* no input */
                              &outputParameters,
                              mSampleRate,
                              paFramesPerBufferUnspecified,
                              paClipOff,
                              &wave_player::stream_data_callback,
                              this
                              )};
    device_manager::get_instance()->check_error(err);

    err = Pa_SetStreamFinishedCallback(mStream, &wave_player::stream_finished_callback);
    if (err != paNoError) {
        Pa_CloseStream(mStream);
        device_manager::get_instance()->check_error(err);
    }
}

void wave_player::close()
{
    if (mStream == nullptr) {
        throw std::runtime_error("stream is already closed");
    }

    PaError err{Pa_CloseStream(mStream)};
    mStream = nullptr;
    device_manager::get_instance()->check_error(err);
}

void wave_player::play()
{
    if (mStream == nullptr) {
        throw std::runtime_error("stream is closed");
    }
    device_manager::get_instance()->check_error(Pa_StartStream(mStream));
}

void wave_player::stop()
{
    if (mStream == nullptr) {
        throw std::runtime_error("stream is closed");
    }
    device_manager::get_instance()->check_error(Pa_StopStream(mStream));
}

int wave_player::on_stream_data(const void *inputBuffer,
                                void *outputBuffer,
                                unsigned long framesPerBuffer,
                                const PaStreamCallbackTimeInfo* timeInfo,
                                PaStreamCallbackFlags statusFlags)
{
    float *out{(float*) outputBuffer};

    (void) timeInfo;
    (void) statusFlags;
    (void) inputBuffer;

    for (unsigned long i{0}; i < framesPerBuffer; i++) {
        for (unsigned int c{0}; c < mChannels; ++c) {
            *out++ = mTable[mTableIndex];
        }
        mTableIndex += 1;
        if (mTableIndex >= mTableLength) mTableIndex -= mTableLength;
    }

    return paContinue;
}

int wave_player::stream_data_callback(const void *inputBuffer, void *outputBuffer,
                                      unsigned long framesPerBuffer,
                                      const PaStreamCallbackTimeInfo* timeInfo,
                                      PaStreamCallbackFlags statusFlags,
                                      void *userData)
{
    return ((wave_player*) userData)->on_stream_data(inputBuffer,
                                                     outputBuffer, framesPerBuffer, timeInfo, statusFlags);
}

void wave_player::stream_finished_callback(void* userData)
{
    return ((wave_player*) userData)->on_stream_finish();
}

void wave_player::on_stream_finish()
{
    cout << "stream finished" << endl;
}
