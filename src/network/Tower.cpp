#include "Tower.hpp"

#include <cassert>

void Tower::receiveFrame(std::vector<int> &frame)
{
    // Meant to collect the 'radio waves' we're simulating with CDMA.
    // This method will 'combine' the signals it receives each tick before processing them (to simulate how radio signals will collide while traveling from towers to devices)

    // If there isn't something in the queue, add this and then you're done
    if (!hasSignal)
    {
        combinedSignal = frame;
        hasSignal = true;
        return;
    }

    // Otherwise, grab the current frame in the queue
    assert(frame.size() == combinedSignal.size());
    for (size_t i = 0; i < combinedSignal.size(); i++)
    {
        combinedSignal[i] += frame[i];
    }
}

void Tower::processTick()
{
    if (!hasSignal)
        return;

    // TODO Despread combinedSignal for each registered device...

    hasSignal = false; // reset for next tick
    combinedSignal.clear();
}