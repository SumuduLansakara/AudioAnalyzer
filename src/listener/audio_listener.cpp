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

void audio_listener::setup_stream(device* inputDevice, spectrum_analyzer* analyzer)
{
    pAnalyzer = analyzer;
    PaStreamParameters inputParameters{inputDevice->input_parameters()};
    inputParameters.channelCount = mChannels;
    inputParameters.sampleFormat = mSampleFormat;
    inputParameters.suggestedLatency = inputDevice->default_low_output_latency();
    inputParameters.hostApiSpecificStreamInfo = nullptr;

    PaError err{Pa_OpenStream(&mStream,
                              &inputParameters,
                              nullptr,
                              mSampleRate,
                              mFramesPerBuffer,
                              paClipOff,
                              &audio_listener::listen_callback,
                              this)};
    device_manager::get_instance()->check_error(err);

    //err = Pa_SetStreamFinishedCallback(mStream, &audio_listener::listen_finished_callback);
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
    return static_cast<audio_listener*> (userData)->on_listen(static_cast<const double*> (inputBuffer),
                                                              framesPerBuffer, timeInfo,
                                                              statusFlags);
}

int audio_listener::on_listen(const double * inputBuffer,
                              unsigned long framesPerBuffer,
                              const PaStreamCallbackTimeInfo* timeInfo,
                              PaStreamCallbackFlags statusFlags)
{
    pAnalyzer->analyze_buffer(inputBuffer, framesPerBuffer, timeInfo, statusFlags);

    //    paTestData *data = (paTestData*) userData;
    //    const SAMPLE *rptr = (const SAMPLE*) inputBuffer;
    //    SAMPLE *wptr = &data->recordedSamples[data->frameIndex * NUM_CHANNELS];
    //    long framesToCalc;
    //    long i;
    //    int finished;
    //    unsigned long framesLeft = data->maxFrameIndex - data->frameIndex;
    //
    //    (void) outputBuffer; /* Prevent unused variable warnings. */
    //    (void) timeInfo;
    //    (void) statusFlags;
    //    (void) userData;
    //
    //    if (framesLeft < framesPerBuffer) {
    //        framesToCalc = framesLeft;
    //        finished = paComplete;
    //    }
    //    else {
    //        framesToCalc = framesPerBuffer;
    //        finished = paContinue;
    //    }
    //
    //    if (inputBuffer == NULL) {
    //        for (i = 0; i < framesToCalc; i++) {
    //            *wptr++ = SAMPLE_SILENCE; /* left */
    //            if (NUM_CHANNELS == 2) *wptr++ = SAMPLE_SILENCE; /* right */
    //        }
    //    }
    //    else {
    //        for (i = 0; i < framesToCalc; i++) {
    //            *wptr++ = *rptr++; /* left */
    //            if (NUM_CHANNELS == 2) *wptr++ = *rptr++; /* right */
    //        }
    //    }
    //    data->frameIndex += framesToCalc;
    //    return finished;
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
