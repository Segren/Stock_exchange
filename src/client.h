#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <boost/asio.hpp>
#include "../include/json.hpp"
using boost::asio::ip::tcp;

bool is_number(const std::string& s);
std::string ProcessRegistration(tcp::socket& aSocket);
std::string ReadMessage(tcp::socket& aSocket);


#endif //CLIENT_H