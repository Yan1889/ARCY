#include "Player.h"
#include "../Globals.h"
#include "../map/Pixel.h"

#include <cassert>
#include <iostream>
#include <cmath>
#include <algorithm>

#include "../Bombs.h"
#include "raylib.h"

using namespace G;
using namespace Terrain;

Player::Player(Pixel *startPos, const std::string &name): _id(static_cast<int>(players.size())),
                                                          _name(name) {
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

    UpdatePopulationMaxValues();
    _totalPopulation = _maxTotalPopulation;
    _troops = _maxTroops;
    _workers = _maxWorkers;

    Expand(-1, 0.5);
}

void Player::Update() {
    // expand
    for (auto &pair: _targetToAttackMap) {
        if (pair.second.troops > 0) {
            ProcessAttackQueue(pair.second);
        }
        if (pair.second.set.empty()) {
            _troops += pair.second.troops;
            pair.second.troops = 0;
        }
    }
    UpdateAllDirtyBorder();

    UpdatePopulationMaxValues();
    GrowPopulation();
    IncreaseMoney();
}

void Player::UpdatePopulationMaxValues() {
    _maxTotalPopulation = 200; // min maxPopulation 200 (you are cooked if < 200)
    _maxTotalPopulation += _pixelCount; // +1 for each pixel
    _maxTotalPopulation += 1000 * GetBuildingsOfType(CITY).size(); // +1000 for each city

    _maxTroops = _maxTotalPopulation * _troopPercentage;
    _maxWorkers = _maxTotalPopulation * (1 - _troopPercentage);
}

void Player::GrowPopulation() {
    // some random ass formula
    int addedPeople = 10 + std::pow(_totalPopulation, 0.73) / 4;

    const float ratio = 1 - _totalPopulation / static_cast<float>(_maxTotalPopulation);
    addedPeople *= ratio;

    if (_bot) {
        addedPeople *= 0.5;
    }

    const int addedTroops = addedPeople * _troopPercentage;
    const int addedWorkers = addedPeople * (1 - _troopPercentage);
    _troops = std::min(_troops + addedTroops, _maxTroops);
    _workers = std::min(_workers + addedWorkers, _maxWorkers);
    _totalPopulation = _troops + _workers;
}

void Player::IncreaseMoney() {
    // another random formula
    _money.getMoney(std::floor(0.045 * std::pow(_workers, 0.7)));
}

void Player::AddPixelToCenter(Pixel *newP) {
    if (_pixelCount == 0) return;

    _allPixelsSummed_x += newP->x;
    _allPixelsSummed_y += newP->y;
    _centerPixel_x = _allPixelsSummed_x / _pixelCount;
    _centerPixel_y = _allPixelsSummed_y / _pixelCount;
}

void Player::RemovePixelFromCenter(Pixel *newP) {
    if (_pixelCount == 0) return;

    _allPixelsSummed_x -= newP->x;
    _allPixelsSummed_y -= newP->y;
    _centerPixel_x = _allPixelsSummed_x / _pixelCount;
    _centerPixel_y = _allPixelsSummed_y / _pixelCount;
}

bool Player::TryAddBuilding(BUILDING_TYPE t, Pixel *pos) {
    if (!CanBuildType(t, pos)) return false;

    const int cost = GetCost(t) * (GetBuildingsOfType(t).size() + 1);
    _money.spendMoney(cost);
    _buildings.emplace_back(pos, t);

    if (_id == 0) mySounds.Play(mySounds.cityBuildPool);
    return true;
}

bool Player::CanBuildType(BUILDING_TYPE t, Pixel *pos) {
    if (!pos) return false;
    const int cost = GetCost(t) * (GetBuildingsOfType(t).size() + 1);
    return _money.moneyBalance > cost && pos->playerId == _id;
}

bool Player::CanLaunchBomb(Pixel *pixel, const int cost) {
    if (!pixel) return false;
    if (_money.moneyBalance < cost) return false;
    if (GetBuildingsOfType(SILO).empty()) return false;

    const Kind k = pixel->kind;
    return k != LOW_WATER && k != DEEP_WATER;
}


void Player::TryLaunchBomb(Pixel *target, const bool isAtom) {
    const int cost = isAtom? Bombs::ATOM_COST: Bombs::H_COST;
    const int radius = isAtom? Bombs::ATOM_RADIUS: Bombs::H_RADIUS;
    const int speed = isAtom? Bombs::ATOM_SPEED: Bombs::H_SPEED;

    if (!CanLaunchBomb(target, cost)) return;
    if (target->kind == DEEP_WATER || target->kind == LOW_WATER) return;

    _money.spendMoney(cost);
    mySounds.Play(mySounds.misslePool);

    Pixel *startPixel = GetNearestSiloFromPixel(target);
    Bombs::allBombs.push_back(SingleBomb{
        .targetPos = target->ToVector2(),
        .originPos = startPixel->ToVector2(),
        .pos = startPixel->ToVector2(),
        .radius = radius,
        .isAtom = isAtom,
        .speed = speed,
    });
}

Pixel *Player::GetNearestSiloFromPixel(Pixel *target) {
    const std::vector<Building *> silos = GetBuildingsOfType(SILO);

    if (silos.empty()) return nullptr;

    Pixel *bestP = nullptr;
    int bestDistSquared = MAP_WIDTH * MAP_WIDTH + MAP_HEIGHT * MAP_HEIGHT;

    for (Building *s: silos) {
        const int dx = target->x - s->pos->x;
        const int dy = target->y - s->pos->y;
        const int distSquared = dx * dx + dy * dy;
        if (distSquared < bestDistSquared) {
            bestDistSquared = distSquared;
            bestP = s->pos;
        }
    }
    return bestP;
}

std::vector<Building *> Player::GetBuildingsOfType(const BUILDING_TYPE t) {
    std::vector<Building *> result;
    for (Building& b: _buildings) {
        if (b.type == t) {
            result.push_back(&b);
        }
    }
    return result;
}