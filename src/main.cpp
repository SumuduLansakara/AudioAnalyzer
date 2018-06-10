#include "device_manager.h"
#include "generator/sine_generator.h"

int main(int argc, char** argv)
{
    device_manager* devMan = device_manager::get_instance();
    devMan->debug_print();

    sine_generator gen;
    gen.debug_print();

    return 0;
}
