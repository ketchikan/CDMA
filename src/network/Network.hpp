#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

#include "types.hpp"

class Tower; // forward declaration

/**
@class Network

@brief A simulated network used for registering user information, assigning spreading codes to devices, and assisting with transmission from tower to tower.

Public methods:
- addTower: create a tower on this network
- registerDevice / unregisterDevice: attach a device to a tower and hand out (or reclaim) its spreading code
- towerFor: find which tower a device is connected to

A Network must outlive every Tower's Devices, since a Device unregisters itself when it is destroyed.
*/
class Network
{
private:
    struct DeviceRecord
    {
        Tower *tower = nullptr;
        CodeIdx code = NoCode;
    };

    static_assert(WalshSize <= 64, "codesTaken holds one bit per Walsh row");
    static constexpr uint64_t AllCodesTaken = WalshSize == 64 ? ~0ULL : (1ULL << WalshSize) - 1;

    // unique_ptr keeps each Tower at a fixed address, so the Tower& handed out by addTower() stays valid as more towers are added
    std::vector<std::unique_ptr<Tower>> towers;
    std::unordered_map<DeviceID, DeviceRecord> registry;

    // Bit n set = Walsh row n is assigned and in use. Row 0 starts out taken: it's all +1's, which real systems reserve for the pilot channel.
    uint64_t codesTaken = 1;

public:
    Network();
    ~Network(); // Defined in the .cpp, where Tower is a complete type

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

    @brief Called by a Tower when a device connects to it. Records which tower the device is on and assigns it the lowest free spreading code.

    Throws std::invalid_argument if the device is already registered, and std::runtime_error if no spreading codes are free.

    @return The index of the Walsh row assigned to the device
    */
    CodeIdx registerDevice(DeviceID id, Tower &t);

    /**
    @fn unregisterDevice

    @brief Forget a device and free its spreading code so another device can use it.

    Throws std::invalid_argument if the device isn't registered.
    */
    void unregisterDevice(DeviceID id);

    /**
    @fn towerFor

    @brief Look up which tower a device is connected to. Returns nullptr if the device isn't registered.
    */
    Tower *towerFor(DeviceID id) const;
};
