#include <iostream>
#include <iterator>

int main()
{
    // Set the number of users
    int numUsers = 1;

    // Set a simple spreading code for our single user
    int spreadingCode[2] = {-1, 1};

    // Create the initial message
    int message = 1;

    int messageConverted = message ? 1 : -1;

    std::cout << messageConverted << std::endl;

    // Spread the message across the spectrum
    int spreadMessage[sizeof(spreadingCode) / sizeof(spreadingCode[0])] = {};
    for (int i = 0; i < sizeof(spreadingCode) / sizeof(spreadingCode[0]); i++)
    {
        spreadMessage[i] = spreadingCode[i] * messageConverted;
        std::cout << spreadMessage[i] << " ";
    }
    std::cout << "\n";

    // Now that we have the message spread, we can decode it
    int resultMessage = 0;

    for (int i = 0; i < sizeof(spreadingCode) / sizeof(spreadingCode[0]); i++)
    {
        resultMessage += spreadMessage[i] * spreadingCode[i];
    }

    int chipLength = sizeof(spreadingCode) / sizeof(spreadingCode[0]);
    int decoded = resultMessage / (numUsers * chipLength); // gives -1 or +1

    // Output!
    int recoveredMessage = (decoded == 1) ? 1 : 0;
    std::cout << "Recovered: " << recoveredMessage << std::endl;
}