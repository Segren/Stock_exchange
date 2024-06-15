#include <gtest/gtest.h>
#include "../src/trade_logic.h"
#include "../src/balance.h"

TEST(TradeLogicTest, BasicTrade){
    TradeLogic trade_logic;
    std::unordered_map<int, Balance> balances;

    Order buy_order = {1,10,50.0, true};
    Order sell_order = {2,10,50.0, false};

    trade_logic.process_order(buy_order, balances);
    trade_logic.process_order(sell_order, balances);

    auto balance1 = balances[1].get_balance();
    auto balance2 = balances[2].get_balance();

    EXPECT_EQ(balance1.first, 10);
    EXPECT_EQ(balance1.second, -500.0);
    EXPECT_EQ(balance2.first, -10);
    EXPECT_EQ(balance2.second, 500.0);
}

TEST(TradeLogicTest, PartialTrade) {
    TradeLogic trade_logic;
    std::unordered_map<int, Balance> balances;

    Order buy_order = {1, 10, 50.0, true};
    Order sell_order = {2, 5, 50.0, false};

    trade_logic.process_order(buy_order, balances);
    trade_logic.process_order(sell_order, balances);

    auto balance1 = balances[1].get_balance();
    auto balance2 = balances[2].get_balance();

    EXPECT_EQ(balance1.first, 5);
    EXPECT_EQ(balance1.second, -250.0);
    EXPECT_EQ(balance2.first, -5);
    EXPECT_EQ(balance2.second, 250.0);
}

TEST(TradeLogicTest, MultipleOrders) {
    TradeLogic trade_logic;
    std::unordered_map<int, Balance> balances;

    Order buy_order1 = {1, 10, 50.0, true};
    Order buy_order2 = {3, 5, 51.0, true};
    Order sell_order = {2, 15, 50.0, false};

    trade_logic.process_order(buy_order1, balances);
    trade_logic.process_order(buy_order2, balances);
    trade_logic.process_order(sell_order, balances);

    auto balance1 = balances[1].get_balance();
    auto balance2 = balances[2].get_balance();
    auto balance3 = balances[3].get_balance();

    EXPECT_EQ(balance1.first, 10);
    EXPECT_EQ(balance1.second, -500.0);
    EXPECT_EQ(balance2.first, -15);
    EXPECT_EQ(balance2.second, 755.0);
    EXPECT_EQ(balance3.first, 5);
    EXPECT_EQ(balance3.second, -255.0);
}

TEST(TradeLogicTest, FromTask) {
    TradeLogic trade_logic;
    std::unordered_map<int, Balance> balances;

    Order buy_order1 = {1, 10, 62.0, true};
    Order buy_order2 = {2, 20, 63.0, true};
    Order sell_order = {3, 50, 61.0, false};

    trade_logic.process_order(buy_order1, balances);
    trade_logic.process_order(buy_order2, balances);
    trade_logic.process_order(sell_order, balances);

    auto balance1 = balances[1].get_balance();
    auto balance2 = balances[2].get_balance();
    auto balance3 = balances[3].get_balance();

    EXPECT_EQ(balance1.first, 10);
    EXPECT_EQ(balance1.second, -620.0);
    EXPECT_EQ(balance2.first, 20);
    EXPECT_EQ(balance2.second, -1260.0);
    EXPECT_EQ(balance3.first, -30);
    EXPECT_EQ(balance3.second, 1880.0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}