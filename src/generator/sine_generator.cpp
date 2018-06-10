#include <cmath>
#include <exception>
#include <iostream>
#include <portaudio.h>

#include "device.h"
#include "device_manager.h"
#include "sine_generator.h"
#include "settings.h"

using std::cout;
using std::endl;

sine_generator::sine_generator(unsigned int channels, double frequency) :
mChannels{channels}, mFrequency{frequency}, mStream{nullptr},
mTableLength{static_cast<int> (SAMPLE_RATE / frequency)}, mTableIndex{0},
mTable{new float[mTableLength]}
{

    for (int i{0}; i < mTableLength; i++) {
        mTable[i] = static_cast<float> (sin(2 * M_PI * ((double) i / mTableLength)));
    }
}

sine_generator::~sine_generator()
{
    delete [] mTable;
}

void sine_generator::debug_print() const
{
    for (int i{0}; i < mTableLength; i++) {
        cout << mTable[i] << " ";
    }
    cout << endl;
}

void sine_generator::setup_stream(device* outputDevice)
{
    PaStreamParameters outputParameters = outputDevice->output_parameters();
    outputParameters.channelCount = mChannels;
    outputParameters.suggestedLatency = outputDevice->default_low_output_latency();
    outputParameters.hostApiSpecificStreamInfo = nullptr;

    PaError err{Pa_OpenStream(
        &mStream,
        nullptr, /* no input */
        &outputParameters,
        SAMPLE_RATE,
        paFramesPerBufferUnspecified,
        paClipOff,
        &sine_generator::stream_data_callback,
        this
        )};
    device_manager::get_instance()->check_error(err);

    err = Pa_SetStreamFinishedCallback(mStream, &sine_generator::stream_finished_callback);
    if (err != paNoError) {
        Pa_CloseStream(mStream);
        device_manager::get_instance()->check_error(err);
    }
}

void sine_generator::close()
{
    if (mStream == nullptr) {
        throw std::runtime_error("stream is already closed");
    }

    PaError err{Pa_CloseStream(mStream)};
    mStream = nullptr;
    device_manager::get_instance()->check_error(err);
}

void sine_generator::start()
{
    if (mStream == nullptr) {
        throw std::runtime_error("stream is closed");
    }
    device_manager::get_instance()->check_error(Pa_StartStream(mStream));
}

void sine_generator::stop()
{
    if (mStream == nullptr) {
        throw std::runtime_error("stream is closed");
    }
    device_manager::get_instance()->check_error(Pa_StopStream(mStream));
}

int sine_generator::on_stream_data(const void *inputBuffer,
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

int sine_generator::stream_data_callback(const void *inputBuffer, void *outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags,
        void *userData)
{
    return ((sine_generator*) userData)->on_stream_data(inputBuffer,
            outputBuffer, framesPerBuffer, timeInfo, statusFlags);
}

void sine_generator::stream_finished_callback(void* userData)
{
    return ((sine_generator*) userData)->on_stream_finish();
}

void sine_generator::on_stream_finish()
{
    cout << "stream finished" << endl;
}
