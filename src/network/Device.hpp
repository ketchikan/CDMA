#pragma once

#include <queue>
#include <string>
#include <vector>

#include "../CDMA/CDMA.hpp"

class Tower; // forward declaration

// TODO: In the future, I think there's an argument to be made that I should be sending one byte at a time rather than one bit at a time. Maybe I need to handle the spreading code on each bit first, and then the tower can combine into bytes?
// - This is because towers can modulate their signals into patterns (like low to high, high to low, high to mid, etc.) that basically encodes a string of bits, I think enough for 1 or 2 bytes (I'll need to check). I'm not sure how I would translate that here or decode it on the devices, but worth looking into for better efficiency.

/**
@class Device

@brief A class to simulate a single 'Device', which can represent a single device on the network. Capable of sending and receiving messages.
*/
class Device
{
private:
    Tower t;   // Tower we are connected to
    CDMA cdma; // The CDMA

    // TODO these are hardcoded and probably should be moved out so everything in the network knows these sizes
    // frameMessageSize must fit in a uint8_t since the header is one byte
    const size_t frameSize = 256;
    const size_t headerSize = 1;
    const size_t frameMessageSize = frameSize - headerSize; // 255 bytes max per frame
    // static_assert(frameMessageSize <= std::numeric_limits<uint8_t>::max(),
    //               "frameMessageSize must fit in header byte");

    /**
    In the simplest case, each queue should be handling a collection of integers. Each tick, we will grab N integers, where N is the frame size in bits.
    */
    std::queue<char> sendQ;
    std::queue<int> readQ;

    std::vector<int> spreadingCode;

public:
    /**
    @fn createMessage

    @brief Add a message the current device to another device to the send queue

    @param std::string
    */
    void createMessage(std::string &s);

    /**
    @fn sendFrame

    @brief Send a single frame from this device to the connected tower

    @param std::string
    */
    void sendFrame();

    /**
    @fn createMessage

    @brief Receive a single frame from the connected tower to this device's read queue

    @param std::string
    */
    void receiveFrame();
};