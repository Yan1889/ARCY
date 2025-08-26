//
// Created by yanam on 21.07.2025.
//

#include <cstdlib>
#include <iostream>

#include "../Globals.h"
#include "Player.h"
#include "../Bombs.h"
#include "../display/DayNightCycle.h"

using namespace G;

void Player::BotLogic() {
    BotLogic_Expanding();
    BotLogic_Building();
    BotLogic_Bombing();
}

void Player::BotLogic_Bombing() {
    Pixel *target = G::GetRandPixel();
    if (target->playerId >= 0) {
        TryLaunchBomb(target, true);
    }
}

void Player::BotLogic_Building() {
    Pixel *randomPixel = G::GetRandPixel();
    // add a random city with 1% chance
    if (rand() < RAND_MAX / 100) {
        TryAddBuilding(CITY, randomPixel);
    }
    // add a random silo with 1% chance
    if (rand() < RAND_MAX / 100) {
        TryAddBuilding(SILO, randomPixel);
    }
}

void Player::BotLogic_Expanding() {
    if (_maxTotalPopulation == 0) return;

    // do a random attack if bot has enough troops
    const float troopFillPercentage = _totalPopulation * _troopPercentage / static_cast<float>(_maxTotalPopulation);

    if (troopFillPercentage < 0.50) return;

    if (DayNightCycle::isNightTime)
    {
        int randomInt = rand() % 100;

        if (randomInt != 0) return;
    }

    // expand with 20% strength and random target:
    // radiation (-2) or neutral land (-1) or other player (>=0)
    const int target = rand() % (players.size() + 2) - 2;
    Expand(target, 0.2f);
}
