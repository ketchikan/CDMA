#include "cdma/cdma.h"

#include <iostream>

int main()
{
    CDMA cdma;
    // Set the number of users
    int numUsers = 1;

    // Set a simple spreading code for our single user
    int spreadingCode[2] = {-1, 1};

    // Create the initial message
    int message = 1;

    int messageConverted = message ? 1 : -1;

    int chipLength = sizeof(spreadingCode) / sizeof(spreadingCode[0]);

    // Spread the message across the spectrum
    int messageSpread[sizeof(spreadingCode) / sizeof(spreadingCode[0])] = {};
    cdma.spreadMessage(messageConverted, spreadingCode, chipLength, messageSpread);

    // Now that we have the message spread, we can decode it
    // Output!
    std::cout << "Recovered: " << cdma.decodeMessage(numUsers, chipLength, messageSpread, spreadingCode) << std::endl;
}