#define MOCK 1

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
#include "utilities/logger.h"
#include "utilities/runtime.h"

using std::cout;
using std::endl;
using std::string;
using std::to_string;

void demo_player(int frequency)
{
    device * defaultOutputDevice{device_manager::get_instance()->default_output_device()};
    defaultOutputDevice->debug_print();

    assert(PLAYER_CHANNELS == 2);
    assert(PLAYER_FRAMES_PER_BUFFER == 1024);
    assert(PLAYER_SAMPLE_RATE == defaultOutputDevice->default_sample_rate());

    sine_wave_player player{frequency};
    player.setup_stream(defaultOutputDevice);
    logger::info("start playing..");
    player.start();
    logger::info("press <ENTER> to exit...");

    std::cin.get();

    player.stop();
    player.close();
    logger::info("playing finished");
}

void demo_non_blocking_listener()
{
    device * defaultInputDevice{device_manager::get_instance()->default_input_device()};

    assert(LISTENER_CHANNELS == 2);
    assert(LISTENER_FRAMES_PER_BUFFER == 1024);
    assert(LISTENER_SAMPLE_RATE == defaultInputDevice->default_sample_rate());
    assert(ANALYZER_FFT_WINDOW_LENGTH == 1024);

    audio_listener listener{};

    spectrum_analyzer analyzer{};
    listener.setup_non_blocking_stream(defaultInputDevice, &analyzer);
    logger::info("start listening..");
    listener.start();
    logger::info("press <ENTER> to exit...");
    std::cin.get();

    listener.close();
    logger::info("listening finished");
}

void print_usage(const string& binary_path)
{
    cout << "Usage:" << endl;
    cout << "\t" << binary_path << " p[lay]|l[isten]" << endl;
}

int main(int argc, char** argv)
{
    char run_mode = '\0';
    int generator_frequency{720};
    if (argc > 1) {
        const string & mode{argv[1]};
        if (mode == "play" || mode == "p") {
            run_mode = 'p';
            if (argc > 2) {
                generator_frequency = atoi(argv[2]);
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

    runtime::init(argv[0]);
    logger::init(runtime::get_instance()->get_path_from_root("bpiaulog.log"), logger::LEVEL_DEBUG);
    logger::debug("instance ID = " + runtime::get_instance()->get_instance_ID());
    device_manager::init();
    switch (run_mode) {
    case 'p':
        logger::info("starting generator [" + to_string(generator_frequency) + " Hz]");
        demo_player(generator_frequency);
        break;
    case 'l':
        logger::info("starting listener");
        demo_non_blocking_listener();
        break;
    }
    return 0;
}
