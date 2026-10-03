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
}
