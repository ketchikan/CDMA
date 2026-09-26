#include "Tower.hpp"
#include "Device.hpp"
#include "Network.hpp"

#include <cassert>

std::vector<int> Tower::registerDevice(Device &d)
{
    // Add this to the list of devices the tower is controlling
    devices.push_back(&d);

    // Send it along to the network, which hands back the device's spreading code
    return network->registerDevice(d.getID(), *this);
}

void Tower::tickDevices()
{
    // Run all device loops
    for (Device *d : devices)
    {
        d->processTick();
    }
}

void Tower::receiveFrame(std::vector<int> &frame)
{
    // Meant to collect the 'radio waves' we're simulating with CDMA.
    // This method will 'combine' the signals it receives each tick before processing them (to simulate how radio signals will collide while traveling from towers to devices)

    // If there isn't something in the queue, add this and then you're done
    if (!hasSignal)
    {
        combinedSignal = frame;
        hasSignal = true;
        return;
    }

    // Otherwise, grab the current frame in the queue
    assert(frame.size() == combinedSignal.size());
    for (size_t i = 0; i < combinedSignal.size(); i++)
    {
        combinedSignal[i] += frame[i];
    }
}

void Tower::processTick()
{
    if (!hasSignal)
        return;

    // TODO Despread combinedSignal for each registered device...

    // Forward one frame from the sender to receiver
    // TODO This is the first step of making the simulation work. Once this is working, we'll need to do the following:
    // - Unpack the frame that's sitting in your current queue
    // - Anything that is going to another tower should be packed together and sent to that tower
    // - Anything going to one of their connected devices should be packed together and sent to the receivers (they will decode on their end using their spreading code; meant to simulate the collission of info in-air from tower to sender)

    hasSignal = false; // reset for next tick
    combinedSignal.clear();
}