#pragma once
#include <vector>
#include <queue>

#include "../CDMA/CDMA.h"
#include "device.h"

/**
@class Tower

@brief Handle a collection of Devices, transmitting messages between them and between other towers to 'distant' users.
*/
class Tower
{
private:
    std::vector<Device> devices;
    std::queue<bool> q;

public:
};