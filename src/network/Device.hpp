#pragma once

#include "types.hpp"

class Tower; // forward declaration

/**
@class Device

@brief A single device on the network. Connects to a tower of your choosing (rather than discovering one) and is given a spreading code when it does.

A Device disconnects itself when destroyed, so the Network and Tower it's connected to must outlive it.
*/
class Device
{
private:
    DeviceID id;
    Tower *tower = nullptr; // Tower we are connected to (not owned)
    CodeIdx codeIdx = NoCode;

public:
    explicit Device(DeviceID id) : id(id) {}
    ~Device();

    // Towers and the Network hold pointers to a connected Device, so it must stay put
    Device(const Device &) = delete;
    Device &operator=(const Device &) = delete;

    DeviceID getID() const { return id; }
    bool isConnected() const { return tower != nullptr; }
    CodeIdx getCodeIdx() const { return codeIdx; }

    /**
    @fn connect

    @brief Connect to a tower, which registers this device with the Network and hands back its spreading code.

    Throws std::logic_error if already connected. Also propagates the Network's errors (duplicate device ID, no codes left), in which case the device stays disconnected.
    */
    void connect(Tower &t);

    /**
    @fn disconnect

    @brief Disconnect from the current tower, freeing the spreading code. Does nothing if not connected.
    */
    void disconnect();
};
