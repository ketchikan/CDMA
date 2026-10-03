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
