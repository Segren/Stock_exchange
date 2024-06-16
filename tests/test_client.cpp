#include <gtest/gtest.h>
#include <boost/asio.hpp>
#include <thread>
#include "../src/server.h"
#include "../src/client.h"
#include "../src/core.h"
#include "../src/trade_logic.h"

using namespace boost::asio;

class ClientTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::cout << "SetUp started" << std::endl;
        io_service_ = std::make_unique<boost::asio::io_service>();
        work_ = std::make_unique<boost::asio::io_service::work>(*io_service_);
        core_ = std::make_unique<Core>();
        server_ = std::make_unique<Server>(*io_service_, 12345, Core::GetCore());
        server_thread_ = std::thread([this] {
            std::cout << "Server thread started" << std::endl;
            io_service_->run();
        });
        std::cout << "SetUp finished" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1)); //Дает время серверу запуститься + удобно для отладки. Закомментить для быстрого прохода тестов
    }

    void TearDown() override {
        std::cout << "TearDown started" << std::endl;

        work_.reset();
        io_service_->stop();
        std::cout << "io_service stopped" << std::endl;
        if (server_thread_.joinable()) {
            server_thread_.join();
            std::cout << "Server thread joined" << std::endl;
        }
        std::cout << "TearDown finished" << std::endl;
    }

    std::string send_request(const std::string& request) {
        try {
            std::cout << "send_request: Creating io_service" << std::endl;
            boost::asio::io_service io_service;
            std::cout << "send_request: Creating socket" << std::endl;
            boost::asio::ip::tcp::socket socket(io_service);
            std::cout << "send_request: Creating resolver" << std::endl;
            boost::asio::ip::tcp::resolver resolver(io_service);
            boost::asio::ip::tcp::resolver::query query("127.0.0.1", "12345");
            boost::asio::connect(socket, resolver.resolve(query));

            std::cout << "send_request: Sending request" << std::endl;
            boost::asio::write(socket, boost::asio::buffer(request + "\n"));

            boost::asio::streambuf response;
            std::cout << "send_request: Waiting for response" << std::endl;
            boost::system::error_code error;

            boost::asio::read_until(socket, response, "\n", error);

            if (error && error != boost::asio::error::eof) {
                throw boost::system::system_error(error);
            }

            std::istream is(&response);
            std::string reply;
            std::getline(is, reply);
            std::cout << "send_request: Received reply: " << reply << std::endl;

            socket.close();
            std::cout << "Request sent and response received" << std::endl;
            return reply;
        } catch (const std::exception& e) {
            std::cerr << "Exception in send_request: " << e.what() << std::endl;
            throw;
        }
    }

    std::unique_ptr<boost::asio::io_service> io_service_;
    std::unique_ptr<boost::asio::io_service::work> work_;
    std::unique_ptr<Core> core_;
    std::unique_ptr<TradeLogic> trade_logic_;
    std::unique_ptr<Server> server_;
    std::thread server_thread_;
};

TEST_F(ClientTest, Registration) {
    std::cout << "Test Registration started" << std::endl;
    try {
        std::string request = R"({"ReqType": "Registration", "Message": "User1", "Test": true})";
        std::string response = send_request(request);
        EXPECT_EQ(response, "0");
        std::cout << "Test Registration finished" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception in Registration test: " << e.what() << std::endl;
        throw;
    }
}

TEST_F(ClientTest, Hello) {
    std::cout << "Test Hello started" << std::endl;
    try {
        std::string reg_request = R"({"ReqType": "Registration", "Message": "User2", "Test": true})";
        std::string reg_response = send_request(reg_request);
        std::string user_id = reg_response;

        std::string request = R"({"ReqType": "Hello", "UserId": )" + user_id + R"(, "Test": true})";
        std::cout << request << std::endl;
        std::string response = send_request(request);
        EXPECT_EQ(response, "Hello, User2!");
    } catch (const std::exception& e) {
        std::cerr << "Exception in Hello test: " << e.what() << std::endl;
        throw;
    }
}

TEST_F(ClientTest, CheckBalance) {
    std::cout << "Test Check Balance started" << std::endl;
    try {
        std::string reg_request = R"({"ReqType": "Registration", "Message": "User3", "Test": true})";
        std::string reg_response = send_request(reg_request);
        std::string user_id = reg_response;

        std::string balance_request = R"({"ReqType": "Balance", "UserId": )" + user_id + R"(, "Test": true})";
        std::string balance_response = send_request(balance_request);

        // Ожидаемый баланс нового пользователя - 0
        std::string expected_response = R"({"Balance":[0,0.0]})";
        EXPECT_EQ(balance_response, expected_response);
        std::cout << "Test Check Balance finished" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception in Check Balance test: " << e.what() << std::endl;
        throw;
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
