CMAKE_MINIMUM_REQUIRED(VERSION 3.21)
PROJECT(ClienServerEcn)

# Найти пакеты Boost, Threads и GTest
FIND_PACKAGE(Boost 1.40 COMPONENTS system REQUIRED)
FIND_PACKAGE(Threads REQUIRED)
FIND_PACKAGE(GTest REQUIRED)

# Включить директории заголовочных файлов
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${GTEST_INCLUDE_DIR})

# Установить стандарт C++
SET(CMAKE_CXX_STANDARD 17)

# Создать исполняемый файл для сервера
ADD_EXECUTABLE(server src/main.cpp src/balance.cpp src/core.cpp src/server.cpp src/session.cpp src/trade_logic.cpp)
TARGET_LINK_LIBRARIES(server PRIVATE ${Boost_LIBRARIES} Threads::Threads)

# Создать исполняемый файл для клиента
ADD_EXECUTABLE(client src/client.cpp)
TARGET_LINK_LIBRARIES(client PRIVATE ${Boost_LIBRARIES} Threads::Threads)

# Включить тестирование
enable_testing()

# Создать исполняемые файлы для тестов
add_executable(test_trade_logic tests/test_trade_logic.cpp src/trade_logic.cpp src/balance.cpp)
add_executable(test_server tests/test_server.cpp src/server.cpp src/session.cpp src/trade_logic.cpp src/balance.cpp src/core.cpp)
add_executable(test_client tests/test_client.cpp src/server.cpp src/session.cpp src/core.cpp src/trade_logic.cpp src/balance.cpp)

# Подключить библиотеки
target_link_libraries(test_trade_logic PRIVATE ${Boost_LIBRARIES} GTest::GTest GTest::Main)
target_link_libraries(test_server PRIVATE ${Boost_LIBRARIES} GTest::GTest GTest::Main)
target_link_libraries(test_client PRIVATE ${Boost_LIBRARIES} GTest::GTest GTest::Main)

# Добавить тесты
add_test(NAME TradeLogicTest COMMAND test_trade_logic)
add_test(NAME ServerTest COMMAND test_server)
add_test(NAME ClientTest COMMAND test_client)
