#include "network/Device.hpp"
#include "network/Network.hpp"

#include <iostream>
// #include <gtest/gtest.h>

// TEST(SanityCheck, BasicMath)
// {
//     EXPECT_EQ(2 + 2, 4);
// }

void runAllTests()
{
    // RUN TESTS
    // TODO Move this into the testing suite, then you can just call on it here
}

int main()
{
    // Testing
    runAllTests();

    // Instantiate the Network
    // Set to 1 tower for now
    Network n;
    Tower &t1 = n.addTower();

    // 1. Create the class
    // 2. For each device, I think the Network adds a new spreading code to a hashmap based on the deviceID (meaning that we can have devices join and leave the network)
    // 3. Each device will, for the sake of this simulation, have a pointer to its spreading code that lives on the network. That way the Towers can do a quick search on the centralized Network for the spreading codes

    // Here's how I think I'll create the simulation:
    // 1. Define the infrastructure, meaning that you can create n number of towers for your network.
    // 2. Devices connect first to towers before registering to the network. So the device does a tower.connect() and then maybe a tower ID? But then the tower notifies the network with something like network.registerDevice(). At that point the network registers the deviceID, it's spreading code, and what tower it's currently connected to.

    // For each device, register them to get their spreading code and then create all their messages
    // TODO I think I should have a separate file or something to define the devices and what towers they connect to. Then I can have one extra file per device as its 'outbox', and then one more file as it's 'inbox' (the received messages)
    // Dummy: register two devices, with the message 'hello' being sent from d1 to d2
    Device d1(1), d2(2);
    d1.connect(t1);
    d2.connect(t1);

    std::string msg = "Hello!";
    d1.createMessage(2, msg);

    // Run the Network loop until all messages have been sent and received
    n.runLoop();

    return 0;
}
