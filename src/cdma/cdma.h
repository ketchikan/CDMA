#pragma once

class CDMA
{
private:
public:
    void spreadMessage(int &message, int *spreadingCode, int spreadingCodeSize, int *messageSpread)
    {
        for (int i = 0; i < spreadingCodeSize; i++)
        {
            messageSpread[i] = spreadingCode[i] * message;
        }
    }

    int decodeMessage(int &numUsers, int &chipLength, int *messageSpread, int *spreadingCode)
    {
        int result = 0;

        for (int i = 0; i < chipLength; i++)
        {
            result += messageSpread[i] * spreadingCode[i];
        }

        int decoded = result / (numUsers * chipLength); // returns -1 for 0, 1 for 1

        return (decoded == 1) ? 1 : 0;
    }
};