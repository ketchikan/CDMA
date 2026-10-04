#include <gtest/gtest.h>

#include <memory>
#include <set>
#include <stdexcept>
#include <vector>

#include "../src/network/Device.hpp"
#include "../src/network/Network.hpp"
#include "../src/network/Tower.hpp"

// A fixture: the constructor runs before every TEST_F, so each test gets its own fresh Network and tower
class NetworkTest : public ::testing::Test
{
protected:
    Network net;
    Tower &tower = net.addTower();
};

TEST_F(NetworkTest, ConnectingAssignsDistinctCodesAndNeverRowZero)
{
    Device a(1), b(2), c(3);
    a.connect(tower);
    b.connect(tower);
    c.connect(tower);

    std::set<CodeIdx> codes = {a.getCodeIdx(), b.getCodeIdx(), c.getCodeIdx()};
    EXPECT_EQ(codes.size(), 3u);
    EXPECT_EQ(codes.count(0), 0u); // row 0 is reserved
}

TEST_F(NetworkTest, NetworkRemembersWhichTowerEachDeviceIsOn)
{
    Tower &other = net.addTower();
    Device a(1), b(2);
    a.connect(tower);
    b.connect(other);

    EXPECT_EQ(net.towerFor(1), &tower);
    EXPECT_EQ(net.towerFor(2), &other);
    EXPECT_EQ(net.towerFor(99), nullptr);
    EXPECT_EQ(tower.getDevices().size(), 1u);
}

TEST_F(NetworkTest, DisconnectingFreesTheCodeForReuse)
{
    Device a(1), b(2);
    a.connect(tower);
    CodeIdx freed = a.getCodeIdx();
    a.disconnect();

    EXPECT_FALSE(a.isConnected());
    EXPECT_EQ(net.towerFor(1), nullptr);

    b.connect(tower);
    EXPECT_EQ(b.getCodeIdx(), freed);
}

TEST_F(NetworkTest, DeviceDisconnectsItselfWhenDestroyed)
{
    {
        Device temp(5);
        temp.connect(tower);
        EXPECT_EQ(net.towerFor(5), &tower);
    }
    EXPECT_EQ(net.towerFor(5), nullptr);
}

TEST_F(NetworkTest, DuplicateIdOrDoubleConnectThrows)
{
    Device a(1), sameId(1);
    a.connect(tower);

    EXPECT_THROW(a.connect(tower), std::logic_error);
    EXPECT_THROW(sameId.connect(tower), std::invalid_argument);
    EXPECT_FALSE(sameId.isConnected()); // a failed connect leaves no trace
}

TEST_F(NetworkTest, RunsOutOfCodesAfterSixtyThreeDevices)
{
    std::vector<std::unique_ptr<Device>> devices;
    for (DeviceID id = 1; id <= WalshSize - 1; id++) // 63: row 0 is reserved
    {
        devices.push_back(std::make_unique<Device>(id));
        devices.back()->connect(tower);
    }

    Device extra(1000);
    EXPECT_THROW(extra.connect(tower), std::runtime_error);

    devices[10]->disconnect();
    EXPECT_NO_THROW(extra.connect(tower));
}
