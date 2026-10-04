#include "Device.hpp"
#include "Tower.hpp"

#include <stdexcept>

Device::~Device()
{
    disconnect();
}

void Device::connect(Tower &t)
{
    if (tower)
    {
        throw std::logic_error("Device: already connected to a tower");
    }

    codeIdx = t.registerDevice(*this);
    tower = &t;
}

void Device::disconnect()
{
    if (!tower)
        return;

    tower->unregisterDevice(*this);
    tower = nullptr;
    codeIdx = NoCode;
    inbound = {}; // anything still waiting was spread for the old code and can't be decoded any more
}

std::optional<Frame> Device::sendFrame()
{
    if (outbound.empty())
    {
        return std::nullopt;
    }
    Frame f = outbound.front();
    outbound.pop();
    return f;
}

std::optional<std::vector<int>> Device::transmit()
{
    if (!tower)
        return std::nullopt;

    std::optional<Frame> f = sendFrame();
    if (!f)
        return std::nullopt;

    std::vector<int> chips;
    tower->cdma().spreadMessage(f->toBytes(), chips, codeIdx);
    return chips;
}

void Device::processInbound()
{
    if (inbound.empty() || !tower)
        return;

    std::vector<int> chips = std::move(inbound.front());
    inbound.pop();

    Frame f = Frame::fromBytes(tower->cdma().decodeFrame(chips, codeIdx));
    if (f.fragmentCount == 0)
        return; // nothing was addressed to us in this signal

    receiveFrame(f);
}

void Device::sendMessage(DeviceID recipient, const std::string &text)
{
    sendMessage(std::vector<DeviceID>{recipient}, text);
}

void Device::sendMessage(const std::vector<DeviceID> &recipients, const std::string &text)
{
    // Build every frame before queueing any, so a failure leaves the outbound queue untouched
    uint16_t messageID = nextMessageID;
    std::vector<Frame> frames;
    for (DeviceID recipient : recipients)
    {
        std::vector<Frame> forRecipient = splitMessage(id, recipient, messageID, text);
        frames.insert(frames.end(), forRecipient.begin(), forRecipient.end());
    }

    nextMessageID++;
    for (const Frame &f : frames)
    {
        outbound.push(f);
    }
}

void Device::receiveFrame(const Frame &f)
{
    if (std::optional<Message> m = assembler.addFrame(f))
    {
        received.push_back(std::move(*m));
    }
}
