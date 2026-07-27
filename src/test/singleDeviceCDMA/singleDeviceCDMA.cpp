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

// TEST 3: Send a length of 1 byte (8 bits)
void sendByte(Test &t, CDMA &c)
{
    t.testName("Send Byte");
    int numUsers = 1;
    int spreadingCode[1] = {-1};
}

// TEST 4: Send a length of 8 bytes (64 bits)
void send8Byte(Test &t, CDMA &c)
{
    t.testName("Send 8 Bytes");
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
    sendByte(t, c);
    send8Byte(t, c);
}