//
// Created by yanam on 21.07.2025.
//

#include <cstdlib>
#include "../Globals.h"
#include "Player.h"
#include "../Bombs.h"

using namespace G;

void Player::BotLogic() {
    BotLogic_Expanding();
    BotLogic_Building();
    BotLogic_Bombing();
}

void Player::BotLogic_Bombing() {
    // randomly bomb something with a 1% chance
    if (rand() > RAND_MAX / 100) return;

    // chose a target that is not yourself
    int targetId{};
    do {
        targetId = rand() % players.size();
    } while (targetId == _id || players[targetId]._dead);

    auto iter = players[targetId]._allPixels.begin();
    std::advance(iter, rand() % players[targetId]._allPixels.size());
    Pixel *target = *iter;

    // Pixel *target = PixelAt(rand() % MAP_WIDTH, rand() % MAP_HEIGHT); // alternative: random firing
    TryLaunchAtomBomb(target);
}

void Player::BotLogic_Building() {
    // get a random pixel
    auto iter = _allPixels.begin();
    std::advance(iter, rand() % _allPixels.size());
    Pixel *randomPixel = *iter;

    // add a random city with 1% chance
    if (rand() < RAND_MAX / 100) {
        TryAddCity(randomPixel);
    }
    // add a random silo with 1% chance
    if (rand() < RAND_MAX / 100) {
        TryAddSilo(randomPixel);
    }
}

void Player::BotLogic_Expanding() {
    if (_maxTotalPopulation == 0) return;

    // do a random attack if bot has enough troops
    const float troopFillPercentage = _totalPopulation * _troopPercentage / static_cast<float>(_maxTotalPopulation);

    if (troopFillPercentage < 0.50) return;

    // expand with 20% strength and random target:
    // radiation (-2) or neutral land (-1) or other player (>=0)
    const int target = rand() % (players.size() + 2) - 2;
    Expand(target, 0.2f);
}
