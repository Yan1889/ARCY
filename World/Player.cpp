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
    Expand(-1, 0.5);
}


void Player::Update() {
    // expand
    for (int i = 0; i < _allOnGoingAttackQueues.size(); i++) {
        ProcessAttackQueue(i);
    }

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

void Player::Expand(const int target, const float percentage) {
    const int newPeopleLeaving = _population * percentage;
    _population -= newPeopleLeaving;

    // ----- init/find an attack queue -----
    int queueIdx;

    auto iter = _attackedPlayerIdToQueueIdxMap.find(target);
    if (iter == _attackedPlayerIdToQueueIdxMap.end()) {
        queueIdx = _allOnGoingAttackQueues.size();
        _allOnGoingAttackQueues.push_back({target, {}});
        _peopleWorkingOnAttack.push_back(newPeopleLeaving);
        _attackedPlayerIdToQueueIdxMap[target] = queueIdx;
    } else {
        queueIdx = iter->second;
        _peopleWorkingOnAttack[queueIdx] += newPeopleLeaving;
    }
    // ----- fill attack queue -----
    for (Pixel *borderPixel: _borderPixels) {
        for (Pixel *potentialEnemyBorderPixel: borderPixel->GetNeighbors()) {
            if (potentialEnemyBorderPixel->playerId == target) {
                if (_peopleWorkingOnAttack[queueIdx] <= 0) return;

                if (potentialEnemyBorderPixel->queuedUpForAttack) continue;

                const float priority = GetPriorityOfPixel(
                    potentialEnemyBorderPixel,
                    _allOnGoingAttackQueues[queueIdx].targetPlayerId
                );
                if (priority == 0) continue; // skip this impossible pixel

                _allOnGoingAttackQueues[queueIdx].queue.push({
                    priority,
                    potentialEnemyBorderPixel
                });
                potentialEnemyBorderPixel->queuedUpForAttack = true;
                _peopleWorkingOnAttack[queueIdx]--;
            }
        }
    }
}


void Player::ProcessAttackQueue(const int queueIdx) {
    auto &queueToWorkOn = _allOnGoingAttackQueues[queueIdx].queue;

    // 60fps => ~10 broder expansions / 1s
    const int attackPixelCount = queueToWorkOn.size() / 2;

    for (int i = 0; i < attackPixelCount; i++) {
        Pixel *newP = queueToWorkOn.top().second;
        queueToWorkOn.pop();
        newP->queuedUpForAttack = false;

        if (newP->playerId != _allOnGoingAttackQueues[queueIdx].targetPlayerId) {
            _peopleWorkingOnAttack[queueIdx]++;
            continue;
        }
        GetOwnershipOfPixel(newP);

        // update attack queue
        const auto &neighbors = newP->GetNeighbors();
        for (Pixel *neighbor: neighbors) {
            if (_peopleWorkingOnAttack[queueIdx] <= 0) break; // no new Pixels

            if (neighbor->playerId == _id || neighbor->queuedUpForAttack) continue;

            const float priority = GetPriorityOfPixel(neighbor, _allOnGoingAttackQueues[queueIdx].targetPlayerId);
            if (priority == 0) continue; // water or mountain are impossible

            neighbor->queuedUpForAttack = true;
            queueToWorkOn.push({
                priority,
                neighbor
            });
            _peopleWorkingOnAttack[queueIdx]--;
        }
    }
}

void Player::GetOwnershipOfPixel(Pixel *newP) {
    Player &attacker = *this;
    attacker._allPixels.insert(newP);

    if (newP->playerId != -1) {
        Player &defender = G::players[newP->playerId];
        defender.LoseOwnershipOfPixel(newP, false);
    }
    newP->playerId = _id;

    attacker.UpdateBorderAroundPixel(newP);


    G::ChangeColorOfPixel(newP, _color);

    // center
    AddPixelToCenter(newP);
}

void Player::UpdateBorderAroundPixel(Pixel *pixel) {
    UpdateBorderStatusOfPixel(pixel);

    const std::vector<Pixel *> &affectedPixels = pixel->GetNeighbors();
    for (Pixel *neighbor: affectedPixels) {
        UpdateBorderStatusOfPixel(neighbor);
    }
}

void Player::UpdateBorderStatusOfPixel(Pixel *pixel) {
    if (!_allPixels.contains(pixel)) {
        // pixel isn't owned by player
        RemoveBorderPixel(pixel);
        return;
    }

    const bool wasBorderPixel = _borderSet.contains(pixel);
    bool nowBorderPixel = false;
    for (Pixel *nn: pixel->GetNeighbors()) {
        if (nn->playerId != _id) {
            nowBorderPixel = true;
            break;
        }
    }
    if (nowBorderPixel && !wasBorderPixel) {
        AddBorderPixel(pixel);
    } else if (!nowBorderPixel && wasBorderPixel) {
        RemoveBorderPixel(pixel);
    }
}

void Player::LoseOwnershipOfPixel(Pixel * pixel, bool updateTextureToo) {
    _allPixels.erase(pixel);

    pixel->playerId = -1;

    UpdateBorderAroundPixel(pixel);
    RemovePixelFromCenter(pixel);

    if (updateTextureToo) {
        G::ChangeColorOfPixel(pixel, BLANK);
    }
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

void Player::IncreaseMoney() {
    float currentTime = GetTime();

    if (currentTime - _lastActionTime >= _cooldownTime) {
        int peopleAddition = 2;
        int totalAddition = peopleAddition * _population;
        _money.moneyBalance += totalAddition;

        _lastActionTime = currentTime;
    }
}

float Player::GetPriorityOfPixel(Pixel *p, const int targetId) const {
    if (p->playerId != targetId) return 0;

    float priority = GetInvasionAcceptP(p->GetColor());
    if (priority == 0) return 0; // impossible

    priority += rand() / static_cast<float>(RAND_MAX) / 2; // some randomness
    return priority;
}


float Player::GetInvasionAcceptP(const Color &terrainColor) {
    for (auto &mapPart: G::mapParts) {
        if (terrainColor.r == mapPart.color.r) {
            return mapPart.difficulty;
        }
    }
    return -1;
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
