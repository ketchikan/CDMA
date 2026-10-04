#pragma once

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "types.hpp"
#include "../cdma/frame.hpp"

/**
@struct Message

@brief A complete message as the receiver sees it, after all of its frames have arrived and been put back together.
*/
struct Message
{
    DeviceID source = 0;
    uint16_t messageID = 0;
    std::string text;

    bool operator==(const Message &o) const { return source == o.source && messageID == o.messageID && text == o.text; }
};

/**
@fn splitMessage

@brief Break a message into the frames that carry it to a single destination. Every frame shares the message ID and fragment count, and carries its position in the fragment index. An empty message still produces one (empty) frame.

Throws std::length_error if the message needs more than 255 frames (the fragment count is one byte).
*/
std::vector<Frame> splitMessage(DeviceID source, DeviceID destination, uint16_t messageID, const std::string &text);

/**
@class MessageAssembler

@brief Collects the frames arriving at a device and puts them back together into messages. Frames can arrive in any order, and frames from different messages (different source or message ID) can be interleaved.
*/
class MessageAssembler
{
private:
    struct Partial
    {
        std::vector<std::string> pieces; // indexed by fragment index
        std::vector<bool> arrived;
        size_t numArrived = 0;
    };

    std::map<std::pair<DeviceID, uint16_t>, Partial> partials; // keyed by (source, message ID)

public:
    /**
    @fn addFrame

    @brief Take in one frame. Returns the finished message if this was the last missing frame, otherwise std::nullopt. A frame that has already arrived is ignored.

    Throws std::invalid_argument for a malformed frame (zero fragment count, an index past the count, or a count that disagrees with earlier frames of the same message).
    */
    std::optional<Message> addFrame(const Frame &f);

    /**
    @fn pendingMessages

    @brief How many messages are partway through being received.
    */
    size_t pendingMessages() const { return partials.size(); }
};
