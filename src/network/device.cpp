#include "Device.hpp"
#include "Tower.hpp"

#include <limits>

void Device::createMessage(std::string &s)
{
    // Convert a message into a series of frames and add them to the sendQ.
    // These frames are defined by a frame header, which reads up to n number of entries from the queue (where n is the maximum number of bytes of the message of a frame, so frame size - header size)

    size_t numChars = s.length();
    size_t numFrames = (numChars + frameMessageSize - 1) / frameMessageSize;
    size_t charIdx = 0;

    for (size_t i = 0; i < numFrames; i++)
    {
        uint8_t numCharsInFrame = static_cast<uint8_t>(
            std::min(frameMessageSize, numChars - charIdx));

        sendQ.push(static_cast<char>(numCharsInFrame));

        for (size_t j = 0; j < numCharsInFrame; j++)
        {
            sendQ.push(s[charIdx++]);
        }

        // Pad remainder of frame with null bytes (no-data sentinel)
        for (size_t j = numCharsInFrame; j < frameMessageSize; j++)
        {
            sendQ.push('\0');
        }
    }
}

void Device::sendFrame()
{
    // Send one frame worth of data to the Tower
    // This is done by taking the characters and spreading the message before sending them to the tower
    // Read one frame of raw bytes from the queue
    std::vector<int> rawFrame(frameSize);
    for (size_t i = 0; i < frameSize; i++)
    {
        if (!sendQ.empty())
        {
            rawFrame[i] = static_cast<uint8_t>(sendQ.front());
            sendQ.pop();
        }
        else
        {
            rawFrame[i] = 0; // pad with no-data
        }
    }

    // CDMA - ENCODE
    size_t chipCount = frameSize * 8 * spreadingCode.size();
    std::vector<int> chips(chipCount);
    cdma.spreadMessage(rawFrame, spreadingCode, chips);

    t.receiveFrame(chips);
}

void Device::receiveFrame()
{
    //
}