#pragma once

const static unsigned int BUFFER_LENGTH = 1024;
const static unsigned int BUFFER_COUNT = 10;
const static unsigned int ELEMENT_COUNT = BUFFER_LENGTH * BUFFER_COUNT;

class cyclic_buffer_container
{
public:
    cyclic_buffer_container();
    virtual ~cyclic_buffer_container();
    cyclic_buffer_container(const cyclic_buffer_container& orig) = delete;
    void operator=(const cyclic_buffer_container& orig) = delete;

    float* push(float* buffer);
    float* pop();

    inline bool is_full() const
    {
        return mBufferCount == BUFFER_COUNT;
    }

    inline unsigned int get_buffer_count() const
    {
        return mBufferCount;
    }

    float* data()
    {
        return pData;
    }

private:
    inline unsigned int next_circuler_buffer_index(unsigned int i) const;

    unsigned int mHead;
    unsigned int mTail;
    unsigned int mBufferCount;

    float* pData;
};