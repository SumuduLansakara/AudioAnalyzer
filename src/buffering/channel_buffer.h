#pragma once

#include "settings.h"

static const unsigned int CYCLIC_BUFFER_COUNT = LISTENER_SAMPLE_RATE * RECORD_SECONDS / LISTENER_FRAMES_PER_BUFFER;

class channel_buffer
{
public:
    channel_buffer();
    virtual ~channel_buffer();
    channel_buffer(const channel_buffer& orig) = delete;
    void operator=(const channel_buffer& orig) = delete;

    float* push(float* buffer);
    float* push_address();
    float* pop_address();

    unsigned int next_circuler_buffer_index(unsigned int i) const;

    inline bool is_full() const
    {
        return mBufferCount == CYCLIC_BUFFER_COUNT;
    }

    inline unsigned int get_buffer_count() const
    {
        return mBufferCount;
    }

    unsigned int tail() const
    {
        return mTail;
    }

    float * data() const
    {
        return pData;
    }

private:
    unsigned int mHead;
    unsigned int mTail;
    unsigned int mBufferCount;

    float* pData;
};