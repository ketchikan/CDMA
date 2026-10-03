#include "Tower.hpp"
#include "Device.hpp"
#include "Network.hpp"

#include <algorithm>

CodeIdx Tower::registerDevice(Device &d)
{
    // The Network may refuse (duplicate device, no codes left), so ask it first and only track the device if it accepts
    CodeIdx code = network->registerDevice(d.getID(), *this);
    devices.push_back(&d);
    return code;
}

void Tower::unregisterDevice(Device &d)
{
    network->unregisterDevice(d.getID());
    devices.erase(std::remove(devices.begin(), devices.end(), &d), devices.end());
}
