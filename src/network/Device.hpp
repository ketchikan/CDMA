#pragma once

#include <cstddef>
#include <queue>
#include <string>
#include <utility>
#include <vector>
#include <optional>

#include "types.hpp"
#include "Message.hpp"
#include "../cdma/cdma.hpp"
#include "../cdma/frame.hpp"

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

    // Frames this device wants to send, waiting for their turn to be spread and transmitted (one per tick)
    std::queue<Frame> outbound;
    // Chip streams received from the tower, waiting for their turn to be decoded with our spreading code (one per tick)
    std::queue<std::vector<int>> inbound;

    uint16_t nextMessageID = 0;
    MessageAssembler assembler;    // Puts received frames back together
    std::vector<Message> received; // Every complete message this device has received, in order of completion

public:
    explicit Device(DeviceID id) : id(id) {}
    ~Device();

    // Towers and the Network hold pointers to a connected Device, so it must stay put
    Device(const Device &) = delete;
    Device &operator=(const Device &) = delete;

    DeviceID getID() const { return id; }
    bool isConnected() const { return tower != nullptr; }
    CodeIdx getCodeIdx() const { return codeIdx; }

    size_t outboundSize() const { return outbound.size(); }
    size_t inboundSize() const { return inbound.size(); }
    bool isIdle() const { return outbound.empty() && inbound.empty(); }

    /**
    @fn queueFrame

    @brief Add a frame to the outbound queue to be sent to the tower.
    */
    void queueFrame(const Frame &f) { outbound.push(f); }

    /**
    @fn sendMessage

    @brief Split a message into frames and add them to the outbound queue, addressed to one recipient (or to each of several; every recipient gets its own copy of the frames, sharing one message ID).

    Throws std::length_error if the message is too long to fit in 255 frames; nothing is queued in that case.
    */
    void sendMessage(DeviceID recipient, const std::string &text);
    void sendMessage(const std::vector<DeviceID> &recipients, const std::string &text);

    /**
    @fn receiveFrame

    @brief Take in one decoded frame addressed to this device. When it completes a message, the message is added to the received list.
    */
    void receiveFrame(const Frame &f);

    /**
    @fn receivedMessages

    @brief Every complete message received so far, in the order they finished arriving.
    */
    const std::vector<Message> &receivedMessages() const { return received; }

    /**
    @fn receiveSignal

    @brief Called by the tower to deliver a received chip stream to the inbound queue.
    */
    void receiveSignal(std::vector<int> chips) { inbound.push(std::move(chips)); }

    /**
    @fn sendFrame

    @brief Drain a single frame from the outbound queue to be sent to the tower.
    */
    std::optional<Frame> sendFrame();

    /**
    @fn transmit

    @brief Take the next frame from the outbound queue and spread it with this device's code. Returns the chips to put on the air, or std::nullopt if there's nothing to send (or the device isn't connected to a tower).
    */
    std::optional<std::vector<int>> transmit();

    /**
    @fn processInbound

    @brief Take one received signal from the inbound queue and despread it with this device's code. A signal with nothing addressed to us decodes to an empty frame and is ignored; a real frame is passed to receiveFrame.
    */
    void processInbound();

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
