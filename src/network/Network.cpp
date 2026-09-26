#include "Network.hpp"

Tower &Network::addTower()
{
    towers.push_back(std::make_unique<Tower>(*this));
    return *towers.back();
}

spreadingCode Network::assignSpreadingCode(deviceID id)
{
    // TODO
    return {};
}

spreadingCode Network::registerDevice(deviceID id, Tower &t)
{
    spreadingCode code = assignSpreadingCode(id);
    registry[id] = {code, &t};
    return code;
}

Tower *Network::towerFor(deviceID id) const
{
    auto it = registry.find(id);
    return it == registry.end() ? nullptr : it->second.tower;
}

void Network::runLoop()
{
    // Each network tick runs in phases so that the order towers are iterated in doesn't change the results

    // Phase 1: every device on every tower sends one frame
    for (auto &t : towers)
    {
        t->tickDevices();
    }

    // Phase 2: every tower despreads the combined signal it received
    for (auto &t : towers)
    {
        t->processTick();
    }

    // TODO Phase 3: towers forward traffic bound for other towers' devices
    // TODO Phase 4: towers spread and send traffic down to their own devices
}
