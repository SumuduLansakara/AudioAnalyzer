#include <iostream>
#include <math.h>
#include <vector>
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

    for (device& d : get_devices()) {
        cout << endl;
        d.debug_print();
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
mError(Pa_Initialize()), mDeviceCount{Pa_GetDeviceCount()}
{
}

device_manager::~device_manager()
{
    if (mError == paNoError) {
        Pa_Terminate();
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

vector<device> device_manager::get_devices() const
{
    vector<device> devices;
    for (int i{0}; i < mDeviceCount; ++i) {
        devices.push_back(device(i, paFloat32));
    }
    return devices;
}