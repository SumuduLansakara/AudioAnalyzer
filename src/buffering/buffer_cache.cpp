#include "buffer_cache.h"
#include "utilities/logger.h"

void buffer_cache::store_buffer(const float* inputBuffer)
{
    for (unsigned int c : ANALYZED_CHANNELS) {
        float* pa{mChannelBuffers[c].push_address()};
        for (unsigned long i{c}; i < LISTENER_FRAMES_PER_BUFFER * LISTENED_CHANNELS; i += LISTENED_CHANNELS) {
            *pa++ = inputBuffer[i];
        }
    }
}

float* buffer_cache::get_read_address(unsigned int channelID)
{
    return mChannelBuffers[channelID].pop_address();
}

channel_buffer* buffer_cache::get_channel_buffer(unsigned int channel)
{
    return &mChannelBuffers[channel];
}
