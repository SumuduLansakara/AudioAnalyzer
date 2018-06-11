#include <stdexcept>
#include "audio_stream.h"
#include "device_manager/device_manager.h"

using std::runtime_error;

audio_stream::audio_stream(unsigned int channels, double sampleRate, int sampleFormat) :
mChannels{channels}, mSampleRate{sampleRate}, mSampleFormat{sampleFormat}, mStream{nullptr}
{
}

audio_stream::~audio_stream()
{
}

bool audio_stream::is_active() const
{
    return Pa_IsStreamActive(mStream);
}

void audio_stream::start()
{
    if (mStream == nullptr) {
        throw runtime_error("stream start failed! stream is closed");
    }
    device_manager::get_instance()->check_error(Pa_StartStream(mStream));
}

void audio_stream::stop()
{
    if (mStream == nullptr) {
        throw runtime_error("stream stop failed! stream is closed");
    }
    device_manager::get_instance()->check_error(Pa_StopStream(mStream));
}

void audio_stream::close()
{
    if (mStream == nullptr) {
        throw runtime_error("stream close failed! stream is already closed");
    }

    PaError err{Pa_CloseStream(mStream)};
    mStream = nullptr;
    device_manager::get_instance()->check_error(err);
}

