#pragma once

static const int TABLE_LENGTH = 200;

class sine_generator
{
public:
    sine_generator();
    sine_generator(const sine_generator& orig) = delete;
    void operator=(const sine_generator& orig) = delete;
    ~sine_generator();

    void debug_print() const;
private:
    float mTable[TABLE_LENGTH];
};