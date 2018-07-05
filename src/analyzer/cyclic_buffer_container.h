#pragma once

#include "settings.h"

class cyclic_buffer_container
{
public:
    cyclic_buffer_container();
    virtual ~cyclic_buffer_container();
    cyclic_buffer_container(const cyclic_buffer_container& orig) = delete;
    void operator=(const cyclic_buffer_container& orig) = delete;

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