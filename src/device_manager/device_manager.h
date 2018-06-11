#pragma once

#include <portaudio.h>
#include "device.h"

class device_manager {
public:
    static device_manager* get_instance();

    device_manager(const device_manager&) = delete;
    void operator=(const device_manager&) = delete;
    ~device_manager();

    void load_default_devices(int sampleFormat);
    void load_all_devices(int sampleFormat);
    void check_error(PaError err);

    void sleep_millis(long milliSecs) const;

    int device_count() const {
        return mDeviceCount;
    }

    device* default_input_device() const {
        return mDefaultInputDevice;
    }

    device* default_output_device() const {
        return mDefaultOutputDevice;
    }

    std::vector<device*> devices() const {
        return mDevices;
    }

    void debug_print() const;

private:
    device_manager();
    device* make_device(int deviceId, int sampleFormat) const;

    PaError mError;
    int mDeviceCount;
    device* mDefaultInputDevice;
    device* mDefaultOutputDevice;
    std::vector<device*> mDevices;

    static device_manager* sInstance;
};
