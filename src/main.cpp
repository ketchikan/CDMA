// #include "CDMA/CDMA.h"
// #include "network/network.h"
#include "test/singleDeviceCDMA/singleDeviceCDMA.h"

#include <iostream>

void runAllTests()
{
    // RUN TESTS
    runSingleDeviceCDMATests();
}

int main()
{
    // Testing
    runAllTests();

    // Instantiate the Network
    // 1. Create the class
    // 2. For each device, I think the Network adds a new spreading code to a hashmap based on the deviceID (meaning that we can have devices join and leave the network)
    // 3. Each device will, for the sake of this simulation, have a pointer to its spreading code that lives on the network. That way the Towers can do a quick search on the centralized Network for the spreading codes

    // Run the Network loop

    return 0;
}
