#ifndef BALANCE_H
#define BALANCE_H

#include <utility>

class Balance {
public:
    void update_balance(int usd, double rub);
    std::pair<int, double> get_balance() const;

private:
    int usd_balance_=0;
    double rub_balance_=0.0;
};

#endif //BALANCE_H

