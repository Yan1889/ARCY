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
    for (auto it = _targetToAttackMap.begin(); it != _targetToAttackMap.end();) {
        ProcessAttackQueue(it->second);
        if (it->second.troops <= 0) {
            it = _targetToAttackMap.erase(it);
        } else if (it->second.set.empty()) {
            _troops += it->second.troops;
            it = _targetToAttackMap.erase(it);
        } else {
            ++it;
        }
    }
    UpdateAllDirtyBorder();

    UpdatePopulationMaxValues();
    GrowPopulation();
    IncreaseMoney();
}

void Player::UpdatePopulationMaxValues() {
    _maxTotalPopulation = 200; // min maxPopulation 200 (you are cooked if < 200)
    _maxTotalPopulation += _allPixels.size(); // +1 for each pixel
    _maxTotalPopulation += 1000 * _cities.size(); // +1000 for each city

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

    if (_id == 0) mySounds.Play(mySounds.cityBuildPool);

    _cities.emplace_back(pos);
    return true;
}

bool Player::TryAddSilo(Pixel *pos) {
    const int cost = siloCost * (_silos.size() + 1);

    if (_money.moneyBalance < cost || !CanBuildCity(pos)) return false;
    _money.spendMoney(cost);

    if (_id == 0) mySounds.Play(mySounds.cityBuildPool);

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

bool Player::CanLaunchAtomBomb() const {
    return !_silos.empty() && Bombs::atomBombCost < _money.moneyBalance;
}

bool Player::CanLaunchHydrogenBomb() const {
    return !_silos.empty() && Bombs::hydrogenBombCost < _money.moneyBalance;
}


void Player::TryLaunchAtomBomb(Pixel *targetPixel) {
    if (!CanLaunchAtomBomb() || targetPixel == nullptr) return;

    const int cost = Bombs::atomBombCost;

    _money.spendMoney(cost);
    mySounds.Play(mySounds.misslePool);

    Pixel *startPixel = GetNearestSiloFromPixel(targetPixel);
    Bombs::allBombs.push_back(SingleBomb{
        .targetPos = targetPixel->ToVector2(),
        .originPos = startPixel->ToVector2(),
        .pos = startPixel->ToVector2(),
        .radius = 50.f,
        .type = ATOM,
        .speed = 15.0
    });
}

void Player::TryLaunchHydrogenBomb(Pixel *targetPixel) {
    if (!CanLaunchHydrogenBomb() || targetPixel == nullptr) return;

    const int cost = Bombs::hydrogenBombCost;

    _money.spendMoney(cost);
    mySounds.Play(mySounds.misslePool);

    Pixel *startPixel = GetNearestSiloFromPixel(targetPixel);
    Bombs::allBombs.push_back(SingleBomb{
        .targetPos = targetPixel->ToVector2(),
        .originPos = startPixel->ToVector2(),
        .pos = startPixel->ToVector2(),
        .radius = 350.f,
        .type = HYDROGEN,
        .speed = 10.0
    });
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
