#include "Player.h"
#include "Globals.h"
#include "Map/Pixel.h"

#include <cassert>
#include <iostream>
#include <cmath>
#include <variant>
#include <algorithm>

// Test
#include <random>
#include <stack>

#include "raylib.h"

Player::Player(const Pixel startPos, const int startRadius): _id(++G::playerCount) {
    _centerPixel = {startPos.x, startPos.y};
    _allPixelsSummed = _centerPixel;

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

    GetOwnershipOfPixel(_centerPixel.x, _centerPixel.y);
    Expand(0, 0.5);
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
    int queueIdx = -1;
    for (int i = 0; i < _allOnGoingAttackQueues.size(); i++) {
        if (target == _allOnGoingAttackQueues[i].first) {
            // the player is already being attacked
            queueIdx = i;
            break;
        }
    }
    if (queueIdx == -1) {
        _allOnGoingAttackQueues.push_back({target, {}});
        _peopleWorkingOnAttack.push_back(newPeopleLeaving);
        _pixelsQueuedUp.push_back({});
        queueIdx = _allOnGoingAttackQueues.size() - 1;
    } else {
        _peopleWorkingOnAttack[queueIdx] += newPeopleLeaving;
    }
    // ----- fill attack queue -----
    for (const PixelRef &borderPixel: _borderPixels) {
        for (const PixelRef &potentialEnemyBorderPixel: borderPixel.GetNeighborPixels()) {
            if (potentialEnemyBorderPixel.GetPlayerId() == target) {
                if (_peopleWorkingOnAttack[queueIdx] <= 0) return;

                if (_pixelsQueuedUp[queueIdx].contains(potentialEnemyBorderPixel)) continue;

                const float priority = GetPriorityOfPixel(potentialEnemyBorderPixel.x, potentialEnemyBorderPixel.y);
                if (priority == 0) continue; // water or mountains are impossible

                _allOnGoingAttackQueues[queueIdx].second.push({
                    priority,
                    potentialEnemyBorderPixel
                });
                _pixelsQueuedUp[queueIdx].insert(potentialEnemyBorderPixel);
                _peopleWorkingOnAttack[queueIdx]--;
            }
        }
    }
}


void Player::ProcessAttackQueue(const int queueIdx) {
    auto &queueToWorkOn = _allOnGoingAttackQueues[queueIdx].second;

    // 60fps => ~10 broder expansions / 1s
    const int attackPixelCount = queueToWorkOn.size() / 6;

    for (int i = 0; i < attackPixelCount; i++) {
        const PixelRef newP = queueToWorkOn.top().second;
        queueToWorkOn.pop();
        GetOwnershipOfPixel(newP.x, newP.y);
        _pixelsQueuedUp[queueIdx].erase(newP);

        // update attack queue
        const auto &neighbors = newP.GetNeighborPixels();
        for (const PixelRef &neighbor: neighbors) {
            if (_peopleWorkingOnAttack[queueIdx] <= 0) break; // no new Pixels

            if (neighbor.GetPlayerId() == _id || _pixelsQueuedUp[queueIdx].contains(neighbor)) continue;

            const float priority = GetPriorityOfPixel(neighbor.x, neighbor.y);
            if (priority == 0) continue; // water or mountain are impossible

            _pixelsQueuedUp[queueIdx].insert(neighbor);
            queueToWorkOn.push({
                priority,
                neighbor
            });
            _peopleWorkingOnAttack[queueIdx]--;
        }
    }
}

void Player::GetOwnershipOfPixel(const int x, const int y) {
    G::territoryMap[y][x] = {x, y, _id};
    const PixelRef newP = {x, y};
    _allPixels.insert(newP);

    // texture
    static_cast<Color *>(G::territoryImage.data)[y * G::WIDTH + x] = _color;
    const Color buffer[]{_color};
    UpdateTextureRec(
        G::territoryTexture,
        Rectangle{
            static_cast<float>(x),
            static_cast<float>(y),
            1,
            1
        },
        buffer
    );

    // center
    _allPixelsSummed += newP;
    _centerPixel = {
        _allPixelsSummed.x / static_cast<int>(_allPixels.size()),
        _allPixelsSummed.y / static_cast<int>(_allPixels.size()),
    };

    std::vector<PixelRef> affectedPixels = newP.GetNeighborPixels();
    affectedPixels.push_back(newP);

    for (const PixelRef &p: affectedPixels) {
        bool wasBorder = _borderSet.contains(p);
        bool isNowBorder = false;

        for (const PixelRef &n: p.GetNeighborPixels()) {
            if (n.GetPlayerId() != _id) {
                isNowBorder = true;
                break;
            }
        }

        if (isNowBorder && !wasBorder) {
            _borderSet.insert(p);
            _borderPixels.push_back(p);
        } else if (!isNowBorder && wasBorder) {
            _borderSet.erase(p);

            auto it = std::ranges::find(_borderPixels, p);
            if (it != _borderPixels.end()) {
                _borderPixels.erase(it);
            }
        }
    }


    /*
    std::vector<PixelRef> affectedPixels = newP.GetNeighborPixels();
    affectedPixels.push_back(newP);
    // update border status of neighbors
    for (const PixelRef &neighbor: affectedPixels) {
        bool wasBorderPixel = _borderSet.contains(neighbor);
        bool nowBorderPixel = false;
        for (const PixelRef &nn: neighbor.GetNeighborPixels()) {
            if (nn.GetPlayerId() != _id) {
                nowBorderPixel = true;
                break;
            }
        }
        if (nowBorderPixel && !wasBorderPixel) {
            _borderPixels.push_back(neighbor);
            _borderSet.insert(neighbor);
        } else if (!nowBorderPixel && wasBorderPixel) {
            _borderPixels.erase(std::ranges::find(_borderPixels, neighbor));
            _borderSet.erase(neighbor);
        }
    }
    */
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

float Player::GetPriorityOfPixel(int x, int y) const {
    float priority = GetInvasionAcceptP(PixelRef{x, y}.GetColor());
    if (priority == 0) return 0;

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

void Player::AddCity(const Vector2 &pos) {
    _cityCount++;
    _cityPositions.push_back(PixelRef{
        static_cast<int>(pos.x),
        static_cast<int>(pos.y)
    });
}

void Player::AddCity(const PixelRef &pos) {
    _cityCount++;
    _cityPositions.push_back(pos);
}
