#pragma once

#include "message.h"
#include <queue>

/**
@class Device

@brief A class to simulate a single 'Device', which can represent a single device on the network. Capable of sending and receiving messages.
*/
class Device
{
private:
    int deviceId;
    /**
    To start off, I am implementing each queue as a collection of bools.
    Eventually we'll decide on a maximum 'frame' size, which is the maximum number of bits that can be sent at once.
    This frame size will be determined based on either the performance of the simulation (we want to maximize the amount of information shared per second) or through current standards.
    Ex: we could settle on 256 bits per frame.
    */
    std::queue<Message> sendQ;
    std::queue<Message> readQ;

public:
    // DEVICE QUEUES
    void createMessage(Message &message);  // Add a message to the sending queue
    void receiveMessage(Message &message); // Add a message to the read queue
};