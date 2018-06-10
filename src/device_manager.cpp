#include <iostream>
#include <math.h>
#include <vector>
#include <utility>
#include <exception>
#include <sstream>

#include "device_manager.h"
#include "device.h"

using std::cout;
using std::cerr;
using std::endl;
using std::vector;

void device_manager::debug_print() const
{
    cout << "******************************" << endl;
    cout << "PortAudio v." << Pa_GetVersion() << endl;
    cout << "[" << Pa_GetVersionInfo()->versionText << "]" << endl;
    cout << "Device count: " << mDeviceCount << endl;

    for (device* d : mDevices) {
        cout << endl;
        d->debug_print();
    }
    cout << "******************************" << endl;
}

device_manager* device_manager::sInstance = nullptr;

device_manager* device_manager::get_instance()
{
    if (sInstance == nullptr) {
        sInstance = new device_manager();
    }
    return sInstance;
}

device_manager::device_manager() :
mError(Pa_Initialize()), mDeviceCount{Pa_GetDeviceCount()},
mDefaultInputDevice{nullptr}, mDefaultOutputDevice{nullptr}, mDevices{}
{
    check_error(mError);
}

device_manager::~device_manager()
{
    if (sInstance != nullptr) {
        Pa_Terminate();
        sInstance = nullptr;
    }
}

void device_manager::check_error(PaError err)
{
    if (err != paNoError) {
        cerr << "PortAudio error [" << err << "]! "
                << Pa_GetErrorText(err) << endl;
        Pa_Terminate();
        std::stringstream ss;
        ss << "PortAudio error [" << err << "]: ";
        ss << Pa_GetErrorText(err);
        throw std::runtime_error(ss.str());
    }
}

void device_manager::load_devices(int sampleFormat)
{
    const int defaultInputDeviceId = Pa_GetDefaultInputDevice();
    const int defaultOutputDeviceId = Pa_GetDefaultOutputDevice();
    for (int i{0}; i < mDeviceCount; ++i) {
        device* dev = new device(i, sampleFormat);
        mDevices.push_back(dev);
        if (i == defaultInputDeviceId) {
            mDefaultInputDevice = dev;
        }
        if (i == defaultOutputDeviceId) {
            mDefaultOutputDevice = dev;
        }
    }
}