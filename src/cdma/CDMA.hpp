#pragma once

#include <cstddef>
#include <vector>
#include "frame.hpp"
#include "../Network/types.hpp"

/**
@class CDMA

@brief An implementation of Code Division Multiple Access used in 2G and 3G cellular networks
*/
class CDMA
{
private:
    int walshMatrix[WalshSize][WalshSize];

    void checkCodeIdx(int spreadingCodeIdx) const;

    // Correlate one bit's worth of chips (WalshSize of them) with a Walsh row; true if the bit was a 1
    bool despreadBit(const int *chips, int spreadingCodeIdx) const;

public:
    CDMA();
    ~CDMA() = default;

    // Don't let CDMA be copied or moved
    CDMA(const CDMA &) = delete;
    CDMA &operator=(const CDMA &) = delete;

    /**
    Throws std::invalid_argument if spreadingCodeIdx isn't a valid Walsh row.
    */
    void spreadMessage(const Frame::Bytes &rawFrame, std::vector<int> &chips, int spreadingCodeIdx) const;

    /**
    @fn decodeFrame

    @brief Recover a frame's raw bytes from a received chip stream by correlating with one Walsh code. Other users' signals added into the stream cancel out, so the receiver doesn't need to know how many there are.

    Throws std::invalid_argument if spreadingCodeIdx isn't a valid Walsh row, or if chips isn't exactly one frame's worth (Frame::FrameSize * 8 * WalshSize).
    */
    Frame::Bytes decodeFrame(const std::vector<int> &chips, int spreadingCodeIdx) const;
};