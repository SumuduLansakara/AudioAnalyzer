#include "buffer_cache.h"
#include "utilities/logger.h"

#include<iostream>

using std::mutex;
using std::unique_lock;

buffer_cache::buffer_cache() : mMutex{}
{
}

void buffer_cache::store_buffer(const float* inputBuffer)
{
    unique_lock<mutex> l{mMutex};
    for (unsigned int c : ANALYZED_CHANNELS) {
        float* pa{mChannelBuffers[c].push_address()};
        for (unsigned long i{c}; i < LISTENER_FRAMES_PER_BUFFER * LISTENED_CHANNELS; i += LISTENED_CHANNELS) {
            *pa++ = inputBuffer[i];
        }
    }
}

void buffer_cache::lock()
{
    mMutex.lock();
}

void buffer_cache::unlock()
{
    mMutex.unlock();
}

float* buffer_cache::get_read_address(unsigned int channelID)
{
    return mChannelBuffers[channelID].pop_address();
}

channel_buffer* buffer_cache::get_channel_buffer(unsigned int channel)
{
    return &mChannelBuffers[channel];
}
