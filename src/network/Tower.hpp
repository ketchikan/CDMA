#pragma once

#include <vector>
#include "Device.hpp"

/**
@class Tower

@brief Handle a collection of Devices, transmitting messages between them and between other towers to 'distant' users.
*/
class Tower
{
private:
    std::vector<Device> devices;
    std::vector<int> combinedSignal;
    bool hasSignal = false;

public:
    void receiveFrame(std::vector<int> &frame);
    void processTick();
};