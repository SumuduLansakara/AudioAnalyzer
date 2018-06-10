#include <cmath>
#include <iostream>

#include "sine_generator.h"

using std::cout;
using std::endl;

sine_generator::sine_generator()
{
    for (int i{0}; i < TABLE_LENGTH; i++) {
        mTable[i] = (float) sin(((double) i / (double) TABLE_LENGTH) * M_PI * 2.);
    }
}

sine_generator::~sine_generator()
{
}

void sine_generator::debug_print() const
{
    for (int i{0}; i < TABLE_LENGTH; i++) {
        cout << mTable[i] << " ";
    }
    cout << endl;
}
