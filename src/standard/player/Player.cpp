#include "Player.h"
#include "../Globals.h"
#include "../map/Pixel.h"

#include <cassert>
#include <iostream>
#include <cmath>
#include <variant>
#include <algorithm>

#include "raylib.h"

Player::Player(Pixel *startPos, const int startRadius): _id(static_cast<int>(G::players.size())) {
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

    _growthCooldown -= GetTime();
    _maxPopulation = 1000 + 1000 * _cities.size();

    _growth = _growthFactor * (1.0f - exp(-_population / 10.f));

    if (_population < 50000) {
        _growthFactor = 0.0085f;
    } else {
        _growthFactor = 0.000085f;
    }

    GrowPopulation();
}


void Player::GrowPopulation() {
    // Todo: calculate with respect to _allPixels.size() too

    if (_growthCooldown <= 0) {
        const float totalGrowth = _population * _growth;
        if (_population < _maxPopulation) {
            _population += static_cast<int>(ceil(totalGrowth));
        }
        if (_population > _maxPopulation) {
            _population = _maxPopulation;
        }
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


void Player::AddBorderPixel(Pixel *p) {
    if (_border_set.insert(p).second) {
        _border_vec.push_back(p);
    }
}

void Player::RemoveBorderPixel(Pixel *p) {
    if (_border_set.erase(p)) {
        _border_vec.erase(std::ranges::find(_border_vec, p));
    }
}

void Player::AddPixelToCenter(Pixel *newP) {
    if (_allPixels.empty()) return;

    _allPixelsSummed_x += newP->x;
    _allPixelsSummed_y += newP->y;
    _centerPixel_x = _allPixelsSummed_x / static_cast<int>(_allPixels.size());
    _centerPixel_y = _allPixelsSummed_y / static_cast<int>(_allPixels.size());
}

void Player::RemovePixelFromCenter(Pixel *newP) {
    if (_allPixels.empty()) return;

    _allPixelsSummed_x -= newP->x;
    _allPixelsSummed_y -= newP->y;
    _centerPixel_x = _allPixelsSummed_x / static_cast<int>(_allPixels.size());
    _centerPixel_y = _allPixelsSummed_y / static_cast<int>(_allPixels.size());
}

bool Player::TryAddCity(Pixel *pos) {
    const int cost = 10000 * (_cities.size() + 1);

    if (_money.moneyBalance < cost || !_allPixels.contains(pos)) return false;
    _money.spendMoney(cost);

    _cities.emplace_back(pos);
    return true;
}
bool Player::TryAddSilo(Pixel *pos) {
    const int cost = 10000 * (_silos.size() + 1);

    if (_money.moneyBalance < cost || !_allPixels.contains(pos)) return false;
    _money.spendMoney(cost);

    _silos.emplace_back(pos);
    return true;
}

Pixel* Player::GetNearestSiloFromPixel(Pixel* target) const {
    if (_silos.empty()) return nullptr;

    Pixel* bestP = nullptr;
    float bestDistSquared = G::MAP_WIDTH * G::MAP_WIDTH + G::MAP_HEIGHT * G::MAP_HEIGHT;

    for (const MissileSilo& c : _silos) {
        const float dx = target->x - c.pos->x;
        const float dy = target->y - c.pos->y;
        const float distSquared = dx * dx + dy * dy;
        if (distSquared < bestDistSquared) {
            bestDistSquared = distSquared;
            bestP = c.pos;
        }
    }
    return bestP;
}
