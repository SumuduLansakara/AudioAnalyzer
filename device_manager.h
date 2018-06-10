#pragma once

#include <portaudio.h>
#include "device.h"

class device_manager
{
public:
    static device_manager* get_instance();

    ~device_manager();

    void check_error(PaError err);
    void terminate(int err);

    std::vector<device> get_devices() const;

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
