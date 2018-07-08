#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <complex>
#include <limits>
#include <fstream>
#include <ctime>

#include "analyzer.h"
#include "settings.h"
#include "settings.h"
#include "utilities/logger.h"
#include "buffering/buffer_cache.h"

const static unsigned int HISTORY_LENGTH = ANALYZER_HISTORY_WINDOW_COUNT * ceil(LISTENER_FRAMES_PER_BUFFER / ANALYZER_FFT_WINDOW_LENGTH);

using std::cout;
using std::endl;
using std::string;
using std::to_string;

void reset_status(ChannelStatus& status)
{
    status.isSignalLocked = false;
    status.isRecordingAllowed = false;
    status.isBufferFull = false;
    status.logCounter = 0;
    status.lastLockedSignalMax = 0;
    status.lastUnlockedSignalMax = 0;
    status.signalMax = 0;
    status.signalAvg = 0;
    status.signalSTD = 0;
    status.noiseMax = 0;
    status.noiseAvg = 0;
    status.noiseSTD = 0;
}

spectrum_analyzer::spectrum_analyzer(buffer_cache* cache) : mShapingWindow{new float[ANALYZER_FFT_WINDOW_LENGTH]},
mReferenceAmplitude{0}, mDefaultNoiseDB{0}, pAmplitudesDB{new float[ANALYZER_FFT_WINDOW_LENGTH]},
mNextHistoryIndex{HISTORY_LENGTH - 1}, pHistNoiseRMS{new float[HISTORY_LENGTH]},
pHistSignalRMS{new float[HISTORY_LENGTH]}, mStatus{},
mInput{static_cast<float*> (fftwf_malloc(sizeof (float) * ANALYZER_FFT_WINDOW_LENGTH))},
mOutput{static_cast<fftwf_complex*> (fftwf_malloc(sizeof (fftwf_complex) * ANALYZER_FFT_WINDOW_LENGTH))},
mFFTPlan{fftwf_plan_dft_r2c_1d(ANALYZER_FFT_WINDOW_LENGTH, mInput, mOutput, FFTW_ESTIMATE)}, pCache{cache}
{
    for (unsigned int i = 0; i < ANALYZER_FFT_WINDOW_LENGTH; ++i) {
        mShapingWindow[i] = 0.5 * (1 - cos(2 * M_PI * i / (ANALYZER_FFT_WINDOW_LENGTH - 1)));
        mReferenceAmplitude += mShapingWindow[i];
    }
    mReferenceAmplitude /= 2;
    mDefaultNoiseDB = 20 * log10(ANALYZER_DEFAULT_NOISE_DB / mReferenceAmplitude);
    reset_status(mStatus);
}

spectrum_analyzer::~spectrum_analyzer()
{
}

void spectrum_analyzer::notify()
{
    pCache->lock();
    for (unsigned int c : ANALYZED_CHANNELS) {
        analyze_window(c, pCache->get_read_address(c));
    }
    pCache->unlock();
}

void spectrum_analyzer::analyze_window(unsigned int channel, const float* buffer)
{
    (void) channel;
    for (unsigned int i{0}; i < LISTENER_FRAMES_PER_BUFFER; ++i) {
        mInput[i] = buffer[i] * mShapingWindow[i];
    }

    fftwf_execute(mFFTPlan);

    // filter and calculate absolute
    unsigned int peakSignalIndex{0};
    float peakSignalDb{-std::numeric_limits<float>::max()};
    ;
    for (unsigned int i{0}; i < ANALYZER_LOW_CUT_INDEX; i++) {
        pAmplitudesDB[i] = mDefaultNoiseDB;
    }
    for (unsigned int i{ANALYZER_LOW_CUT_INDEX}; i < ANALYZER_FFT_WINDOW_LENGTH - ANALYZER_HIGH_CUT_INDEX; i++) {
        const float amplitude = sqrt(mOutput[i][0] * mOutput[i][0] + mOutput[i][1] * mOutput[i][1]);
        pAmplitudesDB[i] = 20 * log10(amplitude / mReferenceAmplitude);
        if (pAmplitudesDB[i] >= peakSignalDb) {
            peakSignalDb = pAmplitudesDB[i];
            peakSignalIndex = i;
        }
    }
    for (unsigned int i{ANALYZER_FFT_WINDOW_LENGTH - ANALYZER_HIGH_CUT_INDEX}; i < ANALYZER_FFT_WINDOW_LENGTH; i++) {
        pAmplitudesDB[i] = mDefaultNoiseDB;
    }
    if (peakSignalIndex == 0) {
        throw 8;
    }

    const float noiseRMSDbL{get_RMS(ANALYZER_LOW_CUT_INDEX, peakSignalIndex - 1)};
    const float noiseRMSDbR{get_RMS(peakSignalIndex + 1, ANALYZER_HIGH_CUT_INDEX)};
    const float noiseRMSDb{-(noiseRMSDbL + noiseRMSDbR) / 2};

    // calculate history status
    const float histNoiseAvg{get_mean(pHistNoiseRMS, 0, HISTORY_LENGTH)};
    const float histNoiseSTD{get_std(pHistNoiseRMS, histNoiseAvg, 0, HISTORY_LENGTH)};
    const float histSignalAvg{get_mean(pHistSignalRMS, 0, HISTORY_LENGTH)};
    const float histSignalSTD{get_std(pHistSignalRMS, histSignalAvg, 0, HISTORY_LENGTH)};

    mStatus.signalMax = peakSignalDb;
    mStatus.signalAvg = histSignalAvg;
    mStatus.signalSTD = histSignalSTD;
    mStatus.noiseMax = noiseRMSDb;
    mStatus.noiseAvg = histNoiseAvg;
    mStatus.noiseSTD = histNoiseSTD;

    // update history
    mNextHistoryIndex++;
    mNextHistoryIndex %= HISTORY_LENGTH;
    pHistNoiseRMS[mNextHistoryIndex] = noiseRMSDb;
    pHistSignalRMS[mNextHistoryIndex] = peakSignalDb;

    // signal interpretation
    const bool isSignificant = peakSignalDb >MIN_SIGNIFICANT_SIGNAL_DB;
    const bool isGoodSNR = (peakSignalDb - noiseRMSDb) > MIN_SNR_DB;
    const bool isStable = abs(peakSignalDb - histSignalAvg) < MAX_STABLE_SIGNAL_OFFSET_DB;

    if (isSignificant && isGoodSNR && isStable) {
        if (not mStatus.isSignalLocked) {
            mStatus.lastLockedSignalMax = peakSignalDb;
            logger::debug("signal locked: " + to_string(peakSignalDb) + " dBFS [" +
                          to_string(bin_to_freq(peakSignalIndex)) + "-" +
                          to_string(bin_to_freq(peakSignalIndex + 1)) + " Hz]");
            mStatus.isSignalLocked = true;
            if (abs(mStatus.lastUnlockedSignalMax - peakSignalDb > 10)) {
                logger::info("RECORD triggered [lock]");
                save_wav_audio(channel, "_L");
            }
        }
    }
    else if (mStatus.isSignalLocked) {
        mStatus.lastUnlockedSignalMax = peakSignalDb;
        logger::info("signal unlocked");
        mStatus.isSignalLocked = false;
        mStatus.isRecordingAllowed = false;
        if (abs(mStatus.lastLockedSignalMax - peakSignalDb > 10)) {
            logger::info("RECORD triggered [unlock]");
            save_wav_audio(channel, "_U");
        }
    }
    if (mStatus.isBufferFull && pHistNoiseRMS[mNextHistoryIndex] != 0) {
        logger::info("history buffers filled");
        mStatus.isBufferFull = true;
    }
    if (not mStatus.isBufferFull || not mStatus.isSignalLocked) {
        return;
    }
    if (histNoiseSTD < 1.5 && histSignalSTD < 0.02) {
        mStatus.isRecordingAllowed = true;
    }
    if (not mStatus.isRecordingAllowed) {
        return;
    }
    if (histSignalSTD > MAX_HIST_SIGNAL_STD) {
        logger::info("RECORD triggered [S]");
        save_wav_audio(channel, "_S");
    }
    else if (histNoiseSTD > MAX_HIST_NOISE_STD) {
        logger::info("RECORD triggered [Std N]");
        save_wav_audio(channel, "_StdN");
    }
    else if ((abs(histNoiseAvg) - abs(noiseRMSDb)) > 5) {
        logger::info("RECORD triggered [Avg N]");
        save_wav_audio(channel, "_AvgN");
    }
    else {
        return;
    }
    mStatus.isRecordingAllowed = false;
    mStatus.isBufferFull = false;
    mStatus.isSignalLocked = false;
    mStatus.logCounter = 0;

    cout << "!" << endl;
}

float spectrum_analyzer::bin_to_freq(unsigned int bid) const
{
    return bid * ANALYZER_BIN_SIZE;
}

float spectrum_analyzer::get_RMS(unsigned int startIndex, unsigned int endIndex) const
{
    float sum = 0;
    for (unsigned int i = startIndex; i < endIndex; ++i) {
        sum += pAmplitudesDB[i] * pAmplitudesDB[i];
    }
    return sqrt(sum / (endIndex - startIndex));
}

float spectrum_analyzer::get_mean(float* buffer, unsigned int startIndex, unsigned int endIndex) const
{
    float sum = 0;
    for (unsigned int i{startIndex}; i < endIndex; ++i) {
        sum += buffer[i];
    }
    return sum / (endIndex - startIndex);

}

float spectrum_analyzer::get_std(float* buffer, float mean, unsigned int startIndex, unsigned int endIndex) const
{
    float t = 0;
    for (unsigned int i{startIndex}; i < endIndex; ++i) {
        t += (buffer[i] - mean) * (buffer[i] - mean);
    }
    return sqrt(t / (endIndex - startIndex - 1));
}

template <typename Word>
std::ostream& write_word(std::ostream& outs, Word value, unsigned size = sizeof ( Word))
{
    for (; size; --size, value >>= 8)
        outs.put(static_cast<char> (value & 0xFF));
    return outs;
}

void spectrum_analyzer::save_wav_audio(unsigned int channel, const string& tag) const
{
    time_t rawtime;
    time(&rawtime);
    struct tm * timeinfo = localtime(&rawtime);
    char buffer[80];
    strftime(buffer, sizeof (buffer), "%d%m%Y%I%M%S", timeinfo);

    std::ofstream file("output_" + string(buffer) + tag + ".wav", std::ios::binary | std::ios::out);
    if (not file) {
        throw std::runtime_error("failed to open output file!");
    }
    file << "RIFF----WAVEfmt ";
    write_word(file, 16, 4); // no extension data
    write_word(file, 3, 2); // IEEEfloat
    write_word(file, 1, 2); // one channels (stereo file)
    write_word(file, 44100, 4); // samples per second (Hz)
    write_word(file, 176400, 4); // (Sample Rate * BitsPerSample * Channels) / 8
    write_word(file, 4, 2); // data block size (size of two integer samples, one for each channel, in bytes)
    write_word(file, 32, 2); // number of bits per sample (use a multiple of 8)

    size_t data_chunk_pos = file.tellp();
    file << "data----"; // (chunk size to be filled in later)

    channel_buffer* mCyclicBuffer = pCache->get_channel_buffer(channel);
    float * const data{mCyclicBuffer->data()};
    unsigned int bufferIndex{mCyclicBuffer->tail()};
    for (unsigned int i{0}; i < CYCLIC_BUFFER_COUNT; ++i) {
        file.write((char*) &data[bufferIndex * LISTENER_FRAMES_PER_BUFFER], LISTENER_FRAMES_PER_BUFFER * sizeof (float));
        bufferIndex = mCyclicBuffer->next_circuler_buffer_index(bufferIndex);
    }
    size_t file_length = file.tellp();
    file.seekp(data_chunk_pos + 4);
    write_word(file, file_length - data_chunk_pos + 8);

    file.seekp(0 + 4);
    write_word(file, file_length - 8, 4);
    file.close();
}
