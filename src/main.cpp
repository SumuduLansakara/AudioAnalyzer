#include <iostream>
#include "device_manager/device_manager.h"
#include "player/sine_wave_player.h"

using std::cout;
using std::endl;

int main(int, char**)
{
    device_manager * devMan{device_manager::get_instance()};
    devMan->load_devices(paFloat32);

    auto defaultOutputDevice{devMan->default_output_device()};
    cout << "Default output device" << endl;
    defaultOutputDevice->debug_print();

    sine_wave_player player{2, defaultOutputDevice->default_sample_rate(), 440};
    const unsigned int seconds{5};
    player.setup_stream(devMan->default_output_device());
    player.play();

    cout << "playing for " << seconds << " seconds" << endl;
    Pa_Sleep(seconds * 1000);

    player.stop();
    player.close();
    return 0;
}
