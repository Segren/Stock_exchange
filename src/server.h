#ifndef SERVER_H
#define SERVER_H

#include <cstdlib>
#include <iostream>
#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>
#include "../include/json.hpp"
#include <unordered_map>
#include "session.h"
#include "trade_logic.h"
#include "balance.h"
#include "core.h"
#include <memory>

using boost::asio::ip::tcp;

class Server
{
public:
    Server(boost::asio::io_service& io_service, short port, Core& core);
private:
    void start_accept();
    void handle_accept(std::shared_ptr<Session> new_session,
        const boost::system::error_code& error);

    boost::asio::io_service& io_service_;
    tcp::acceptor acceptor_;
    TradeLogic trade_logic_;
    std::unordered_map<int, Balance> clients_balances_;
    Core& core_;
};

#endif // SERVER_H
