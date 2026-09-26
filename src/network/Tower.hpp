#pragma once

#include <vector>

class Device;  // forward declaration
class Network; // forward declaration

// TODO Depending on the use case, we may be able to justify having the SmallVector or SmallArray from LLVM here (if the number of devices is low enough)

/**
@class Tower

@brief Handle a collection of Devices, transmitting messages between them and between other towers to 'distant' users.
*/
class Tower
{
private:
    Network *network;             // Network this tower belongs to
    std::vector<Device *> devices; // Devices connected to this tower (not owned)
    std::vector<int> combinedSignal;
    bool hasSignal = false;

public:
    explicit Tower(Network &n) : network(&n) {}

    /**
    @fn registerDevice

    @brief Connect a device to this tower and register it with the Network. Returns the device's spreading code.
    */
    std::vector<int> registerDevice(Device &d);

    void tickDevices();
    void receiveFrame(std::vector<int> &frame);
    void processTick();
};
