#pragma once

#include <cstddef>
#include <queue>
#include <vector>

#include "types.hpp"
#include "../cdma/frame.hpp"

class CDMA;    // forward declaration
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

    // Decoded frames waiting to go out on this tower's downlink to one of its devices. Only the radio hop uses chips; everything else (including the Network) works with decoded frames.
    std::queue<Frame> processing;

    // Devices transmit; the tower adds up everything it hears, decodes each connected device's frame, and routes it
    void collectUplink();
    // Spread waiting frames with their destination's code, add them into one composite signal, and broadcast it
    void sendDownlink();
    Device *findDevice(DeviceID id) const;

public:
    Tower(TowerID id, Network &n) : id(id), network(&n) {}

    TowerID getID() const { return id; }
    const std::vector<Device *> &getDevices() const { return devices; }
    size_t processingSize() const { return processing.size(); }

    /**
    @fn cdma

    @brief The network's shared CDMA engine, for devices to spread and despread with.
    */
    const CDMA &cdma() const;

    /**
    @fn isIdle

    @brief True when this tower has nothing waiting and none of its devices have anything queued.
    */
    bool isIdle() const;

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

    /**
    @fn receiveFrame

    @brief Accepts a decoded frame from the Network, to be sent to one of this tower's devices.
    */
    void receiveFrame(const Frame &f);

    /**
    @fn processTick

    @brief Run this tower's share of a single simulation tick. Called by Network::runLoop.
    */
    void processTick();
};
