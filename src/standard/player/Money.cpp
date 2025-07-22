//
// Created by minhp on 03.07.2025.
//

#include "Money.h"

void Money::spendMoney(int cost)
{
    if (moneyBalance - cost < 0) return;

    moneyBalance -= cost;
}

void Money::getMoney(int amount)
{
    if (moneyBalance + amount > 100000000) return;
    moneyBalance += amount;
}

int Money::returnMoney()
{
    return moneyBalance;
}

void Money::setMoney(int amount)
{
    moneyBalance = amount;
}
