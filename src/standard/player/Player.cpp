#include "Player.h"
#include "../Globals.h"
#include "../map/Pixel.h"

#include <cassert>
#include <iostream>
#include <cmath>
#include <variant>
#include <algorithm>

#include "../Bombs.h"
#include "raylib.h"

using namespace G;

Player::Player(Pixel *startPos, const int startPop): _id(static_cast<int>(players.size())),
                                                     _population(startPop),
                                                     _maxPopulation(_population) {

    do {
        _color = Color{
            static_cast<unsigned char>(GetRandomValue(0, 255)), // Red
            static_cast<unsigned char>(GetRandomValue(0, 255)), // Green
            static_cast<unsigned char>(GetRandomValue(0, 255)), // Blue
            255 // Alpha
        };
    } while (_color.g > _color.r + 40 && _color.g > _color.b + 40);

    _money = Money();
    _lastActionTime = GetTime();

    GetOwnershipOfPixel(startPos);
    Expand(-1, 0.5);
}

void Player::Update() {
    // expand
    for (auto it = _targetToAttackMap.begin(); it != _targetToAttackMap.end();) {
        ProcessAttackQueue(it->second);
        if (it->second.troops <= 0) {
            it = _targetToAttackMap.erase(it);
        } else if (it->second.set.empty()) {
            _population += it->second.troops;
            it = _targetToAttackMap.erase(it);
        } else {
            ++it;
        }
    }
    UpdateAllDirtyBorder();

    // Add money depending on population
    IncreaseMoney();

    GrowPopulation();
}


void Player::GrowPopulation() {
    _maxPopulation = 50; // min maxPopulation is 50 (wie die 3 in risiko lol)
    _maxPopulation += _allPixels.size(); // +1 for each pixel
    _maxPopulation += 1000 * _cities.size(); // +1000 for each city

    _growthCooldown -= GetTime();
    _growth = _growthFactor * (1.0f - exp(-_population / 10.f));

    if (_population < 50000) {
        _growthFactor = 0.0085f;
    } else {
        _growthFactor = 0.000085f;
    }

    if (_growthCooldown <= 0) {
        const float totalGrowth = _population * _growth;

        const int newPopulation = _population + static_cast<int>(ceil(totalGrowth));
        _population = std::min(newPopulation, _maxPopulation);

        _growthCooldown = _growthRate;
    }
}

void Player::IncreaseMoney() {
    float currentTime = GetTime();

    if (currentTime - _lastActionTime >= _cooldownTime) {
        int peopleAddition = 2;
        int totalAddition = peopleAddition * _population;

        if (_money.moneyBalance + totalAddition > 100000000) _money.moneyBalance = 100000000;
        else _money.moneyBalance += totalAddition;

        _lastActionTime = currentTime;
    }
}

void Player::AddPixelToCenter(Pixel *newP) {
    if (_allPixels.empty()) return;

    _allPixelsSummed_x += newP->x;
    _allPixelsSummed_y += newP->y;
    _centerPixel_x = _allPixelsSummed_x / _allPixels.size();
    _centerPixel_y = _allPixelsSummed_y / _allPixels.size();
}

void Player::RemovePixelFromCenter(Pixel *newP) {
    if (_allPixels.empty()) return;

    _allPixelsSummed_x -= newP->x;
    _allPixelsSummed_y -= newP->y;
    _centerPixel_x = _allPixelsSummed_x / _allPixels.size();
    _centerPixel_y = _allPixelsSummed_y / _allPixels.size();
}

bool Player::TryAddCity(Pixel *pos) {
    const int cost = cityCost * (_cities.size() + 1);

    if (_money.moneyBalance < cost || !_allPixels.contains(pos)) return false;
    _money.spendMoney(cost);

    mySounds.Play(mySounds.cityBuildPool);

    _cities.emplace_back(pos);
    return true;
}

bool Player::TryAddSilo(Pixel *pos) {
    const int cost = siloCost * (_silos.size() + 1);

    if (_money.moneyBalance < cost || !CanBuildCity(pos)) return false;
    _money.spendMoney(cost);

    mySounds.Play(mySounds.cityBuildPool);

    _silos.emplace_back(pos);
    return true;
}

bool Player::CanBuildCity(Pixel *pos) const {
    const int cost = cityCost * (_cities.size() + 1);
    if (_money.moneyBalance < cost || !_allPixels.contains(pos)) return false;
    return true;
}

bool Player::CanBuildSilo(Pixel *pos) const {
    const int cost = siloCost * (_silos.size() + 1);
    if (_money.moneyBalance < cost || !_allPixels.contains(pos)) return false;
    return true;
}

Pixel *Player::GetNearestSiloFromPixel(Pixel *target) const {
    if (_silos.empty()) return nullptr;

    Pixel *bestP = nullptr;
    float bestDistSquared = MAP_WIDTH * MAP_WIDTH + MAP_HEIGHT * MAP_HEIGHT;

    for (Pixel *c: _silos) {
        const float dx = target->x - c->x;
        const float dy = target->y - c->y;
        const float distSquared = dx * dx + dy * dy;
        if (distSquared < bestDistSquared) {
            bestDistSquared = distSquared;
            bestP = c;
        }
    }
    return bestP;
}
