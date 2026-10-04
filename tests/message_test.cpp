#include <gtest/gtest.h>

#include <algorithm>
#include <optional>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "../src/network/Device.hpp"
#include "../src/network/Message.hpp"

namespace
{
    std::string alphabetText(size_t length)
    {
        std::string s;
        for (size_t i = 0; i < length; i++)
        {
            s += static_cast<char>('a' + i % 26);
        }
        return s;
    }
}

TEST(MessageTest, SplitsLongTextIntoOrderedFrames)
{
    std::string text = alphabetText(1000);
    std::vector<Frame> frames = splitMessage(1, 2, 42, text);

    size_t expectedFrames = (1000 + Frame::MaxPayload - 1) / Frame::MaxPayload;
    ASSERT_EQ(frames.size(), expectedFrames); // ASSERT_* stops the test on failure; the loop below would be meaningless if this were wrong

    for (size_t i = 0; i < frames.size(); i++)
    {
        EXPECT_EQ(frames[i].fragmentIndex, i);
        EXPECT_EQ(frames[i].fragmentCount, expectedFrames);
        EXPECT_EQ(frames[i].messageID, 42);
        EXPECT_EQ(frames[i].destination, 2);
    }
}

TEST(MessageTest, EmptyMessageStillProducesOneFrame)
{
    std::vector<Frame> frames = splitMessage(1, 2, 0, "");

    ASSERT_EQ(frames.size(), 1u);
    EXPECT_EQ(frames[0].payloadLength, 0);
}

TEST(MessageTest, TooLongToFitThrows)
{
    EXPECT_THROW(splitMessage(1, 2, 0, std::string(255 * Frame::MaxPayload + 1, 'x')), std::length_error);
    EXPECT_NO_THROW(splitMessage(1, 2, 0, std::string(255 * Frame::MaxPayload, 'x')));
}

TEST(MessageAssemblerTest, ReassemblesFramesArrivingOutOfOrder)
{
    std::string text = alphabetText(500);
    std::vector<Frame> frames = splitMessage(1, 2, 7, text);
    std::shuffle(frames.begin(), frames.end(), std::mt19937(1)); // fixed seed, so the test is repeatable

    MessageAssembler assembler;
    std::optional<Message> result;
    for (const Frame &f : frames)
    {
        EXPECT_FALSE(result.has_value()); // must not finish early
        result = assembler.addFrame(f);
    }

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->text, text);
    EXPECT_EQ(result->source, 1);
    EXPECT_EQ(result->messageID, 7);
    EXPECT_EQ(assembler.pendingMessages(), 0u);
}

TEST(MessageAssemblerTest, KeepsInterleavedMessagesApart)
{
    std::vector<Frame> a = splitMessage(1, 3, 0, alphabetText(300));
    std::vector<Frame> b = splitMessage(2, 3, 0, std::string(300, 'z')); // same message ID, different sender

    MessageAssembler assembler;
    std::vector<Message> done;
    for (size_t i = 0; i < a.size(); i++)
    {
        for (const Frame *f : {&a[i], &b[i]})
        {
            if (auto m = assembler.addFrame(*f))
            {
                done.push_back(*m);
            }
        }
    }

    ASSERT_EQ(done.size(), 2u);
    EXPECT_EQ(done[0].text, alphabetText(300));
    EXPECT_EQ(done[1].text, std::string(300, 'z'));
}

TEST(MessageAssemblerTest, MalformedFrameThrows)
{
    Frame f = splitMessage(1, 2, 0, "hi")[0];
    f.fragmentIndex = 5; // past the fragment count

    MessageAssembler assembler;
    EXPECT_THROW(assembler.addFrame(f), std::invalid_argument);
}

TEST(DeviceMessageTest, MultipleRecipientsEachGetTheirOwnFrames)
{
    Device sender(1);
    sender.sendMessage({2, 3}, "hi");

    ASSERT_EQ(sender.outboundSize(), 2u);
    std::optional<Frame> first = sender.sendFrame();
    std::optional<Frame> second = sender.sendFrame();
    ASSERT_TRUE(first.has_value()); // an empty optional can't be dereferenced, so stop here if it is
    ASSERT_TRUE(second.has_value());
    EXPECT_EQ(first->destination, 2);
    EXPECT_EQ(second->destination, 3);
    EXPECT_EQ(first->messageID, second->messageID); // one message, one ID
    EXPECT_FALSE(sender.sendFrame().has_value());   // nothing left to send
}

TEST(DeviceMessageTest, ReceivedFramesBecomeAReceivedMessage)
{
    Device sender(1), receiver(2);
    sender.sendMessage(2, alphabetText(200));

    while (std::optional<Frame> f = sender.sendFrame())
    {
        receiver.receiveFrame(*f);
    }

    ASSERT_EQ(receiver.receivedMessages().size(), 1u);
    EXPECT_EQ(receiver.receivedMessages()[0].text, alphabetText(200));
}
