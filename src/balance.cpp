#include "balance.h"

void Balance::update_balance(int usd, double rub) {
    usd_balance_ += usd;
    rub_balance_ += rub;
}

std::pair<int, double> Balance::get_balance() const {
    return {usd_balance_, rub_balance_};
}