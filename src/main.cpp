#include "core/System.h"
#include "utils/Logger.h"
#include <iostream>

int main()
{
    try
    {
        System system;
        system.run();  // This starts your CLI menu

    }
    catch (const std::exception& e)
    {
        std::cerr << "[FATAL] " << e.what() << "\n";
        return 1;
    }

    return 0;
}