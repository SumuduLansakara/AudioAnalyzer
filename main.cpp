#include "device_manager.h"

int main(int argc, char** argv)
{
    device_manager* devMan = device_manager::get_instance();
    devMan->debug_print();

    return 0;
}
