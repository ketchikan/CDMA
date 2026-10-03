#pragma once

#include <cstdint>

// Set the WalshSize to 64
constexpr int WalshSize = 64;

// IDs are 16 bits to match the source/destination fields in Frame
using DeviceID = uint16_t;
using TowerID = uint16_t;

// A spreading code is identified by its row in the Walsh matrix (see CDMA)
using CodeIdx = int;
constexpr CodeIdx NoCode = -1;
