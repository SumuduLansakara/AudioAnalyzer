#include <iostream>
#include "analyzer/analyzer.h"
#include "mocked_listener.h"
#include "device_manager/device_manager.h"

#define MOCK_INPUT_FREQUENCY 720

using std::cout;
using std::endl;

mocked_listener::mocked_listener(unsigned int channels, double sampleRate, int sampleFormat,
                                 unsigned long framesPerBuffer) :
audio_stream(channels, sampleRate, sampleFormat, framesPerBuffer), pAnalyzer{nullptr},
mPlayer{channels, sampleRate, paFloat32, framesPerBuffer, MOCK_INPUT_FREQUENCY}, pFakeInputBuffer{new float[framesPerBuffer *channels]}
{
}

mocked_listener::~mocked_listener()
{
}

void mocked_listener::setup_non_blocking_stream(device* inputDevice, spectrum_analyzer* analyzer)
{
    pAnalyzer = analyzer;
    PaStreamParameters inputParameters{inputDevice->input_parameters()};
    inputParameters.channelCount = mChannels;
    inputParameters.sampleFormat = mSampleFormat;
    inputParameters.suggestedLatency = inputDevice->default_high_input_latency();
    inputParameters.hostApiSpecificStreamInfo = nullptr;

    PaError err{Pa_OpenStream(&mStream, &inputParameters, nullptr, mSampleRate, mFramesPerBuffer, paClipOff,
                              &mocked_listener::listen_callback, this)};
    device_manager::get_instance()->check_error(err);

    err = Pa_SetStreamFinishedCallback(mStream, &mocked_listener::listen_finished_callback);
    if (err != paNoError) {
        Pa_CloseStream(mStream);
        device_manager::get_instance()->check_error(err);
    }
}

void mocked_listener::setup_blocking_stream(device* inputDevice, spectrum_analyzer* analyzer)
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

void mocked_listener::start_blocking_listen_loop()
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
    mPlayer.on_play(pFakeInputBuffer, framesPerBuffer, timeInfo, statusFlags);
    pAnalyzer->analyze_buffer(pFakeInputBuffer, framesPerBuffer, timeInfo, statusFlags);
    return false;
}

void mocked_listener::listen_finished_callback(void* userData)
{
    return static_cast<mocked_listener*> (userData)->on_listen_finished();
}

void mocked_listener::on_listen_finished()
{
    cout << "listen finished" << endl;
}

bool mocked_listener::is_listening() const
{
    return Pa_IsStreamActive(mStream);
}
