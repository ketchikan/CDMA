#include "CDMA.hpp"

#include <stdexcept>

CDMA::CDMA()
{
    // INITIALIZATION
    // Create the Hadamard matrix for Walsh Codes using Sylvester's Construction
    // For IS-95, N = 64
    // Begin with the first index
    walshMatrix[0][0] = 1;
    for (int size = 1; size < WalshSize; size *= 2)
    {
        // Copy the finished size×size block in the top-left into the other three quadrants
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                walshMatrix[i][j + size] = walshMatrix[i][j];         // Top right
                walshMatrix[i + size][j] = walshMatrix[i][j];         // Bottom left
                walshMatrix[i + size][j + size] = -walshMatrix[i][j]; // Bottom right
            }
        }
    }
}

void CDMA::checkCodeIdx(int spreadingCodeIdx) const
{
    if (spreadingCodeIdx < 0 || spreadingCodeIdx >= WalshSize)
    {
        throw std::invalid_argument("CDMA: spreading code index out of range");
    }
}

void CDMA::spreadMessage(const Frame::Bytes &rawFrame, std::vector<int> &chips, int spreadingCodeIdx)
{
    checkCodeIdx(spreadingCodeIdx);

    // Resetting the chips vector to be able to hold what we're going to encode
    chips.clear();
    chips.reserve(rawFrame.size() * 8 * WalshSize);

    // Iterate byte by byte
    for (int byte : rawFrame)
    {
        for (int b = 7; b >= 0; --b)
        {
            int encoded = ((byte >> b) & 1) ? +1 : -1;

            for (int chip : walshMatrix[spreadingCodeIdx])
            {
                chips.push_back(encoded * chip);
            }
        }
    }
}

bool CDMA::despreadBit(const int *chips, int spreadingCodeIdx) const
{
    int correlation = 0;
    for (int i = 0; i < WalshSize; i++)
    {
        correlation += chips[i] * walshMatrix[spreadingCodeIdx][i];
    }

    // With orthogonal codes our own bit contributes +/-WalshSize and every other user contributes 0, so only the sign matters
    return correlation > 0;
}

Frame::Bytes CDMA::decodeFrame(const std::vector<int> &chips, int spreadingCodeIdx) const
{
    checkCodeIdx(spreadingCodeIdx);

    if (chips.size() != Frame::FrameSize * 8 * WalshSize)
    {
        throw std::invalid_argument("CDMA: chip stream is not exactly one frame long");
    }

    Frame::Bytes out{};
    for (size_t byte = 0; byte < out.size(); byte++)
    {
        uint8_t value = 0;
        for (size_t bit = 0; bit < 8; bit++)
        {
            // Bits were spread most significant first
            value = static_cast<uint8_t>((value << 1) | despreadBit(&chips[(byte * 8 + bit) * WalshSize], spreadingCodeIdx));
        }
        out[byte] = value;
    }
    return out;
}
