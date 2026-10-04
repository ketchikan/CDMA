#include "Tower.hpp"
#include "Device.hpp"
#include "Network.hpp"

#include <algorithm>
#include <cassert>
#include <optional>
#include <set>

CodeIdx Tower::registerDevice(Device &d)
{
    // The Network may refuse (duplicate device, no codes left), so ask it first and only track the device if it accepts
    CodeIdx code = network->registerDevice(d.getID(), *this);
    devices.push_back(&d);
    return code;
}

void Tower::unregisterDevice(Device &d)
{
    network->unregisterDevice(d.getID());
    devices.erase(std::remove(devices.begin(), devices.end(), &d), devices.end());
}

const CDMA &Tower::cdma() const
{
    return network->cdma();
}

bool Tower::isIdle() const
{
    if (!processing.empty())
        return false;

    return std::all_of(devices.begin(), devices.end(), [](const Device *d)
                       { return d->isIdle(); });
}

Device *Tower::findDevice(DeviceID id) const
{
    for (Device *d : devices)
    {
        if (d->getID() == id)
            return d;
    }
    return nullptr;
}

void Tower::receiveFrame(const Frame &f)
{
    processing.push(f);
}

void Tower::collectUplink()
{
    // The 'air': every transmission this tick added together, the way radio signals collide in real life
    std::vector<int> airwaves;
    for (Device *d : devices)
    {
        std::optional<std::vector<int>> chips = d->transmit();
        if (!chips)
            continue;

        if (airwaves.empty())
        {
            airwaves = std::move(*chips);
            continue;
        }

        assert(chips->size() == airwaves.size());
        for (size_t i = 0; i < airwaves.size(); i++)
        {
            airwaves[i] += (*chips)[i];
        }
    }

    if (airwaves.empty())
        return;

    // Despread once per connected device, each with its own code. Everyone else's signal cancels out.
    for (Device *d : devices)
    {
        Frame f = Frame::fromBytes(network->cdma().decodeFrame(airwaves, d->getCodeIdx()));
        if (f.fragmentCount == 0)
            continue; // a real frame always has at least one fragment, so this device sent nothing

        // Decoding the header is how the tower learns where to send it
        if (network->towerFor(f.destination) == this)
        {
            processing.push(f);
        }
        else
        {
            network->forward(f);
        }
    }
}

void Tower::sendDownlink()
{
    if (processing.empty())
        return;

    // Each device is addressed by its code, so two frames for the same device in one tick would add together and corrupt each other.
    // Send at most one per device per tick and hold the rest, in order, for the next tick.
    std::vector<int> composite;
    std::set<DeviceID> served;
    std::queue<Frame> deferred;

    while (!processing.empty())
    {
        Frame f = processing.front();
        processing.pop();

        Device *d = findDevice(f.destination);
        if (!d)
        {
            // The device left this tower since the frame was queued; let the Network re-route it (or drop it if it's gone entirely)
            network->forward(f);
            continue;
        }
        if (!served.insert(f.destination).second)
        {
            deferred.push(f);
            continue;
        }

        std::vector<int> chips;
        network->cdma().spreadMessage(f.toBytes(), chips, d->getCodeIdx());

        if (composite.empty())
        {
            composite = std::move(chips);
            continue;
        }
        for (size_t i = 0; i < composite.size(); i++)
        {
            composite[i] += chips[i];
        }
    }
    processing = std::move(deferred);

    // Everyone on the tower receives the same composite signal; each device pulls out its own frame (or nothing) with its code
    if (!composite.empty())
    {
        for (Device *d : devices)
        {
            d->receiveSignal(composite);
        }
    }
}

void Tower::processTick()
{
    collectUplink();
    sendDownlink();

    // Each connected device consumes a single signal from its inbound queue (if there is one)
    for (Device *d : devices)
    {
        d->processInbound();
    }
}
