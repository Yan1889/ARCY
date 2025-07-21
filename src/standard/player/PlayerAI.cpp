//
// Created by yanam on 21.07.2025.
//

#include <stdlib.h>
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

    Pixel *startPixel = GetNearestSiloFromPixel(target);
    if (startPixel == nullptr) return; // doesnt have a silo yet

    constexpr int cost = 10000; // only atom bombs for now
    if (_money.moneyBalance < cost) return;

    _money.spendMoney(cost);
    mySounds.Play(mySounds.misslePool);
    Bombs::allBombs.push_back(SingleBomb{
        .targetPos = target->ToVector2(),
        .originPos = startPixel->ToVector2(),
        .pos = startPixel->ToVector2(),
        .speed = 1,
        .radius = 50.f,
        .type = ATOM
    });
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
    if (_maxPopulation == 0) return;

    // do a random attack if bot has enough troops
    const float popPercentage = _population / _maxPopulation;
    if (popPercentage < 0.75) return;

    // expand with 20% strength and random target (radiation or neutral or other player)
    const int target = rand() % (players.size() + 2) - 2;
    Expand(target, 0.2f);
}
