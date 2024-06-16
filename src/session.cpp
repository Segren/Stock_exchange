#include "session.h"

tcp::socket& Session::socket()
{
    return socket_;
}

void Session::start()
{

    std::cout << "Session started" << std::endl;
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        [this, self](const boost::system::error_code& error, std::size_t bytes_transferred){
            handle_read(error, bytes_transferred);
        });
}

bool Session::is_test_request(const std::string& request) {
    auto j = json::parse(request);
    return j.contains("Test") && j["Test"].get<bool>();
}

void Session::handle_read(const boost::system::error_code& error,
    size_t bytes_transferred)
{
    if (!error)
    {
        std::cout << "Read " << bytes_transferred << " bytes\n";
        data_[bytes_transferred] = '\0';

        if (is_test_request(data_)) {
            from_test_ = true;
        }

        try {
            // Парсим json, который пришёл нам в сообщении.
            auto j = json::parse(data_);
            auto reqType = j["ReqType"];

            std::string reply = "Error! Unknown request type";
            if (reqType == "Registration")
            {
                std::cout << "reqType == Registration" << std::endl; 
                // Это реквест на регистрацию пользователя.
                // Добавляем нового пользователя и возвращаем его ID.
                reply = core_.RegisterNewUser(j["Message"]);
            }
            else if (reqType == "Hello")
            {
                std::cout << "reqType == Hello" << std::endl; 
                // Это реквест на приветствие.
                // Находим имя пользователя по ID и приветствуем его по имени.
                std::string user_id = get_user_id(j);
                std::cout << "user_id == " << user_id << std::endl;
                reply = "Hello, " + Core::GetCore().GetUserName(user_id) + "!\n";
            }
            else if (reqType == "Order") 
            {
                std::cout << "reqType == Order" << std::endl;
                std::cout << j << std::endl;
                auto order_details = j["Message"];

                std::string str_volume = get_volume(order_details);
                std::cout << "str_volume == " << str_volume << std::endl;
                int volume = std::stoi(str_volume);

                std::string str_price = get_price(order_details); //ошибка
                std::cout << "str_price == " << str_price << std::endl;
                double price = std::stod(str_price);

                std::string side = order_details["side"];
                std::cout << "side == " << side << std::endl;

                std::string user_id = get_user_id(j);
                int client_id = std::stoi(user_id);;

                Order order{client_id, volume, price, side == "buy"};
                trade_logic_.process_order(order, clients_balances_);
                reply = "Order processed.";
            }
            else if (reqType == "Balance") 
            {
                std::cout << "reqType == Balance" << std::endl;
                //Возвращаем информацию по балансу
                std::string user_id = get_user_id(j);
                int client_id = std::stoi(user_id);
                std::cout << "Balance: client_id parced" << std::endl;
                json response = {{"Balance", clients_balances_[client_id].get_balance()}};
                std::cout << "Balance: response formed" << std::endl;
                reply = response.dump();
                std::cout << "Balance: reply dumped" << std::endl;
            } else if (reqType == "Shutdown") {
                std::cout << "Shutdown request received" << std::endl;
                stop();  // Завершаем работу сервера
                //return;
            } else if (reqType == "ActiveBuyOrders") {
                std::cout << "reqType == ActiveBuyOrders" << std::endl;
                //Возвращаем информацию по активным заявкам на покупку
                auto orders = trade_logic_.get_active_buy_orders(); 
                json response;
                if(orders.empty()){
                    response = {{"message", "No buy orders have been made yet"}};
                    reply  = response.dump(1);
                } 
                else{
                    for(const auto& order : orders) {
                        response.push_back({
                            {"client_id", order.client_id},
                            {"volume", order.volume},
                            {"price", order.price},
                            {"side", "buy"}
                        });
                        reply = response.dump(4);
                    }
                }
            } else if (reqType == "ActiveSellOrders") {
                std::cout << "reqType == ActiveSellOrders" << std::endl;
                //Возвращаем информацию по активным заявкам на продажу
                auto orders = trade_logic_.get_active_sell_orders(); 
                json response;
                if(orders.empty()){
                    response = {{"message", "No sell orders have been made yet"}};
                    reply  = response.dump(1);
                } else{
                    for(const auto& order : orders) {
                        response.push_back({
                            {"client_id", order.client_id},
                            {"volume", order.volume},
                            {"price", order.price},
                            {"side", "sell"}
                        });
                        reply = response.dump(4);  
                    }
                }              
            } else if (reqType == "GetTrades") {
                std::cout << "reqType == GetTrades" << std::endl;
                //Возвращаем историю по сделкам
                auto trades = trade_logic_.get_trades();
                json response;
                if(trades.empty()){
                    response = {{"message", "No trades have been made yet"}};
                    reply  = response.dump(1);
                } else {
                    for(const auto& trade : trades) {
                        std::time_t timestamp = std::chrono::system_clock::to_time_t(trade.timestamp);
                        std::stringstream ss;
                        ss << std::put_time(std::localtime(&timestamp), "%Y-%m-%d %H:%M:%S");
                        response.push_back({
                            {"buyer_id", trade.buy_client_id},
                            {"seller_id", trade.sell_client_id},
                            {"volume", trade.volume},
                            {"price", trade.price},
                            {"timestamp", ss.str()}
                        });
                        reply = response.dump(4);
                    }
                }
            } else if (reqType == "GetQuoteHistory") {
                std::cout << "reqType == GetQuoteHistory" << std::endl;
                //Возвращаем историю котировок
                auto quotes = trade_logic_.get_quote_history();
                json response; 
                if(quotes.empty()){
                    response = {{"message", "No quote history just yet"}};
                    reply  = response.dump(1);
                } else {    
                    for(const auto& quote:quotes){
                        std::time_t timestamp = std::chrono::system_clock::to_time_t(quote.timestamp);
                        std::stringstream ss;
                        ss << std::put_time(std::localtime(&timestamp), "%Y-%m-%d %H:%M:%S");
                        response.push_back({
                            {"price", quote.price},
                            {"timestamp", ss.str()}
                        });
                        reply = response.dump(4);
                    }

                }       
            }
            send_reply(reply);
        } catch (const std::exception& e) {
            std::cerr << "Exception in handle_read: " << e.what() << std::endl;
            send_reply("Error processing request: " + std::string(e.what()));
        }
    }
    else
    {
        std::cerr << "Client disconnected: " << error.message() << std::endl;
    }
}

std::string Session::get_user_id(const json& j) {
    std::string user_id;
    if(j["UserId"].is_number()){
        user_id = std::to_string(j["UserId"].get<int>());
    } else if (j["UserId"].is_string()) {
        user_id = j["UserId"].get<std::string>();
    } else {
        throw std::runtime_error("Invalid UserId type");
    }
    return user_id;
}

std::string Session::get_volume(const json& j) {
    std::cout << j << std::endl;
    std::string volume_str;
    if(j["volume"].is_number()){
        std::cout << "get_volume: volume is num" << std::endl;
        volume_str = std::to_string(j["volume"].get<int>());
    } else if (j["volume"].is_string()) {
        std::cout << "get_volume: volume is string" << std::endl;
        volume_str = j["volume"].get<std::string>();
    } else {
        throw std::runtime_error("Invalid volume type");
    }
    return volume_str;
}

std::string Session::get_price(const json& j) {
    std::string price_str;
    if(j["price"].is_number()){
        price_str = std::to_string(j["price"].get<double>());
    } else if (j["price"].is_string()) {
        price_str = j["price"].get<std::string>();
    } else {
        throw std::runtime_error("Invalid price type");
    }
    return price_str;
}

void Session::send_reply(const std::string& reply) {
    auto self(shared_from_this());
    std::cout << "Sending reply: " << reply << std::endl;
    boost::asio::async_write(socket_,
        boost::asio::buffer(reply, reply.size()),
        [this, self](const boost::system::error_code& error, std::size_t){
            handle_write(error);
        });
}

void Session::handle_write(const boost::system::error_code& error)
{
    if (!error)
    {
        std::cout << "Write complete\n";
        if(!from_test_)
            start();    
    }
    else
    {
        std::cerr << "Error in handle_write: " << error.message() << std::endl;
        stop();
    }
}

void Session::stop() {
    std::cout << "Closing socket\n";
    boost::system::error_code ec;
    if(socket_.is_open()){
        socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
        if(ec) {
            std::cerr << "Error in socket shutdown: " << ec.message() << std::endl;
        }
        socket_.close(ec);
        if(ec){
            std::cerr << "Error in socket close: " << ec.message() << std::endl;
        }
    } else {
                std::cerr << "Socket is not open, no need to shutdown or close.\n";
    }
}