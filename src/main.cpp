#include "server.h"
#include <boost/asio.hpp>

int main() {
    try{
        boost::asio::io_service io_service;
        Core& core = Core::GetCore();
        Server server(io_service, 12345, core);
        io_service.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}