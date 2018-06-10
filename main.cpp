#include <iostream>
#include <portaudio.h>

#include "device.h"

using std::cout;
using std::cerr;
using std::endl;

void terminate(int err)
{
    Pa_Terminate();
    exit(err);
}

void check_error(PaError err)
{
    if (err != paNoError) {
        cerr << "PortAudio error [" << err << "]! "
                << Pa_GetErrorText(err) << endl;
        terminate(err);
    }
}

void print_portaudio_version_info()
{
    cout << "PortAudio v." << Pa_GetVersion() << " [" << Pa_GetVersionInfo()->versionText << "]" << endl;
}

int main(int argc, char** argv)
{
    PaError err;
    err = Pa_Initialize();
    check_error(err);

    print_portaudio_version_info();

    int n = Pa_GetDeviceCount();
    if (n < 0) {
        cerr << "No device found!" << endl;
        terminate(-1);
    }

    for (int i = 0; i < n; i++) {
        Device d(i, paFloat32);
        d.debug_print();
        cout << endl;
    }

    return 0;
}
