# CMake generated Testfile for 
# Source directory: /Users/dgon/Documents/stock_exchange
# Build directory: /Users/dgon/Documents/stock_exchange/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[TradeLogicTest]=] "/Users/dgon/Documents/stock_exchange/build/test_trade_logic")
set_tests_properties([=[TradeLogicTest]=] PROPERTIES  _BACKTRACE_TRIPLES "/Users/dgon/Documents/stock_exchange/CMakeLists.txt;28;add_test;/Users/dgon/Documents/stock_exchange/CMakeLists.txt;0;")
add_test([=[ServerTest]=] "/Users/dgon/Documents/stock_exchange/build/test_server")
set_tests_properties([=[ServerTest]=] PROPERTIES  _BACKTRACE_TRIPLES "/Users/dgon/Documents/stock_exchange/CMakeLists.txt;29;add_test;/Users/dgon/Documents/stock_exchange/CMakeLists.txt;0;")
