#include <gtest/gtest.h>
#include <boost/asio.hpp>
#include <thread>
#include <chrono>
#include "../src/server.h"
#include "../src/core.h"

using namespace boost::asio;

class ServerTest : public ::testing::Test {
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
        std::this_thread::sleep_for(std::chrono::seconds(1));
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

            std::cout << "send_request: Shutting down socket" << std::endl;
            try{
                socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
            } catch (const boost::system::system_error& e) {
                std::cerr << "Exception in socket shutdown: " << e.what() << std::endl;
            }
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
    std::unique_ptr<Server> server_;
    std::thread server_thread_;
};

TEST_F(ServerTest, Registration) {
    std::cout << "Test Registration started" << std::endl;
    try{
        std::string request = R"({"ReqType": "Registration", "Message": "User1", "Test": true})";
        std::string response = send_request(request);
        EXPECT_EQ(response, "0");
        std::cout << "Test Registration finished" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Exception in Registration test: " << e.what() << std::endl;
        throw;
    }
}

TEST_F(ServerTest, Hello){
    std::cout << "Test Hello started" << std::endl;
    try{
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

TEST_F(ServerTest, DuplicateRegistration) {
    std::cout << "Test Duplicate Registration started" << std::endl;
    try {
        std::string request = R"({"ReqType": "Registration", "Message": "User3", "Test": true})";
        std::string response1 = send_request(request);
        std::string response2 = send_request(request);
        EXPECT_EQ(response1, "2");
        EXPECT_EQ(response2, "Error! User already exists.");
        std::cout << "Test Duplicate Registration finished" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception in Duplicate Registration test: " << e.what() << std::endl;
        throw;
    }
}

TEST_F(ServerTest, CheckBalance) {
    std::cout << "Test Check Balance started" << std::endl;
    try {
        std::string reg_request = R"({"ReqType": "Registration", "Message": "User4", "Test": true})";
        std::string reg_response = send_request(reg_request);
        std::string user_id = reg_response;

        std::string balance_request = R"({"ReqType": "Balance", "UserId": )" + user_id + R"(, "Test": true})";
        std::string balance_response = send_request(balance_request);

        std::string expected_response = R"({"Balance":[0,0.0]})";
        EXPECT_EQ(balance_response, expected_response);
        std::cout << "Test Check Balance finished" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception in Check Balance test: " << e.what() << std::endl;
        throw;
    }
}

TEST_F(ServerTest, InvalidRequestType) {
    std::cout << "Test Invalid Request Type started" << std::endl;
    try {
        std::string invalid_request = R"({"ReqType": "Invalid", "Message": "Test", "Test": true})";
        std::string invalid_response = send_request(invalid_request);
        EXPECT_EQ(invalid_response, "Error! Unknown request type");
        std::cout << "Test Invalid Request Type finished" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception in Invalid Request Type test: " << e.what() << std::endl;
        throw;
    }
}

TEST_F(ServerTest, OrderExecution) {
    std::cout << "Test Order Execution started" << std::endl;
    try {
        // Регистрация покупателя
        std::string reg_request1 = R"({"ReqType": "Registration", "Message": "Buyer", "Test": true})";
        std::string reg_response1 = send_request(reg_request1);
        std::string buyer_id = reg_response1;

        // Регистрация продавца
        std::string reg_request2 = R"({"ReqType": "Registration", "Message": "Seller", "Test": true})";
        std::string reg_response2 = send_request(reg_request2);
        std::string seller_id = reg_response2;

        // Покупатель размещает ордер на покупку 10 USD по цене 60 RUB
        std::string buy_order = R"({"ReqType": "Order", "UserId": )" + buyer_id + R"(, "Message":{"side": "buy", "price": 60, "volume": 10}, "Test": true})";
        std::string buy_response = send_request(buy_order);

        // Продавец размещает ордер на продажу 10 USD по цене 60 RUB
        std::string sell_order = R"({"ReqType": "Order", "UserId": )" + seller_id + R"(, "Message":{"side": "sell", "price": 60, "volume": 10}, "Test": true})";
        std::string sell_response = send_request(sell_order);

        // Проверка баланса покупателя
        std::string balance_request_buyer = R"({"ReqType": "Balance", "UserId": )" + buyer_id + R"(, "Test": true})";
        std::string balance_response_buyer = send_request(balance_request_buyer);
        std::string expected_balance_buyer = R"({"Balance":[10,-600.0]})";
        EXPECT_EQ(balance_response_buyer, expected_balance_buyer);

        // Проверка баланса продавца
        std::string balance_request_seller = R"({"ReqType": "Balance", "UserId": )" + seller_id + R"(, "Test": true})";
        std::string balance_response_seller = send_request(balance_request_seller);
        std::string expected_balance_seller = R"({"Balance":[-10,600.0]})";
        EXPECT_EQ(balance_response_seller, expected_balance_seller);

        std::cout << "Test Order Execution finished" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception in Order Execution test: " << e.what() << std::endl;
        throw;
    }
}




int main(int argc, char **argv){
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
