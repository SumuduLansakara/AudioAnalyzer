#include <iostream>
#include <cassert>
#include "analyzer/analyzer.h"
#include "buffering/buffer_cache.h"
#include "audio_listener.h"
#include "device_manager/device_manager.h"
#include "settings.h"
#include "utilities/logger.h"

using std::cout;
using std::endl;
using std::to_string;

audio_listener::audio_listener(buffer_cache* cache) : pAnalyzer{nullptr}, pCache{cache}
{
}

audio_listener::~audio_listener()
{
}

void audio_listener::setup_non_blocking_stream(device* inputDevice, spectrum_analyzer* analyzer)
{
    pAnalyzer = analyzer;
    PaStreamParameters inputParameters{inputDevice->input_parameters()};
    inputParameters.channelCount = LISTENED_CHANNELS;
    inputParameters.sampleFormat = LISTENER_SAMPLE_FORMAT;
    inputParameters.suggestedLatency = inputDevice->default_high_input_latency();
    inputParameters.hostApiSpecificStreamInfo = nullptr;

    PaError err{Pa_OpenStream(&mStream, &inputParameters, nullptr, LISTENER_SAMPLE_RATE, LISTENER_FRAMES_PER_BUFFER,
                              paClipOff, &audio_listener::listen_callback, this)};
    device_manager::get_instance()->check_error(err);

    err = Pa_SetStreamFinishedCallback(mStream, &audio_listener::listen_finished_callback);
    if (err != paNoError) {
        Pa_CloseStream(mStream);
        device_manager::get_instance()->check_error(err);
    }
}

int audio_listener::listen_callback(const void *inputBuffer, void *outputBuffer,
                                    unsigned long framesPerBuffer,
                                    const PaStreamCallbackTimeInfo* timeInfo,
                                    PaStreamCallbackFlags statusFlags,
                                    void *userData)
{

    (void) outputBuffer;
    return static_cast<audio_listener*> (userData)->on_listen(static_cast<const float*> (inputBuffer), framesPerBuffer, timeInfo, statusFlags);
}

int audio_listener::on_listen(const float * inputBuffer,
                              unsigned long framesPerBuffer,
                              const PaStreamCallbackTimeInfo* timeInfo,
                              PaStreamCallbackFlags statusFlags)
{
    (void) timeInfo;
    assert(framesPerBuffer == LISTENER_FRAMES_PER_BUFFER);
    if (statusFlags != 0) {
        logger::info("listener status = " + to_string(statusFlags));
    }
    pCache->store_buffer(inputBuffer);
    pAnalyzer->notify();
    return false;
}

void audio_listener::listen_finished_callback(void* userData)
{
    return static_cast<audio_listener*> (userData)->on_listen_finished();
}

void audio_listener::on_listen_finished()
{
    logger::debug("on_listen_finished()");
}

bool audio_listener::is_listening() const
{
    return Pa_IsStreamActive(mStream);
}
