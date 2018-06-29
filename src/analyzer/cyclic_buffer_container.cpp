#include "cyclic_buffer_container.h"
#include <stdexcept>

cyclic_buffer_container::cyclic_buffer_container() :
mHead{0}, mTail{0}, mBufferCount{0}, pData{new float[ELEMENT_COUNT]}
{
}

cyclic_buffer_container::~cyclic_buffer_container()
{
    delete [] pData;
}

unsigned int cyclic_buffer_container::next_circuler_buffer_index(unsigned int i) const
{
    if (++i >= BUFFER_COUNT) {
        i = 0;
    }
    return i;
}

float* cyclic_buffer_container::push(float* buffer)
{
    mBufferCount++;
    if (mBufferCount >= BUFFER_COUNT) {
        throw std::logic_error("failed pushing to buffer! buffer overflow");
    }
    const unsigned int startIndex{mHead * BUFFER_LENGTH};
    for (unsigned int i{0}; i < BUFFER_LENGTH; i++) {
        pData[startIndex + i] = buffer[i];
    }
    mHead = next_circuler_buffer_index(mHead);
    return &pData[startIndex];
}

float* cyclic_buffer_container::pop()
{
    if (mBufferCount == 0) {
        throw std::logic_error("failed popping from buffer! buffer empty");
    }
    mBufferCount--;
    const unsigned int temp{mTail};
    mTail = next_circuler_buffer_index(mTail);
    return &pData[temp * BUFFER_LENGTH];
}
