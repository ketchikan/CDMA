#include "CDMA.h"

void CDMA::spreadMessage(std::vector<int> &rawFrame, std::vector<int> &spreadingCode, std::vector<int> &chips)
{
    chips.clear();
    chips.reserve(rawFrame.size() * 8 * spreadingCode.size());

    for (int byte : rawFrame)
    {
        for (int b = 7; b >= 0; --b)
        {
            int encoded = ((byte >> b) & 1) ? +1 : -1;

            for (int chip : spreadingCode)
            {
                chips.push_back(encoded * chip);
            }
        }
    }
}

// TODO
// - Decode Header? Decode N Bytes? Something that allows us to just read the header, maybe for recognition of where a message needs to be sent to without decoding the whole message.

int CDMA::decodeMessage(int &numUsers, int &chipLength, int *messageSpread, int *spreadingCode)
{
    int result = 0;

    for (int i = 0; i < chipLength; i++)
    {
        result += messageSpread[i] * spreadingCode[i];
    }

    int decoded = result / (numUsers * chipLength); // returns -1 for 0, 1 for 1

    return (decoded == 1) ? 1 : 0;
}