#include <iostream>
#include "analyzer/analyzer.h"
#include "mocked_listener.h"
#include "device_manager/device_manager.h"
#include "settings.h"
#include "utilities/logger.h"
#include "buffering/buffer_cache.h"

using std::cout;
using std::endl;
using std::to_string;

mocked_listener::mocked_listener(buffer_cache* cache) : pAnalyzer{nullptr}, mPlayer{440},
pFakeInputBuffer{new float[LISTENER_FRAMES_PER_BUFFER * LISTENED_CHANNELS]}, pCache{cache}
{
    logger::warning("mocked listener constructed");
}

void mocked_listener::setup_non_blocking_stream(device* inputDevice, spectrum_analyzer * analyzer)
{
    pAnalyzer = analyzer;
    PaStreamParameters inputParameters{inputDevice->input_parameters()};
    inputParameters.channelCount = LISTENED_CHANNELS;
    inputParameters.sampleFormat = LISTENER_SAMPLE_FORMAT;
    inputParameters.suggestedLatency = inputDevice->default_high_input_latency();
    inputParameters.hostApiSpecificStreamInfo = nullptr;

    PaError err{Pa_OpenStream(&mStream, &inputParameters, nullptr, LISTENER_SAMPLE_RATE, LISTENER_FRAMES_PER_BUFFER,
                              paClipOff, &mocked_listener::listen_callback, this)};
    device_manager::get_instance()->check_error(err);

    err = Pa_SetStreamFinishedCallback(mStream, &mocked_listener::listen_finished_callback);
    if (err != paNoError) {
        Pa_CloseStream(mStream);
        device_manager::get_instance()->check_error(err);
    }
}

int mocked_listener::listen_callback(const void *inputBuffer, void *outputBuffer,
                                     unsigned long framesPerBuffer,
                                     const PaStreamCallbackTimeInfo* timeInfo,
                                     PaStreamCallbackFlags statusFlags,
                                     void *userData)
{

    (void) outputBuffer;
    return static_cast<mocked_listener*> (userData)->on_listen(static_cast<const float*> (inputBuffer), framesPerBuffer, timeInfo, statusFlags);
}

int mocked_listener::on_listen(const float * inputBuffer,
                               unsigned long framesPerBuffer,
                               const PaStreamCallbackTimeInfo* timeInfo,
                               PaStreamCallbackFlags statusFlags)
{
    (void) inputBuffer;
    if (statusFlags != 0) {
        logger::info("listener status = " + to_string(statusFlags));
    }
    mPlayer.on_play(pFakeInputBuffer, framesPerBuffer, timeInfo, statusFlags);
    pCache->store_buffer(pFakeInputBuffer);
    pAnalyzer->notify();
    // pAnalyzer->analyze_buffer(pFakeInputBuffer, framesPerBuffer, timeInfo, statusFlags);
    return false;
}

void mocked_listener::listen_finished_callback(void* userData)
{
    return static_cast<mocked_listener*> (userData)->on_listen_finished();
}

void mocked_listener::on_listen_finished()
{
    logger::info("mocked listener on_listen_finished()");
}

bool mocked_listener::is_listening() const
{
    return Pa_IsStreamActive(mStream);
}
