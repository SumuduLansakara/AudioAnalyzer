#pragma once

struct ChannelStatus
{
    bool isSignalLocked;
    bool isRecordingAllowed;
    bool isBufferFull;

    int logCounter;

    float lastLockedSignalMax;
    float lastUnlockedSignalMax;

    float signalMax;
    float signalAvg;
    float signalSTD;

    float noiseMax;
    float noiseAvg;
    float noiseSTD;
};