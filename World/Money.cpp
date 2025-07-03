//
// Created by minhp on 03.07.2025.
//

#include "Money.h"

void Money::spendMoney(int cost)
{
    if (moneyBalance - cost < 0) return;

    moneyBalance -= cost;
}