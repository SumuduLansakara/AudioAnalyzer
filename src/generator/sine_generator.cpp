#include <cmath>
#include <iostream>
#include <portaudio.h>

#include "sine_generator.h"
#include "settings.h"

using std::cout;
using std::endl;

sine_generator::sine_generator()
{
    for (int i{0}; i < TABLE_LENGTH; i++) {
        mTable[i] = (float) sin(((double) i / (double) TABLE_LENGTH) * M_PI * 2.);
    }
}

sine_generator::~sine_generator()
{
}

void sine_generator::debug_print() const
{
    for (int i{0}; i < TABLE_LENGTH; i++) {
        cout << mTable[i] << " ";
    }
    cout << endl;
}

bool sine_generator::open(PaDeviceIndex index)
{
    PaStreamParameters outputParameters;

    outputParameters.device = index;
    if (outputParameters.device == paNoDevice) {
        return false;
    }

    const PaDeviceInfo* pInfo = Pa_GetDeviceInfo(index);
    if (pInfo != 0) {
        printf("Output device name: '%s'\r", pInfo->name);
    }

    outputParameters.channelCount = 2; /* stereo output */
    outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
    outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    PaError err = Pa_OpenStream(
            &stream,
            NULL, /* no input */
            &outputParameters,
            SAMPLE_RATE,
            paFramesPerBufferUnspecified,
            paClipOff, /* we won't output out of range samples so don't bother clipping them */
            &sine_generator::paCallback,
            this /* Using 'this' for userData so we can cast to Sine* in paCallback method */
            );

    if (err != paNoError) {
        /* Failed to open stream to device !!! */
        return false;
    }

    err = Pa_SetStreamFinishedCallback(stream, &sine_generator::paStreamFinished);

    if (err != paNoError) {
        Pa_CloseStream(stream);
        stream = 0;

        return false;
    }

    return true;
}

bool sine_generator::close()
{
    if (stream == 0)
        return false;

    PaError err = Pa_CloseStream(stream);
    stream = 0;

    return (err == paNoError);
}

bool sine_generator::start()
{
    if (stream == 0)
        return false;

    PaError err = Pa_StartStream(stream);

    return (err == paNoError);
}

bool sine_generator::stop()
{
    if (stream == 0)
        return false;

    PaError err = Pa_StopStream(stream);

    return (err == paNoError);
}

int sine_generator::paCallbackMethod(const void *inputBuffer, void *outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags)
{
    float *out = (float*) outputBuffer;

    // (void) timeInfo; /* Prevent unused variable warnings. */
    (void) statusFlags;
    (void) inputBuffer;

    for (unsigned long i{0}; i < framesPerBuffer; i++) {
        *out++ = mTable[left_phase]; /* left */
        *out++ = mTable[right_phase]; /* right */
        left_phase += 1;
        if (left_phase >= TABLE_LENGTH) left_phase -= TABLE_LENGTH;
        right_phase += 3; /* higher pitch so we can distinguish left and right. */
        if (right_phase >= TABLE_LENGTH) right_phase -= TABLE_LENGTH;
    }

    return paContinue;

}

int sine_generator::paCallback(const void *inputBuffer, void *outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags,
        void *userData)
{
    return ((sine_generator*) userData)->paCallbackMethod(inputBuffer,
            outputBuffer, framesPerBuffer, timeInfo, statusFlags);
}

void sine_generator::paStreamFinishedMethod()
{
    cout << "stream finished" << endl;
}

void sine_generator::paStreamFinished(void* userData)
{
    return ((sine_generator*) userData)->paStreamFinishedMethod();
}
