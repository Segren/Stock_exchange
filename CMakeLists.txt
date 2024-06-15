CMAKE_MINIMUM_REQUIRED(VERSION 3.21)
PROJECT(ClienServerEcn)


FIND_PACKAGE(Boost 1.40 COMPONENTS system REQUIRED)
FIND_PACKAGE(Threads REQUIRED)
FIND_PACKAGE(GTest REQUIRED)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${GTEST_INCLUDE_DIR})


SET(CMAKE_CXX_STANDARD 17)

ADD_EXECUTABLE(server src/main.cpp src/balance.cpp src/core.cpp src/server.cpp src/session.cpp src/trade_logic.cpp)
TARGET_LINK_LIBRARIES(server PRIVATE ${Boost_LIBRARIES} Threads::Threads )

ADD_EXECUTABLE(client src/client.cpp)
TARGET_LINK_LIBRARIES(client PRIVATE ${Boost_LIBRARIES} Threads::Threads )

enable_testing()
add_executable(test_trade_logic tests/test_trade_logic.cpp src/trade_logic.cpp src/balance.cpp)
add_executable(test_server tests/test_server.cpp src/server.cpp src/session.cpp src/trade_logic.cpp src/balance.cpp src/core.cpp)
target_link_libraries(test_trade_logic PRIVATE ${Boost_LIBRARIES} GTest::GTest GTest::Main)
target_link_libraries(test_server PRIVATE ${Boost_LIBRARIES} GTest::GTest GTest::Main)

add_test(NAME TradeLogicTest COMMAND test_trade_logic)
add_test(NAME ServerTest COMMAND test_server)
