#pragma once

#include <portaudio.h>
#include "device.h"

class device_manager
{
public:
    static device_manager* get_instance();

    device_manager(const device_manager&) = delete;
    void operator=(const device_manager&) = delete;
    ~device_manager();

    std::vector<device*> get_all_devices() const;
    void check_error(PaError err);

    void sleep_millis(long milliSecs) const;

    int device_count() const
    {
        return mDeviceCount;
    }

    device* default_input_device() const
    {
        return make_device(Pa_GetDefaultInputDevice());
    }

    device* default_output_device() const
    {
        return make_device(Pa_GetDefaultOutputDevice());
    }



private:
    device_manager();
    device* make_device(int deviceId) const;

    PaError mError;
    int mDeviceCount;

    static device_manager* sInstance;
};
