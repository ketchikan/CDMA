#include "device.h"

void Device::createMessage(Message &message)
{
    // Add the message to the send queue
    // TODO When I move on to longer messages, I need a terminating character to be either appended to the message or added to the queue
    // TODO Is this the function where I need to handle converting message to bits?
    sendQ.push(message);
};

void Device::receiveMessage(Message &message)
{
    // Add the message to the receive queue
    // TODO When I move on to longer messages, I need a terminating character to be either appended to the message or added to the queue
    readQ.push(message);
};