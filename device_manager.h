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

    void check_error(PaError err);

    std::vector<device*> get_devices() const;

    int device_count() const
    {
        return mDeviceCount;
    }

    void debug_print() const;

private:
    device_manager();

    PaError mError;
    int mDeviceCount;

    static device_manager* sInstance;
};
