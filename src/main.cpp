#include <gtest/gtest.h>

#include <iostream>
#include <string>

#include "network/Device.hpp"
#include "network/Message.hpp"
#include "network/Network.hpp"
#include "network/Tower.hpp"

// -----------------------------------------------------------------------
// Usage:
//   ./CDMA            run every test, then the demo simulation below
//   ./CDMA --tests    run the tests only (gtest flags such as --gtest_filter=NetworkTest.* work too)
//   ./CDMA --demo     run the demo only
// -----------------------------------------------------------------------

static void printUsage()
{
    std::cout << "Usage: CDMA [--tests] [--demo]\n"
              << "  --tests   run the correctness tests only\n"
              << "  --demo    run the demo simulation only\n"
              << "  (no flags run both; --gtest_* flags are passed to Google Test)\n";
}

static void printInbox(const Device &d)
{
    std::cout << "  Device " << d.getID() << " received " << d.receivedMessages().size() << " message(s)\n";
    for (const Message &m : d.receivedMessages())
    {
        std::string preview = m.text.size() > 60 ? m.text.substr(0, 57) + "..." : m.text;
        std::cout << "    from device " << m.source << " (" << m.text.size() << " chars): \"" << preview << "\"\n";
    }
}

/**
A worked example of building your own simulation. Copy this function, change the towers, devices and messages, and run it.
*/
static void runDemo()
{
    // 1. Create the network, then add as many towers as you like. The
    // network is declared first so it outlives the devices, which
    // unregister themselves from it when they are destroyed.
    Network network;
    Tower &downtown = network.addTower();
    Tower &uptown = network.addTower();

    // 2. Create devices and connect each one to the tower you choose
    // (there is no tower discovery in this simulation). Connecting
    // registers the device with the network, which hands it a free Walsh
    // code. A network has
    // NOTE: WalshSize - 1 = 63 codes, so connecting a 64th device at once throws; disconnect() frees a code for reuse.
    Device d1(1), d2(2), d3(3);
    d1.connect(downtown);
    d2.connect(downtown);
    d3.connect(uptown);

    std::cout << "Network layout:\n";
    for (const Device *d : {&d1, &d2, &d3})
    {
        std::cout << "  Device " << d->getID() << " -> tower " << network.towerFor(d->getID())->getID()
                  << ", Walsh code " << d->getCodeIdx() << "\n";
    }

    // 3. Queue up messages. A message goes to one device, or to a list
    // of devices. Anything longer than one frame (Frame::MaxPayload
    // bytes) is split into several frames automatically.
    // Nothing is sent until the network runs.
    d1.sendMessage(2, "Hi device 2, we're on the same tower.");
    d1.sendMessage({2, 3}, "Hello to both of you.");
    d2.sendMessage(3, "Hey device 3, I'm on a different tower from you.");
    d3.sendMessage(1, std::string("This reply is long enough to need several frames. ") + std::string(150, '.'));
    d3.sendMessage(99, "Anyone there? (device 99 doesn't exist)");

    // 4. Run until nothing is left in flight. Each tick, every device
    // sends at most one frame, towers combine and separate the
    // overlapping signals using the Walsh codes, and frames cross
    // between towers.
    // (network.tick() advances a single tick if you want to inspect the state in between.)
    size_t ticks = network.runLoop();

    // 5. Look at what arrived.
    std::cout << "\nFinished after " << ticks << " ticks. Frames dropped: " << network.droppedFrames() << "\n";
    for (const Device *d : {&d1, &d2, &d3})
    {
        printInbox(*d);
    }

    // Things to try:
    //  - Move d2 to the uptown tower and see which messages still arrive.
    //  - Add a hundred devices with d.connect() in a loop and catch the std::runtime_error when the codes run out.
    //  - Send the same long message from every device at once and watch how ticks grows.
    // TODO Load devices, towers and messages from a text file, with one inbox/outbox file per device.
}

int main(int argc, char **argv)
{
    // Google Test removes the flags it recognizes (--gtest_filter, ...) from argv; whatever is left is ours
    testing::InitGoogleTest(&argc, argv);

    bool wantTests = false;
    bool wantDemo = false;
    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];
        if (arg == "--tests")
        {
            wantTests = true;
        }
        else if (arg == "--demo")
        {
            wantDemo = true;
        }
        else
        {
            printUsage();
            return arg == "--help" || arg == "-h" ? 0 : 2;
        }
    }
    if (!wantTests && !wantDemo)
    {
        wantTests = wantDemo = true;
    }

    if (wantTests)
    {
        std::cout << "=== Running tests ===\n";
        if (RUN_ALL_TESTS() != 0)
        {
            std::cout << "\nTests failed, so the demo was not run.\n";
            return 1;
        }
    }

    if (wantDemo)
    {
        std::cout << "\n=== Demo simulation ===\n";
        runDemo();
    }

    return 0;
}
