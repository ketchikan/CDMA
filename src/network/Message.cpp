#include "Message.hpp"

#include <algorithm>
#include <stdexcept>

std::vector<Frame> splitMessage(DeviceID source, DeviceID destination, uint16_t messageID, const std::string &text)
{
    size_t numFrames = std::max<size_t>(1, (text.size() + Frame::MaxPayload - 1) / Frame::MaxPayload);
    if (numFrames > UINT8_MAX)
    {
        throw std::length_error("Message: too long to fit in 255 frames");
    }

    std::vector<Frame> frames(numFrames);
    for (size_t i = 0; i < numFrames; i++)
    {
        Frame &f = frames[i];
        f.source = source;
        f.destination = destination;
        f.messageID = messageID;
        f.fragmentIndex = static_cast<uint8_t>(i);
        f.fragmentCount = static_cast<uint8_t>(numFrames);

        size_t start = i * Frame::MaxPayload;
        size_t len = std::min(Frame::MaxPayload, text.size() - start);
        f.payloadLength = static_cast<uint8_t>(len);
        std::copy_n(text.begin() + start, len, f.payload.begin());
    }
    return frames;
}

std::optional<Message> MessageAssembler::addFrame(const Frame &f)
{
    if (f.fragmentCount == 0 || f.fragmentIndex >= f.fragmentCount)
    {
        throw std::invalid_argument("MessageAssembler: frame has an invalid fragment index/count");
    }

    Partial &p = partials[{f.source, f.messageID}];
    if (p.pieces.empty())
    {
        p.pieces.resize(f.fragmentCount);
        p.arrived.assign(f.fragmentCount, false);
    }
    else if (p.pieces.size() != f.fragmentCount)
    {
        throw std::invalid_argument("MessageAssembler: fragment count disagrees with earlier frames of this message");
    }

    if (p.arrived[f.fragmentIndex])
    {
        return std::nullopt; // duplicate
    }
    p.arrived[f.fragmentIndex] = true;
    p.pieces[f.fragmentIndex].assign(f.payload.begin(), f.payload.begin() + f.payloadLength);
    p.numArrived++;

    if (p.numArrived < p.pieces.size())
    {
        return std::nullopt;
    }

    Message m;
    m.source = f.source;
    m.messageID = f.messageID;
    for (const std::string &piece : p.pieces)
    {
        m.text += piece;
    }
    partials.erase({f.source, f.messageID});
    return m;
}
