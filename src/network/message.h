#pragma once

// TODO Utilize Bitset eventually. The expectation is that any information is converted into binary before being created as a message, which bitset can handle.
// TODO If I want a more comprehensive implementation, I should add a detail for the type that the message takes (bool, char, int, string, etc.) for decoding on the device end.

/**
@struct Message

@brief A single binary sequence to be sent from one device to another
*/
struct Message
{
    bool msg;
    int userId;
};