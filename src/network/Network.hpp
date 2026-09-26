#pragma once

#include "Tower.hpp"

#include <memory>
#include <vector>
#include <unordered_map>

using spreadingCode = std::vector<int>;
using deviceID = int;

/**
@struct DeviceRecord

@brief Everything the Network knows about a registered device: its spreading code and the tower it is currently connected to.
*/
struct DeviceRecord
{
    spreadingCode code;
    Tower *tower = nullptr;
};

/**
@class Network

@brief Handle a collection of Towers, as well as handling device registration, spreading codes, and tower-to-tower communication.
*/
class Network
{
private:
    // unique_ptr keeps each Tower at a fixed address, so Tower& / Tower* handed out by addTower() stay valid as more towers are added
    std::vector<std::unique_ptr<Tower>> towers;
    std::unordered_map<deviceID, DeviceRecord> registry;

    spreadingCode assignSpreadingCode(deviceID id); // Creates a single spreading code for a device ID. Eventually I want to keep track of already created but unused spreading codes we can use instead of creating a new one.

public:
    Network() = default;

    // Towers hold a pointer back to their Network, so the Network must not be copied or moved
    Network(const Network &) = delete;
    Network &operator=(const Network &) = delete;

    /**
    @fn addTower

    @brief Create a new Tower on this network and return a reference to it that devices can connect to.
    */
    Tower &addTower();

    /**
    @fn registerDevice

    @brief Called by a Tower when a device connects to it. Records the device and the tower it's on, and returns the device's spreading code.
    */
    spreadingCode registerDevice(deviceID id, Tower &t);

    /**
    @fn towerFor

    @brief Look up which tower a device is connected to (for routing). Returns nullptr if the device isn't registered.
    */
    Tower *towerFor(deviceID id) const;

    void runLoop();
};
