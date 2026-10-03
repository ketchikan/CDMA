#pragma once

#include <vector>

#include "types.hpp"

class Device;  // forward declaration
class Network; // forward declaration

/**
@class Tower

@brief A single cell tower. Keeps track of the devices connected to it; the Network owns the Tower and does the spreading code bookkeeping.
*/
class Tower
{
private:
    TowerID id;
    Network *network;              // Network this tower belongs to
    std::vector<Device *> devices; // Devices connected to this tower (not owned)

public:
    Tower(TowerID id, Network &n) : id(id), network(&n) {}

    TowerID getID() const { return id; }
    const std::vector<Device *> &getDevices() const { return devices; }

    /**
    @fn registerDevice

    @brief Connect a device to this tower, registering it with the Network. Called by Device::connect.

    @return The spreading code assigned to the device
    */
    CodeIdx registerDevice(Device &d);

    /**
    @fn unregisterDevice

    @brief Disconnect a device from this tower, freeing its spreading code on the Network. Called by Device::disconnect.
    */
    void unregisterDevice(Device &d);
};
