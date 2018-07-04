#define MOCK 0

#if MOCK
#include "mocked_listener/mocked_listener.h"
#define audio_listener mocked_listener
#else
#include "listener/audio_listener.h"
#endif

#include <iostream>
#include <string>
#include <cassert>
#include "device_manager/device_manager.h"
#include "player/sine_wave_player.h"
#include "analyzer/analyzer.h"
#include "settings.h"

using std::cout;
using std::endl;
using std::string;

void demo_player()
{
    device * defaultOutputDevice{device_manager::get_instance()->default_output_device()};
    cout << "Default output device" << endl;
    defaultOutputDevice->debug_print();

    assert(PLAYER_CHANNELS == 2);
    assert(PLAYER_FRAMES_PER_BUFFER == 1024);
    assert(PLAYER_SAMPLE_RATE == defaultOutputDevice->default_sample_rate());

    sine_wave_player player{};
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

    assert(LISTENER_CHANNELS == 2);
    assert(LISTENER_FRAMES_PER_BUFFER == 1024);
    assert(LISTENER_SAMPLE_RATE == defaultInputDevice->default_sample_rate());
    assert(ANALYZER_FFT_WINDOW_LENGTH == 1024);

    audio_listener listener{};

    spectrum_analyzer analyzer{};
    listener.setup_non_blocking_stream(defaultInputDevice, &analyzer);
    listener.start();
    cout << "press Enter to exit..." << endl;
    std::cin.get();

    listener.close();
    cout << "listening finished" << endl;
}

void print_usage(const string& binary_path)
{
    cout << "Usage:" << endl;
    cout << "\t" << binary_path << " p[lay]|l[isten]" << endl;
}

int main(int argc, char** argv)
{
    char run_mode = '\0';
    if (argc > 1) {
        const string & mode{argv[1]};
        if (mode == "play" || mode == "p") {
            run_mode = 'p';
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
        cout << "starting generator [" << GENERATOR_SINE_WAVE_FREQUENCY << " Hz]" << endl;
        demo_player();
        break;
    case 'l':
        demo_non_blocking_listener();
        break;
    }
    return 0;
}
