#ifndef TRADE_LOGIC_H
#define TRADE_LOGIC_H

#include <unordered_map>
#include <vector>
#include "balance.h"
#include <chrono>

struct Order {
    int client_id;
    int volume;
    double price;
    bool is_buy;
};

struct Quote {
    double price;
    std::chrono::time_point<std::chrono::system_clock> timestamp;

    Quote(double p, std::chrono::time_point<std::chrono::system_clock> t) : price(p),timestamp(t) {}
};

struct TimedOrder {
    Order order;
    std::chrono::time_point<std::chrono::steady_clock> timestamp;

    TimedOrder(const Order& o) : order(o), timestamp(std::chrono::steady_clock::now()) {}

    bool operator < (const TimedOrder& other) const {
        return timestamp > other.timestamp;
    }
};

struct Trade {
    int buy_client_id;
    int sell_client_id;
    int volume;
    double price;
    std::chrono::time_point<std::chrono::system_clock> timestamp;

    Trade(int buy_id, int sell_id, int vol, double pr)
        : buy_client_id(buy_id), sell_client_id(sell_id), volume(vol), price(pr), timestamp(std::chrono::system_clock::now()) {}
};


class TradeLogic {
public:
    void process_order(const Order& order, std::unordered_map<int, Balance>& clients_balances);
    std::vector<Order> get_active_buy_orders() const;
    std::vector<Order> get_active_sell_orders() const;    
    std::vector<Trade> get_trades() const;
    std::vector<Quote> get_quote_history() const;
private:
    std::vector<TimedOrder> buy_orders_;
    std::vector<TimedOrder> sell_orders_;
    std::vector<Trade> trades_;
    std::vector<Quote> quote_history_; //хранение истории котировок
};

#endif //TRADE_LOGIC_H