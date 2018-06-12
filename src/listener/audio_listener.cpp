#include <iostream>
#include "analyzer/analyzer.h"
#include "audio_listener.h"
#include "device_manager/device_manager.h"

using std::cout;
using std::endl;

audio_listener::audio_listener(unsigned int channels, double sampleRate, int sampleFormat,
                               unsigned long framesPerBuffer) :
audio_stream(channels, sampleRate, sampleFormat), mFramesPerBuffer{framesPerBuffer},
pAnalyzer{nullptr}
{
}

audio_listener::~audio_listener()
{
}

void audio_listener::setup_non_blocking_stream(device* inputDevice, spectrum_analyzer* analyzer)
{
    pAnalyzer = analyzer;
    PaStreamParameters inputParameters{inputDevice->input_parameters()};
    inputParameters.channelCount = mChannels;
    inputParameters.sampleFormat = mSampleFormat;
    inputParameters.suggestedLatency = inputDevice->default_high_input_latency();
    inputParameters.hostApiSpecificStreamInfo = nullptr;

    PaError err{Pa_OpenStream(&mStream, &inputParameters, nullptr, mSampleRate, mFramesPerBuffer, paClipOff,
                              &audio_listener::listen_callback, this)};
    device_manager::get_instance()->check_error(err);

    err = Pa_SetStreamFinishedCallback(mStream, &audio_listener::listen_finished_callback);
    if (err != paNoError) {
        Pa_CloseStream(mStream);
        device_manager::get_instance()->check_error(err);
    }
}

void audio_listener::setup_blocking_stream(device* inputDevice, spectrum_analyzer* analyzer)
{
    pAnalyzer = analyzer;
    PaStreamParameters inputParameters{inputDevice->input_parameters()};
    inputParameters.channelCount = mChannels;
    inputParameters.sampleFormat = mSampleFormat;
    inputParameters.suggestedLatency = inputDevice->default_high_input_latency();
    inputParameters.hostApiSpecificStreamInfo = nullptr;

    PaError err{Pa_OpenStream(&mStream, &inputParameters, nullptr, mSampleRate, mFramesPerBuffer, paClipOff,
                              nullptr, nullptr)};
    device_manager::get_instance()->check_error(err);
}

void audio_listener::start_blocking_listen_loop()
{
    start();
    const unsigned int bufLen = mFramesPerBuffer * mChannels;
    float* buffer = new float[bufLen];
    while (true) {
        Pa_ReadStream(mStream, buffer, mFramesPerBuffer);
        //        on_listen(buffer, mFramesPerBuffer, 0, 0);

        float minimum = 999999;
        float maximum = -999999;
        for (unsigned int i = 0; i < mFramesPerBuffer; ++i) {
            unsigned int realIndex = (i * mChannels) + 0;
            const float sample = buffer[realIndex];
            minimum = std::min(minimum, sample);
            maximum = std::max(maximum, sample);
        }
        cout << "min, max = " << minimum << ", " << maximum << endl;
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
    pAnalyzer->analyze_buffer(inputBuffer, framesPerBuffer, timeInfo, statusFlags);
    return false;
}

void audio_listener::listen_finished_callback(void* userData)
{
    return static_cast<audio_listener*> (userData)->on_listen_finished();
}

void audio_listener::on_listen_finished()
{
    cout << "listen finished" << endl;
}

bool audio_listener::is_listening() const
{
    return Pa_IsStreamActive(mStream);
}
