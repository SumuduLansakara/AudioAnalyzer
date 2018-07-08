#pragma once

#include <mutex>

#include "settings.h"
#include "channel_buffer.h"

class buffer_cache
{
public:

    buffer_cache();

    ~buffer_cache()
    {
    }

    buffer_cache(const buffer_cache& orig) = delete;
    void operator=(const buffer_cache& orig) = delete;

    void store_buffer(const float * inputBuffer);
    float* get_read_address(unsigned int channelID);

    channel_buffer* get_channel_buffer(unsigned int channel);

    void lock();
    void unlock();

private:
    channel_buffer mChannelBuffers[LISTENED_CHANNELS];
    std::mutex mMutex;
};
