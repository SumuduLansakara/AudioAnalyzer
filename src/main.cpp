#include <iostream>
#include "device_manager.h"
#include "generator/sine_generator.h"

using std::cout;
using std::endl;

int main(int, char**)
{
    device_manager* devMan = device_manager::get_instance();
    devMan->load_devices(paFloat32);
    //devMan->debug_print();
    auto defaultOutputDevice = devMan->default_output_device();
    cout << "Default output device" << endl;
    defaultOutputDevice->debug_print();

    sine_generator generator;
    generator.debug_print();

    const unsigned int seconds = 5;
    if (generator.open(Pa_GetDefaultOutputDevice())) {
        if (generator.start()) {
            printf("Play for %u seconds.\n", seconds);
            Pa_Sleep(seconds * 1000);

            generator.stop();
        }

        generator.close();
    }
    return 0;
}
