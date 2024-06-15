#include "trade_logic.h"
#include <algorithm>
#include "balance.h"
#include <queue>


void TradeLogic::process_order(const Order& order, std::unordered_map<int, Balance>& clients_balances) {
    TimedOrder timed_order(order);

    if(order.is_buy) {
        buy_orders_.push_back(timed_order);
        std::sort(buy_orders_.begin(), buy_orders_.end(), [](const TimedOrder& a, const TimedOrder& b){
            return a.order.price > b.order.price || (a.order.price == b.order.price && a.timestamp < b.timestamp);
        });
    } else {
        sell_orders_.push_back(timed_order);
        std::sort(sell_orders_.begin(), sell_orders_.end(), [](const TimedOrder& a, const TimedOrder& b) {
            return a.order.price < b.order.price || (a.order.price == b.order.price && a.timestamp < b.timestamp);
        });
    }

    //логика сопоставления
    while (!buy_orders_.empty() && !sell_orders_.empty()) {
        TimedOrder& buy_order = buy_orders_.front();
        TimedOrder& sell_order = sell_orders_.front();

        if(buy_order.order.price >= sell_order.order.price){
            int traded_volume = std::min(buy_order.order.volume, sell_order.order.volume);   
            double trade_price = (buy_order.timestamp < sell_order.timestamp) ? buy_order.order.price : sell_order.order.price;

            clients_balances[buy_order.order.client_id].update_balance(traded_volume, -trade_price * traded_volume);
            clients_balances[sell_order.order.client_id].update_balance(-traded_volume, trade_price * traded_volume);

            trades_.emplace_back(buy_order.order.client_id, sell_order.order.client_id, traded_volume, trade_price);
            quote_history_.emplace_back(trade_price, std::chrono::system_clock::now());

            buy_order.order.volume -= traded_volume;
            sell_order.order.volume -= traded_volume;

            if(buy_order.order.volume == 0){
                buy_orders_.erase(buy_orders_.begin());
            }

            if(sell_order.order.volume == 0){
                sell_orders_.erase(sell_orders_.begin());
            }
        } else {
            break;
        }
    }
}

std::vector<Order> TradeLogic::get_active_buy_orders() const {
    std::vector<Order> active_buy_orders;
    for (const auto& timed_order : buy_orders_) {
        active_buy_orders.push_back(timed_order.order);
    }
    return active_buy_orders;
}

std::vector<Order> TradeLogic::get_active_sell_orders() const {
    std::vector<Order> active_sell_orders;
    for (const auto& timed_order : sell_orders_) {
        active_sell_orders.push_back(timed_order.order);
    }
    return active_sell_orders;
}

std::vector<Trade> TradeLogic::get_trades() const {
    return trades_;
}

std::vector<Quote> TradeLogic::get_quote_history() const {
    return quote_history_;
}