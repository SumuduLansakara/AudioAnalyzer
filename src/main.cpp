#include <iostream>
#include <string>
#include "device_manager/device_manager.h"
#include "player/sine_wave_player.h"
#include "listener/audio_listener.h"
#include "analyzer/analyzer.h"

using std::cout;
using std::endl;
using std::string;

void demo_player(double frequency)
{
    device * defaultOutputDevice{device_manager::get_instance()->default_output_device()};
    cout << "Default output device" << endl;
    defaultOutputDevice->debug_print();

    unsigned int channels = 2;
    unsigned long framesPerBuffer = 1024;
    sine_wave_player player{channels, defaultOutputDevice->default_sample_rate(), paFloat32, framesPerBuffer, frequency};
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
    audio_listener listener{channels, defaultInputDevice->default_sample_rate(), paFloat32, framesPerBuffer};

    spectrum_analyzer analyzer{channels, defaultInputDevice->default_sample_rate(), framesPerBuffer, fftWindowLength};
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

void print_usage(const string& binary_path)
{
    cout << "Usage:" << endl;
    cout << "\t" << binary_path << " p[lay]|l[isten]" << endl;
}

int main(int argc, char** argv)
{
    char run_mode = '\0';
    float gen_frequency{440};
    if (argc > 1) {
        const string & mode{argv[1]};
        if (mode == "play" || mode == "p") {
            run_mode = 'p';
            if (argc > 2) {
                gen_frequency = atoi(argv[2]);
            }
        }
        else if (mode == "listen" || mode == "l") {
            run_mode = 'l';
        }
    }
    if (run_mode == '\0') {
        cout << "invalid run mode!" << endl;
        print_usage(argv[0]);
        exit(-1);
    }

    device_manager::get_instance();
    switch (run_mode) {
    case 'p':
        cout << "starting generator [" << gen_frequency << " Hz]" << endl;
        demo_player(gen_frequency);
        break;
    case 'l':
        demo_non_blocking_listener();
        break;
    }
    return 0;
}