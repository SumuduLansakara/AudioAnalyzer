#include <iostream>
#include "device_manager.h"
#include "generator/sine_generator.h"

using std::cout;
using std::endl;

int main(int, char**)
{
    device_manager* devMan = device_manager::get_instance();
    devMan->load_devices(paFloat32);

    auto defaultOutputDevice = devMan->default_output_device();
    cout << "Default output device" << endl;
    defaultOutputDevice->debug_print();

    sine_generator generator{2, 440};
    generator.debug_print();

    const unsigned int seconds = 5;
    generator.setup_stream(devMan->default_output_device());
    generator.start();

    cout << "playing for " << seconds << " seconds" << endl;
    Pa_Sleep(seconds * 1000);

    generator.stop();
    generator.close();
    return 0;
}
