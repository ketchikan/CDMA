#include <iostream>
#include <iterator>

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

int main()
{
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
    spreadMessage(messageConverted, spreadingCode, chipLength, messageSpread);

    // Now that we have the message spread, we can decode it
    // Output!
    std::cout << "Recovered: " << decodeMessage(numUsers, chipLength, messageSpread, spreadingCode) << std::endl;
}