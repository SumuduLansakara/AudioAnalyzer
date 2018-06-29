#include "cyclic_buffer_container.h"
#include <stdexcept>

cyclic_buffer_container::cyclic_buffer_container() :
mHead{0}, mTail{0}, mBufferCount{0}, pData{new float[CYCLIC_ELEMENT_COUNT]}
{
}

cyclic_buffer_container::~cyclic_buffer_container()
{
    delete [] pData;
}

unsigned int cyclic_buffer_container::next_circuler_buffer_index(unsigned int i) const
{
    if (++i >= CYCLIC_BUFFER_COUNT) {
        i = 0;
    }
    return i;
}

float* cyclic_buffer_container::push(float* buffer)
{
    mBufferCount++;
    if (mBufferCount >= CYCLIC_BUFFER_COUNT) {
        throw std::logic_error("failed pushing to buffer! buffer overflow");
    }
    const unsigned int startIndex{mHead * LISTENER_FRAMES_PER_BUFFER};
    for (unsigned int i{0}; i < LISTENER_FRAMES_PER_BUFFER; i++) {
        pData[startIndex + i] = buffer[i];
    }
    mHead = next_circuler_buffer_index(mHead);
    return &pData[startIndex];
}

float* cyclic_buffer_container::push_address()
{
    if (mBufferCount == CYCLIC_BUFFER_COUNT - 1) {
        throw std::logic_error("failed getting push address! buffer overflow");
    }
    mBufferCount++;
    const unsigned int startIndex{mHead * LISTENER_FRAMES_PER_BUFFER};
    mHead = next_circuler_buffer_index(mHead);
    return &pData[startIndex];
}

float* cyclic_buffer_container::pop_address()
{
    if (mBufferCount == 0) {
        throw std::logic_error("failed getting pop address! buffer empty");
    }
    mBufferCount--;
    const unsigned int temp{mTail};
    mTail = next_circuler_buffer_index(mTail);
    return &pData[temp * LISTENER_FRAMES_PER_BUFFER];
}
