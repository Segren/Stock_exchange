#ifndef SESSION_H
#define SESSION_H

#include <boost/asio.hpp>
#include <unordered_map>
#include <vector>
#include "trade_logic.h"
#include "balance.h"
#include "core.h"
#include "../include/json.hpp"
#include <iostream>


using boost::asio::ip::tcp;
using json = nlohmann::json;


class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(boost::asio::io_service& io_service, TradeLogic& trade_logic, std::unordered_map<int, Balance>& clients_balances, Core& core)
        : socket_(io_service), trade_logic_(trade_logic), clients_balances_(clients_balances), core_(core) {}

    tcp::socket& socket();

    void start();
    void stop();

    bool is_test_request(const std::string& request);
    void send_reply(const std::string& reply);
    std::string get_user_id(const json& j);
    std::string get_price(const json& j);
    std::string get_volume(const json& j);


    // Обработка полученного сообщения.
    void handle_read(const boost::system::error_code& error,
        size_t bytes_transferred);

    void handle_write(const boost::system::error_code& error);

    void set_from_test(bool value) { from_test_ = value; }


private:
    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
    TradeLogic& trade_logic_;
    std::unordered_map<int, Balance>& clients_balances_; //!!
    Core& core_;
    bool from_test_; //флаг для указания соединения с тестом
};

#endif // SESSION_H


