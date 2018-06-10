#include "device_manager.h"
#include "generator/sine_generator.h"

int main(int argc [[maybe_unused]], char** argv [[maybe_unused]])
{
    device_manager* devMan = device_manager::get_instance();
    //devMan->debug_print();

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
