#pragma once

#include <vector>

/**
@class CDMA

@brief An implementation of Code Division Multiple Access used in 2G and 3G cellular networks
*/
class CDMA
{
private:
public:
    void spreadMessage(std::vector<int> &rawFrame, std::vector<int> &spreadingCode, std::vector<int> &chips);

    int decodeMessage(int &numUsers, int &chipLength, int *messageSpread, int *spreadingCode);
};