#include <iostream>
#include "device_manager/device_manager.h"
#include "player/sine_wave_player.h"
#include "listener/audio_listener.h"
#include "analyzer/analyzer.h"

using std::cout;
using std::endl;

void demo_player()
{
    device * defaultOutputDevice{device_manager::get_instance()->default_output_device()};
    cout << "Default output device" << endl;
    defaultOutputDevice->debug_print();

    sine_wave_player player{2, defaultOutputDevice->default_sample_rate(), paFloat32, 440};
    player.setup_stream(defaultOutputDevice);
    player.start();

    cout << "playing... press Enter to exit" << endl;
    std::cin.get();

    player.stop();
    player.close();
}

void demo_non_blocking_listener()
{
    cout << "start listening..." << endl << std::flush;
    device * defaultInputDevice{device_manager::get_instance()->default_input_device()};

    unsigned int channels = 2;
    unsigned long framesPerBuffer = 1024;
    unsigned int fftWindowLength = 1024;
    spectrum_analyzer analyzer{channels, defaultInputDevice->default_sample_rate(), framesPerBuffer, fftWindowLength};
    audio_listener listener{channels, defaultInputDevice->default_sample_rate(), paFloat32, framesPerBuffer};

    listener.setup_non_blocking_stream(defaultInputDevice, &analyzer);
    listener.start();
    cout << "press Enter to exit..." << endl;
    std::cin.get();

    listener.close();
    cout << "listening finished" << endl;
}

void demo_blocking_listener()
{
    cout << "start listening..." << endl << std::flush;
    device * defaultInputDevice{device_manager::get_instance()->default_input_device()};

    unsigned int channels = 2;
    unsigned long framesPerBuffer = 1024;
    unsigned int fftWindowLength = 1024;
    spectrum_analyzer analyzer{channels, defaultInputDevice->default_sample_rate(), framesPerBuffer, fftWindowLength};
    audio_listener listener{channels, defaultInputDevice->default_sample_rate(), paFloat32, framesPerBuffer};

    listener.setup_blocking_stream(defaultInputDevice, &analyzer);
    cout << "entering blocking listen loop" << endl;
    listener.start_blocking_listen_loop();
}

int main(int, char**)
{
    device_manager * devMan{device_manager::get_instance()};
    //devMan->load_all_devices(paFloat32);
    devMan->load_default_devices(paFloat32);

    // demo_player();
    demo_non_blocking_listener();

    return 0;
}