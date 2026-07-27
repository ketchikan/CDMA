#include "../test.h"
#include "../../CDMA/CDMA.h"

// Test CDMA implementation with a single device and simple messages

// TEST 1: Send a 0
void sendBit0(Test &t, CDMA &c)
{
    t.testName("Send 0");
    int numUsers = 1;
    int spreadingCode[2] = {-1, 1};
    int message = 0;

    // Convert message // TODO this should probably be part of CDMA
    int messageConverted = message ? 1 : -1;

    int chipLength = sizeof(spreadingCode) / sizeof(spreadingCode[0]);

    // Spread the message across the spectrum
    int messageSpread[sizeof(spreadingCode) / sizeof(spreadingCode[0])] = {};
    c.spreadMessage(messageConverted, spreadingCode, chipLength, messageSpread);

    // Decode the message
    int decode = c.decodeMessage(numUsers, chipLength, messageSpread, spreadingCode);

    if (decode == message)
    {
        t.pass("Message '0' completed.");
        return;
    }
    t.fail("Message '1' did fail to decode correctly.");
}

// TEST 2: Send a 1
void sendBit1(Test &t, CDMA &c)
{
    t.testName("Send 1");
    int numUsers = 1;
    int spreadingCode[2] = {-1, 1};
    int message = 1;

    // Convert message // TODO this should probably be part of CDMA
    int messageConverted = message ? 1 : -1;

    int chipLength = sizeof(spreadingCode) / sizeof(spreadingCode[0]);

    // Spread the message across the spectrum
    int messageSpread[sizeof(spreadingCode) / sizeof(spreadingCode[0])] = {};
    c.spreadMessage(messageConverted, spreadingCode, chipLength, messageSpread);

    // Decode the message
    int decode = c.decodeMessage(numUsers, chipLength, messageSpread, spreadingCode);

    if (decode == message)
    {
        t.pass("Message '1' completed.");
        return;
    }
    t.fail("Message '1' did fail to decode correctly.");
}

// TEST 3: Send a uint8_t
void sendUINT8_T(Test &t, CDMA &c)
{
    t.testName("Send uint8_t");
    int numUsers = 1;
    int spreadingCode[1] = {-1};
}

// TEST 4: Send a uint16_t
void sendUINT16_T(Test &t, CDMA &c)
{
    t.testName("Send uint16_t");
    int numUsers = 1;
    int spreadingCode[1] = {-1};
}

// TEST 5: Send a uint32_t
void sendUINT32_T(Test &t, CDMA &c)
{
    t.testName("Send uint32_t");
    int numUsers = 1;
    int spreadingCode[1] = {-1};
}

// TEST 6: Send a uint64_t
void sendUINT64_T(Test &t, CDMA &c)
{
    t.testName("Send uint64_t");
    int numUsers = 1;
    int spreadingCode[1] = {-1};
}

// TEST 7: Send a String
void sendString(Test &t, CDMA &c)
{
    t.testName("Send std::string");
    int numUsers = 1;
    int spreadingCode[1] = {-1};
}

// RUN TESTS
void runSingleDeviceCDMATests()
{
    Test t;
    CDMA c;
    sendBit0(t, c);
    sendBit1(t, c);
    sendUINT8_T(t, c);
    sendUINT16_T(t, c);
    sendUINT32_T(t, c);
    sendUINT64_T(t, c);
    sendString(t, c);
}