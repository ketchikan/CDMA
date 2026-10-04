#include "Network.hpp"
#include "Tower.hpp"

#include <stdexcept>

Network::Network() = default;
Network::~Network() = default;

Tower &Network::addTower()
{
    TowerID id = static_cast<TowerID>(towers.size());
    towers.push_back(std::make_unique<Tower>(id, *this));
    return *towers.back();
}

CodeIdx Network::registerDevice(DeviceID id, Tower &t)
{
    if (registry.count(id))
    {
        throw std::invalid_argument("Network: device is already registered");
    }
    if (codesTaken == AllCodesTaken)
    {
        throw std::runtime_error("Network: no spreading codes available");
    }

    // The free rows are the 1 bits of ~codesTaken, so the lowest free row is its count of trailing zeros
    CodeIdx code = __builtin_ctzll(~codesTaken);
    codesTaken |= (1ULL << code);

    registry[id] = {&t, code};
    return code;
}

void Network::unregisterDevice(DeviceID id)
{
    auto it = registry.find(id);
    if (it == registry.end())
    {
        throw std::invalid_argument("Network: device is not registered");
    }

    codesTaken &= ~(1ULL << it->second.code);
    registry.erase(it);
}

Tower *Network::towerFor(DeviceID id) const
{
    auto it = registry.find(id);
    return it == registry.end() ? nullptr : it->second.tower;
}

bool Network::isIdle() const
{
    if (!processing.empty())
        return false;

    for (const auto &t : towers)
    {
        if (!t->isIdle())
            return false;
    }
    return true;
}

void Network::tick()
{
    // Each tower runs its own loop:
    // - Devices transmit a single frame (if there is one in their outbound queue); the tower combines and decodes them
    // - Decoded frames stay on this tower or are forwarded into the Network processing queue
    // - Frames waiting on this tower are spread and broadcast to its devices, which consume a single signal from their inbox
    for (auto &t : towers)
    {
        t->processTick();
    }

    // After all towers have run, drain the Network processing queue and send each frame to the tower serving its destination
    while (!processing.empty())
    {
        Frame f = processing.front();
        processing.pop();

        if (Tower *t = towerFor(f.destination))
        {
            t->receiveFrame(f);
        }
        else
        {
            dropped++;
        }
    }
}

size_t Network::runLoop(size_t maxTicks)
{
    size_t ticks = 0;
    while (!isIdle())
    {
        if (ticks == maxTicks)
        {
            throw std::runtime_error("Network: still busy after the maximum number of ticks");
        }
        tick();
        ticks++;
    }
    return ticks;
}
