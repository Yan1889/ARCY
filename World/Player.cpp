#include "Player.h"
#include "Globals.h"
#include "Map/Pixel.h"

#include <cassert>
#include <iostream>
#include <cmath>
#include <variant>
#include <algorithm>

#include "raylib.h"

Player::Player(Pixel *startPos, const int startRadius): _id(G::players.size()) {
    _centerPixel_x = startPos->x;
    _centerPixel_y = startPos->y;
    _allPixelsSummed_x = _centerPixel_x;
    _allPixelsSummed_y = _centerPixel_y;

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

    GetOwnershipOfPixel(&G::territoryMap[_centerPixel_x][_centerPixel_y]);
    UpdateAllDirty();

    Expand(-1, 0.5);
}

void Player::Update() {
    // expand
    for (auto& attackQueue : _targetToAttackMap) {
        ProcessAttackQueue(attackQueue.second);
    }
    UpdateAllDirty();

    // Add money depending on population
    IncreaseMoney();

    _growthCooldown -= GetTime();
    _maxPopulation = 1000 + 1000 * _cityCount;

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

void Player::UpdateAllDirty() {
    for (Pixel *p: _dirtyPixels) {
        UpdateSingleDirty(p);
    }
    _dirtyPixels.clear();
}

void Player::AddBorderPixel(Pixel *p) {
    if (_borderSet.insert(p).second) {
        _borderPixels.push_back(p);
    }
}

void Player::RemoveBorderPixel(Pixel *p) {
    if (_borderSet.erase(p)) {
        auto it = std::ranges::find(_borderPixels, p);
        if (it != _borderPixels.end()) {
            _borderPixels.erase(it);
        }
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


void Player::AddCity(const Vector2 &pos) {
    _cityCount++;
    _cityPositions.push_back(
        &G::territoryMap[static_cast<int>(pos.x)][static_cast<int>(pos.y)]
    );
}

void Player::AddCity(Pixel *pos) {
    _cityCount++;
    _cityPositions.push_back(pos);
}

Pixel* Player::GetNearestCityFromPixel(Pixel* target) const {
    if (_cityPositions.empty()) return nullptr;

    Pixel* bestP = nullptr;
    float bestDistSquared = G::MAP_WIDTH * G::MAP_WIDTH + G::MAP_HEIGHT * G::MAP_HEIGHT;

    for (Pixel* p : _cityPositions) {
        const double dx = target->x - p->x;
        const double dy = target->y - p->y;
        const double distSquared = dx * dx + dy * dy;
        if (distSquared < bestDistSquared) {
            bestDistSquared = distSquared;
            bestP = p;
        }
    }
    return bestP;
}
