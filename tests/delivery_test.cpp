#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "network/Device.hpp"
#include "network/Message.hpp"
#include "network/Network.hpp"
#include "network/Tower.hpp"

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

// Two towers on one network. Tests that don't need the second tower just ignore it.
class DeliveryTest : public ::testing::Test
{
protected:
    Network net;
    Tower &t1 = net.addTower();
    Tower &t2 = net.addTower();
};

TEST_F(DeliveryTest, IdleNetworkHasNothingToDo)
{
    Device a(1);
    a.connect(t1);

    EXPECT_TRUE(net.isIdle());
    EXPECT_EQ(net.runLoop(), 0u);
}

TEST_F(DeliveryTest, MessageOnTheSameTowerIsDelivered)
{
    Device a(1), b(2);
    a.connect(t1);
    b.connect(t1);

    a.sendMessage(2, "Hello!");
    net.runLoop();

    std::vector<Message> expected = {Message{1, 0, "Hello!"}};
    EXPECT_EQ(b.receivedMessages(), expected);
    EXPECT_TRUE(a.receivedMessages().empty());
}

TEST_F(DeliveryTest, MessageBetweenTowersIsDelivered)
{
    Device a(1), b(2);
    a.connect(t1);
    b.connect(t2);

    a.sendMessage(2, "across towers");
    net.runLoop();

    ASSERT_EQ(b.receivedMessages().size(), 1u);
    EXPECT_EQ(b.receivedMessages()[0].text, "across towers");
    EXPECT_EQ(b.receivedMessages()[0].source, 1);
}

TEST_F(DeliveryTest, LongMessageSpanningManyFramesArrivesIntact)
{
    Device a(1), b(2);
    a.connect(t1);
    b.connect(t2);

    std::string text = alphabetText(2000);
    a.sendMessage(2, text);
    size_t ticks = net.runLoop();

    ASSERT_EQ(b.receivedMessages().size(), 1u);
    EXPECT_EQ(b.receivedMessages()[0].text, text);
    EXPECT_GE(ticks, (2000 + Frame::MaxPayload - 1) / Frame::MaxPayload); // one frame per tick, so it can't be faster than this
}

TEST_F(DeliveryTest, SimultaneousSendersDoNotInterfere)
{
    // Both transmit on the same tick, so their signals collide on the uplink and the tower has to separate them
    Device a(1), b(2), c(3), d(4);
    for (Device *dev : {&a, &b, &c, &d})
    {
        dev->connect(t1);
    }

    a.sendMessage(3, "from a to c");
    b.sendMessage(4, "from b to d");
    net.runLoop();

    ASSERT_EQ(c.receivedMessages().size(), 1u);
    ASSERT_EQ(d.receivedMessages().size(), 1u);
    EXPECT_EQ(c.receivedMessages()[0].text, "from a to c");
    EXPECT_EQ(d.receivedMessages()[0].text, "from b to d");
    EXPECT_TRUE(a.receivedMessages().empty());
    EXPECT_TRUE(b.receivedMessages().empty());
}

TEST_F(DeliveryTest, TwoSendersToOneReceiverBothArrive)
{
    // Two frames for the same device can't share a downlink tick, so the tower has to queue one
    Device a(1), b(2), c(3);
    a.connect(t1);
    b.connect(t2);
    c.connect(t1);

    a.sendMessage(3, alphabetText(300));
    b.sendMessage(3, std::string(300, 'z'));
    net.runLoop();

    ASSERT_EQ(c.receivedMessages().size(), 2u);
    std::vector<std::string> texts = {c.receivedMessages()[0].text, c.receivedMessages()[1].text};
    EXPECT_NE(std::find(texts.begin(), texts.end(), alphabetText(300)), texts.end());
    EXPECT_NE(std::find(texts.begin(), texts.end(), std::string(300, 'z')), texts.end());
}

TEST_F(DeliveryTest, OneMessageToSeveralRecipients)
{
    Device a(1), b(2), c(3);
    a.connect(t1);
    b.connect(t1);
    c.connect(t2);

    a.sendMessage({2, 3}, "to both");
    net.runLoop();

    ASSERT_EQ(b.receivedMessages().size(), 1u);
    ASSERT_EQ(c.receivedMessages().size(), 1u);
    EXPECT_EQ(b.receivedMessages()[0].text, "to both");
    EXPECT_EQ(c.receivedMessages()[0].text, "to both");
}

TEST_F(DeliveryTest, ConversationInBothDirections)
{
    Device a(1), b(2);
    a.connect(t1);
    b.connect(t2);

    a.sendMessage(2, "ping");
    b.sendMessage(1, "pong");
    net.runLoop();

    ASSERT_EQ(a.receivedMessages().size(), 1u);
    ASSERT_EQ(b.receivedMessages().size(), 1u);
    EXPECT_EQ(a.receivedMessages()[0].text, "pong");
    EXPECT_EQ(b.receivedMessages()[0].text, "ping");
}

TEST_F(DeliveryTest, MessageToAnUnknownDeviceIsDropped)
{
    Device a(1);
    a.connect(t1);

    a.sendMessage(99, "anyone there?");
    net.runLoop();

    EXPECT_EQ(net.droppedFrames(), 1u);
    EXPECT_TRUE(net.isIdle());
}

TEST_F(DeliveryTest, MessageForADeviceThatLeftIsDroppedNotGivenToTheNextDeviceOnItsCode)
{
    Device a(1), c(3);
    a.connect(t1);
    {
        Device b(2);
        b.connect(t2);
        a.sendMessage(2, "for b");
        net.tick(); // the frame has crossed to t2 and is waiting for the downlink, then b leaves
    }
    c.connect(t2); // takes b's old code

    net.runLoop();

    EXPECT_TRUE(c.receivedMessages().empty());
    EXPECT_EQ(net.droppedFrames(), 1u);
}
