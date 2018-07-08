#include <iostream>
#include <math.h>
#include <vector>
#include <utility>
#include <exception>
#include <sstream>
#include <cassert>

#include "device_manager.h"
#include "device.h"
#include "utilities/logger.h"

using std::cout;
using std::cerr;
using std::endl;
using std::vector;
using std::runtime_error;

device_manager* device_manager::sInstance{nullptr};

void device_manager::init()
{
    assert(sInstance == nullptr);
    freopen("/dev/null", "w", stderr);
    sInstance = new device_manager();
    freopen("/dev/tty", "w", stderr);
}

device_manager* device_manager::get_instance()
{
    if (not sInstance) {
        throw std::runtime_error("device_manager not initialized!");
    }
    return sInstance;
}

device_manager::device_manager() : mError(Pa_Initialize()), mDeviceCount{Pa_GetDeviceCount()}
{
    logger::debug("device manager initialized with error " + std::to_string(mError));
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
        Pa_Terminate();
        std::stringstream ss;
        ss << "PortAudio error [" << err << "]: ";
        ss << Pa_GetErrorText(err);
        throw runtime_error(ss.str());
    }
}

void device_manager::sleep_millis(long milliSecs) const
{
    Pa_Sleep(milliSecs);
}

vector<device*> device_manager::get_all_devices() const
{
    vector<device*> devices;
    for (int i{0}; i < mDeviceCount; ++i) {
        devices.push_back(make_device(i));
    }
    return devices;
}

device* device_manager::make_device(int deviceId) const
{
    logger::debug("creating audio device for deviceID = " + std::to_string(deviceId));
    if (deviceId < 0) {
        throw runtime_error("invalid device id {}" + std::to_string(deviceId));
    }
    return new device(deviceId);
}
