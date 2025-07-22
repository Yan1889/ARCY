//
// Created by minhp on 03.07.2025.
//

#ifndef MONEY_H
#define MONEY_H



class Money {
public:
    int moneyBalance = 0;

    void spendMoney(int cost);
    void getMoney(int amount);
    int returnMoney();
    void setMoney(int amount);
};

#endif //MONEY_H
