#pragma once

/**
@class CDMA

@brief An implementation of Code Division Multiple Access used in 2G and 3G cellular networks
*/
class CDMA
{
private:
public:
    void spreadMessage(int &message, int *spreadingCode, int spreadingCodeSize, int *messageSpread);

    int decodeMessage(int &numUsers, int &chipLength, int *messageSpread, int *spreadingCode);
};