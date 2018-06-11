#include <iostream>
#include "device_manager/device_manager.h"
#include "player/sine_wave_player.h"
#include "listener/audio_listener.h"

using std::cout;
using std::endl;

void demo_player()
{
    device * defaultOutputDevice{device_manager::get_instance()->default_output_device()};
    cout << "Default output device" << endl;
    defaultOutputDevice->debug_print();

    sine_wave_player player{2, defaultOutputDevice->default_sample_rate(), paFloat32, 440};
    const unsigned int seconds{5};
    player.setup_stream(defaultOutputDevice);
    player.start();

    cout << "playing for " << seconds << " seconds" << endl;
    Pa_Sleep(seconds * 1000);

    player.stop();
    player.close();
}

void demo_listener()
{
    cout << "start listening..." << endl << std::flush;
    device * defaultInputDevice{device_manager::get_instance()->default_input_device()};
    audio_listener listener{2, defaultInputDevice->default_sample_rate(), paFloat32, 1024};
    listener.setup_stream(defaultInputDevice);
    listener.start();
    Pa_Sleep(5000);
    listener.close();
    cout << "listening finished" << endl;
}

int main(int, char**)
{
    device_manager * devMan{device_manager::get_instance()};
    //devMan->load_all_devices(paFloat32);
    devMan->load_default_devices(paFloat32);

    // demo_player();
    demo_listener();

    return 0;
}
