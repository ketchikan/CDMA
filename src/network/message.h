#pragma once

#include <vector>
#include <cstdint>

// TODO Utilize Bitset eventually. The expectation is that any information is converted into binary before being created as a message, which bitset can handle.
// TODO If I want a more comprehensive implementation, I should add a detail for the type that the message takes (bool, char, int, string, etc.) for decoding on the device end.

using Bits = std::vector<bool>;

enum class DataType : uint8_t
{
    INT,
    FLOAT,
    DOUBLE,
    STRING,
    CHAR,
    // TODO add more types as you find and test them
};

/**
@struct Message

@brief A single binary sequence to be sent from one device to another
*/
struct Message
{
    Bits message;
    DataType type;
    size_t byteSize; // mainly useful for types that can vary in size
};