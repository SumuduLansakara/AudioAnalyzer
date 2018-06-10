#pragma once

static const int TABLE_LENGTH = 200;

class sine_generator
{
public:
    sine_generator();
    sine_generator(const sine_generator& orig) = delete;
    void operator=(const sine_generator& orig) = delete;
    ~sine_generator();

    bool open(PaDeviceIndex index);
    bool start();
    bool close();
    bool stop();

    void debug_print() const;
private:
    static void paStreamFinished(void* userData);
    void paStreamFinishedMethod();

    int paCallbackMethod(const void *inputBuffer, void *outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags);
    static int paCallback(const void *inputBuffer, void *outputBuffer,
                          unsigned long framesPerBuffer,
                          const PaStreamCallbackTimeInfo* timeInfo,
                          PaStreamCallbackFlags statusFlags,
                          void *userData);

    float mTable[TABLE_LENGTH];

    PaStream *stream;
    int left_phase;
    int right_phase;
    char message[20];
};