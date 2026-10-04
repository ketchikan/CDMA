#include <gtest/gtest.h>

#include <stdexcept>
#include <vector>

#include "../src/cdma/cdma.hpp"
#include "../src/cdma/frame.hpp"

namespace
{
    Frame makeFrame(const std::string &text)
    {
        Frame f;
        f.source = 1;
        f.destination = 2;
        f.messageID = 9;
        f.payloadLength = static_cast<uint8_t>(text.size());
        std::copy(text.begin(), text.end(), f.payload.begin());
        return f;
    }
}

// TEST(SuiteName, TestName) defines one independent test. Related tests share a suite name.
TEST(FrameTest, RoundTripsThroughBytes)
{
    Frame f = makeFrame("hello");
    f.fragmentIndex = 2;
    f.fragmentCount = 7;

    Frame g = Frame::fromBytes(f.toBytes());

    EXPECT_EQ(g.source, f.source);
    EXPECT_EQ(g.destination, f.destination);
    EXPECT_EQ(g.messageID, f.messageID);
    EXPECT_EQ(g.fragmentIndex, 2);
    EXPECT_EQ(g.fragmentCount, 7);
    EXPECT_EQ(g.payloadLength, 5);
    EXPECT_EQ(g.payload, f.payload);
}

TEST(CdmaTest, SingleUserRoundTrip)
{
    CDMA cdma;
    Frame::Bytes raw = makeFrame("hello").toBytes();

    std::vector<int> chips;
    cdma.spreadMessage(raw, chips, 3);

    EXPECT_EQ(chips.size(), Frame::FrameSize * 8 * WalshSize);
    EXPECT_EQ(cdma.decodeFrame(chips, 3), raw);
}

TEST(CdmaTest, TwoUsersOnTheSameSignalDecodeIndependently)
{
    CDMA cdma;
    Frame::Bytes rawA = makeFrame("from A").toBytes();
    Frame::Bytes rawB = makeFrame("from B, which is different").toBytes();

    std::vector<int> chipsA, chipsB;
    cdma.spreadMessage(rawA, chipsA, 3);
    cdma.spreadMessage(rawB, chipsB, 7);

    // The "air": both signals added together
    std::vector<int> combined(chipsA.size());
    for (size_t i = 0; i < combined.size(); i++)
    {
        combined[i] = chipsA[i] + chipsB[i];
    }

    EXPECT_EQ(cdma.decodeFrame(combined, 3), rawA);
    EXPECT_EQ(cdma.decodeFrame(combined, 7), rawB);
}

TEST(CdmaTest, InvalidInputThrows)
{
    CDMA cdma;
    std::vector<int> chips;
    Frame::Bytes raw = makeFrame("x").toBytes();

    // EXPECT_THROW(statement, ExceptionType) passes only if the statement throws exactly that type
    EXPECT_THROW(cdma.spreadMessage(raw, chips, -1), std::invalid_argument);
    EXPECT_THROW(cdma.spreadMessage(raw, chips, WalshSize), std::invalid_argument);

    cdma.spreadMessage(raw, chips, 1);
    EXPECT_THROW(cdma.decodeFrame(chips, WalshSize), std::invalid_argument);

    chips.pop_back(); // no longer exactly one frame's worth of chips
    EXPECT_THROW(cdma.decodeFrame(chips, 1), std::invalid_argument);
}
