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

device_manager* device_manager::sInstance{nullptr};

device_manager* device_manager::get_instance()
{
    if (sInstance == nullptr) {
        sInstance = new device_manager();
    }
    return sInstance;
}

device_manager::device_manager() : mError(Pa_Initialize()), mDeviceCount{Pa_GetDeviceCount()}
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
        Pa_Terminate();
        std::stringstream ss;
        ss << "PortAudio error [" << err << "]: ";
        ss << Pa_GetErrorText(err);
        throw std::runtime_error(ss.str());
    }
}

void device_manager::sleep_millis(long milliSecs) const
{
    Pa_Sleep(milliSecs);
}

std::vector<device*> device_manager::get_all_devices() const
{
    std::vector<device*> devices;
    for (int i{0}; i < mDeviceCount; ++i) {
        devices.push_back(make_device(i));
    }
    return devices;
}

device* device_manager::make_device(int deviceId) const
{
    if (deviceId < 0) {
        cout << "invalid device id " << deviceId << endl;
        return nullptr;
    }
    return new device(deviceId);
}
