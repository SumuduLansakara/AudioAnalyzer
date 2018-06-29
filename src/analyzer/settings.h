#pragma once
#include "../settings.h"

// simplifications
#define ANALYZER_BIN_SIZE (LISTENER_SAMPLE_RATE / ANALYZER_FFT_WINDOW_LENGTH)
#define ANALYZER_LOW_CUT_INDEX (ANALYZER_LOW_CUT_FREQ / ANALYZER_BIN_SIZE)
#define ANALYZER_HIGH_CUT_INDEX (ANALYZER_HIGH_CUT_FREQ / ANALYZER_BIN_SIZE)

const static unsigned int CYCLIC_BUFFER_COUNT = 10;
const static unsigned int CYCLIC_ELEMENT_COUNT = LISTENER_SAMPLE_RATE * RECORD_SECONDS;

const static float ANALYZER_DEFAULT_NOISE_DB = 0.00001;

const static int MIN_SIGNIFICANT_SIGNAL_DB = -30;
const static int MIN_SNR_DB = 30;
const static int MAX_STABLE_SIGNAL_OFFSET_DB = 2;

const static float MAX_HIST_SIGNAL_STD = 0.15;
const static float MAX_HIST_NOISE_STD = 3;