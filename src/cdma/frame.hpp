#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

/**
@struct Frame

@brief The unit of data sent between devices. Every frame is exactly FrameSize bytes on the wire: a fixed-size header followed by up to MaxPayload bytes of message data (the rest is padding).

Wire layout (multi-byte fields are big-endian):
    [0..1]  source device ID
    [2..3]  destination device ID
    [4..5]  message ID       (identifies which message this frame belongs to)
    [6]     fragment index   (position of this frame within the message, starting at 0)
    [7]     fragment count   (total number of frames in the message)
    [8]     payload length   (how many of the payload bytes are real data)
    [9..]   payload
*/
struct Frame
{
    static constexpr size_t FrameSize = 64;
    static constexpr size_t HeaderSize = 9;
    static constexpr size_t MaxPayload = FrameSize - HeaderSize; // 55

    static_assert(MaxPayload <= UINT8_MAX, "payload length must fit in its one-byte header field");

    using Bytes = std::array<uint8_t, FrameSize>;

    uint16_t source = 0;
    uint16_t destination = 0;
    uint16_t messageID = 0;
    uint8_t fragmentIndex = 0;
    uint8_t fragmentCount = 1;
    uint8_t payloadLength = 0;
    std::array<uint8_t, MaxPayload> payload{}; // zero-padded past payloadLength

    /**
    @fn toBytes

    @brief Serialize to the raw bytes that CDMA spreads. Written field by field rather than by copying the struct, so padding and the machine's endianness never leak into the wire format.
    */
    Bytes toBytes() const
    {
        Bytes out{};
        out[0] = source >> 8;
        out[1] = source & 0xFF;
        out[2] = destination >> 8;
        out[3] = destination & 0xFF;
        out[4] = messageID >> 8;
        out[5] = messageID & 0xFF;
        out[6] = fragmentIndex;
        out[7] = fragmentCount;
        out[8] = payloadLength;
        for (size_t i = 0; i < MaxPayload; i++)
        {
            out[HeaderSize + i] = payload[i];
        }
        return out;
    }

    /**
    @fn fromBytes

    @brief Rebuild a Frame from the raw bytes CDMA decoded. A corrupt length is clamped to MaxPayload so a bad frame can't make a caller read past the payload.
    */
    static Frame fromBytes(const Bytes &in)
    {
        Frame f;
        f.source = static_cast<uint16_t>((in[0] << 8) | in[1]);
        f.destination = static_cast<uint16_t>((in[2] << 8) | in[3]);
        f.messageID = static_cast<uint16_t>((in[4] << 8) | in[5]);
        f.fragmentIndex = in[6];
        f.fragmentCount = in[7];
        f.payloadLength = in[8] > MaxPayload ? static_cast<uint8_t>(MaxPayload) : in[8];
        for (size_t i = 0; i < MaxPayload; i++)
        {
            f.payload[i] = in[HeaderSize + i];
        }
        return f;
    }
};
