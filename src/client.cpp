#include "client.h"

constexpr int port = 12345;

namespace Requests {
    const std::string Registration = "Registration";
    const std::string Hello = "Hello";
    const std::string Balance = "Balance";
    const std::string Order = "Order";
    const std::string ActiveBuyOrders = "ActiveBuyOrders";
    const std::string ActiveSellOrders = "ActiveSellOrders";
    const std::string GetTrades = "GetTrades";
    const std::string GetQuoteHistory = "GetQuoteHistory";
}

// Отправка сообщения на сервер по шаблону.
void SendMessage(
    tcp::socket& aSocket,
    const std::string& aId,
    const std::string& aRequestType,
    const nlohmann::json& aMessage)
{
    nlohmann::json req;
    req["UserId"] = aId;
    req["ReqType"] = aRequestType;
    req["Message"] = aMessage;

    std::string request = req.dump();
    boost::asio::write(aSocket, boost::asio::buffer(request, request.size()));
}

// Возвращает строку с ответом сервера на последний запрос.
std::string ReadMessage(tcp::socket& aSocket)
{
    boost::asio::streambuf b;
    boost::asio::read_until(aSocket, b, "\0");
    std::istream is(&b);
    std::string line(std::istreambuf_iterator<char>(is), {});
    return line;
}

// "Создаём" пользователя, получаем его ID.
std::string ProcessRegistration(tcp::socket& aSocket)
{
    std::string name;
    std::cout << "Hello! Enter your name: ";
    std::cin >> name;

    // Для регистрации Id не нужен, заполним его нулём
    SendMessage(aSocket, "0", Requests::Registration, name);
    return ReadMessage(aSocket);
}

// Проверка, является ли строка числом
bool is_number(const std::string& s) {
    return !s.empty() && std::find_if(s.begin(), s.end(), [](unsigned char c) { return !std::isdigit(c) && c != '.' && c != '-'; }) == s.end();
}

int main()
{
    try
    {
        boost::asio::io_service io_service;

        tcp::resolver resolver(io_service);
        tcp::resolver::query query(tcp::v4(), "127.0.0.1", std::to_string(port));
        tcp::resolver::iterator iterator = resolver.resolve(query);

        tcp::socket s(io_service);
        s.connect(*iterator);

        // Мы предполагаем, что для идентификации пользователя будет использоваться ID.
        // Тут мы "регистрируем" пользователя - отправляем на сервер имя, а сервер возвращает нам ID.
        // Этот ID далее используется при отправке запросов.
        std::string my_id = ProcessRegistration(s);
        std::cout << "Registered with ID: " << my_id << std::endl;

        while (true)
        {
            // Тут реализовано "бесконечное" меню.
            std::cout << "Menu:\n"
                         "1) Hello Request\n"
                         "2) Check Balance\n"
                         "3) Place Buy Order\n"
                         "4) Place Sell Order\n"
                         "5) Exit\n"
                         "6) View active buy orders\n"
                         "7) View active sell orders\n"
                         "8) View trade history\n"
                         "9) View quote history\n"
                         << std::endl;

            short menu_option_num;
            std::string input;
            std::cin >> input;

            // Проверяем, является ли ввод числом
            try {
                menu_option_num = std::stoi(input);
            } catch (std::invalid_argument&) {
                std::cout << "Invalid input. Please enter a number." << std::endl;
                continue;
            }

            switch (menu_option_num)
            {
                case 1:
                {
                    SendMessage(s, my_id, Requests::Hello, "");
                    std::cout << ReadMessage(s);
                    break;
                }
                case 2:
                {
                    //Запрос баланса пользователя
                    SendMessage(s, my_id, Requests::Balance, "");
                    std::cout << "Balance: " << ReadMessage(s) << std::endl;
                    break;
                }
                case 3:
                {
                    //Размещение ордера на покупку
                    std::string price, volume;
                    std::cout << "Enter buy order price: ";
                    std::cin >> price;
                    if (!is_number(price)) {
                        std::cout << "Invalid price. Please enter a valid number." << std::endl;
                        break;
                    }                    
                    std::cout << "Enter buy order volume: ";
                    std::cin >> volume;
                    if (!is_number(volume)) {
                        std::cout << "Invalid volume. Please enter a valid number." << std::endl;
                        break;
                    }

                    nlohmann::json message= {
                        {"side", "buy"},
                        {"price", std::stod(price)},
                        {"volume", std::stoi(volume)}
                    };  

                    SendMessage(s, my_id, Requests::Order, message);
                    std::cout << "Buy Order Response: " << ReadMessage(s) << std::endl;
                    break;                 
                }
                case 4:
                {
                    //Размещение ордера на продажу
                    std::string price,volume;
                    std::cout << "Enter sell order price: ";
                    std::cin >> price;
                    if (!is_number(price)) {
                        std::cout << "Invalid price. Please enter a valid number." << std::endl;
                        break;
                    }   
                    std::cout << "Enter sell order volume: ";
                    std::cin >> volume;
                    if (!is_number(volume)) {
                        std::cout << "Invalid volume. Please enter a valid number." << std::endl;
                        break;
                    }

                    nlohmann::json message= {
                        {"side", "sell"},
                        {"price", std::stod(price)},
                        {"volume", std::stoi(volume)}
                    };  

                    SendMessage(s, my_id, Requests::Order, message);
                    std::cout << "Sell Order Response: " << ReadMessage(s) << std::endl;
                    break;                                        
                }
                case 5:
                {
                    std::cout << "Client disconnected" << std::endl;
                    exit(0);
                    break;
                }
                case 6:
                {
                    SendMessage(s, my_id, Requests::ActiveBuyOrders, "");
                    std::cout << "Active Buy Orders:\n" << ReadMessage(s) << std::endl;
                    break;                    
                }
                case 7:
                {
                    SendMessage(s, my_id, Requests::ActiveSellOrders, "");
                    std::cout << "Active Sell Orders:\n" << ReadMessage(s) << std::endl;
                    break;                    
                }    
                case 8:
                {
                    SendMessage(s, my_id, Requests::GetTrades, "");
                    std::cout << "Trade history:\n" << ReadMessage(s) << std::endl;
                    break;                      
                }  
                case 9:
                {
                    SendMessage(s, my_id, Requests::GetQuoteHistory, "");
                    std::cout << "Quote history:\n" << ReadMessage(s) << std::endl;
                    break; 
                }          
                default:
                {
                    std::cout << "Unknown menu option\n" << std::endl;
                }
            }
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}