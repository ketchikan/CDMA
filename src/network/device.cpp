#include "device.h"

#include <string>
#include <vector>
#include <type_traits>

// TODO Instead of sending a terminating character, I can start the message with a preview of the message that they're going to receive.
// This would include things like the size of the data in bytes, so that the device or tower knows how many to collect.

template <typename T>
void Device::createMessage(const T &message)
{
    static_assert(std::is_trivially_copyable_v<T>,
                  "createMessage only works on trivially copyable types");

    const auto *bytes = reinterpret_cast<const unsigned char *>(&message);
    Bits bits;
    bits.reserve(sizeof(T) * 8);

    for (size_t i = 0; i < sizeof(T); ++i)
        for (int b = 7; b >= 0; --b)
            bits.push_back((bytes[i] >> b) & 1);

    Message m;
    m.message = bits;
    m.type = DataType::INT; // TODO I need a way to convert the current type into the correct enum value
    m.byteSize = sizeof(T);

    sendQ.push(m);
}

// Specialization for std::string, which is not trivially copyable
template <>
void Device::createMessage(const std::string &message)
{
    Bits bits;
    for (unsigned char c : message)
        for (int b = 7; b >= 0; --b)
            bits.push_back((c >> b) & 1);

    Message m;
    m.message = bits;
    m.type = DataType::STRING;
    m.byteSize = message.size();

    sendQ.push(m);
}

void Device::receiveMessage(Message &message)
{
    // Add the message to the receive queue
    // TODO When I move on to longer messages, I need a terminating character to be either appended to the message or added to the queue
    readQ.push(message);
};