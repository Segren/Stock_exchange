#include "server.h"

Server::Server(boost::asio::io_service& io_service, short port, Core& core)
    : io_service_(io_service),
    acceptor_(io_service, tcp::endpoint(tcp::v4(), port)), 
    core_(core)
{
    std::cout << "Server started! Listen " << port << " port" << std::endl;
    start_accept();
}

void Server::start_accept() {
    try {
        auto new_session = std::make_shared<Session>(io_service_, trade_logic_, clients_balances_, core_);
        acceptor_.async_accept(new_session->socket(),
            boost::bind(&Server::handle_accept, this, new_session,
                boost::asio::placeholders::error));
    } catch (const std::exception& e) {
        std::cerr << "Exception in start_accept: " << e.what() << std::endl;
    }
}

void Server::handle_accept(std::shared_ptr<Session> new_session, const boost::system::error_code& error) {
    if (!error) {
        try {
            std::cout << "New session accepted" << std::endl;
            new_session->start();
        } catch (const std::exception& e) {
            std::cerr << "Exception in handle_accept: " << e.what() << std::endl;
        }
        start_accept();
    } else {
        std::cerr << "Error in handle_accept: " << error.message() << std::endl;
        start_accept();
    }
}

