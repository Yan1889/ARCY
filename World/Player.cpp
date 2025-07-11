#include "Player.h"
#include "Globals.h"
#include "Pixel.h"

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
    _centerPixel = Pixel(startPos.x, startPos.y, G::playerCount);
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
        queueIdx = _allOnGoingAttackQueues.size() - 1;
    } else {
        _peopleWorkingOnAttack[queueIdx] += newPeopleLeaving;
    }
    // ----- fill attack queue -----
    for (const Pixel &borderPixel: _borderPixels) {
        for (const Pixel &potentialEnemyBorderPixel: borderPixel.GetNeighborPixels()) {
            if (potentialEnemyBorderPixel.playerId == target) {
                if (_peopleWorkingOnAttack[queueIdx] <= 0) return;

                if (_pixelsQueuedUp.contains(potentialEnemyBorderPixel)) continue;

                _pixelsQueuedUp.insert(potentialEnemyBorderPixel);
                _allOnGoingAttackQueues[queueIdx].second.push({
                    GetPriorityOfPixel(potentialEnemyBorderPixel.x, potentialEnemyBorderPixel.y),
                    potentialEnemyBorderPixel
                });
                _peopleWorkingOnAttack[queueIdx]--;
            }
        }
    }
}


void Player::ProcessAttackQueue(const int queueIdx) {
    auto &queueToWorkOn = _allOnGoingAttackQueues[queueIdx].second;

    const int attackPixelCount = queueToWorkOn.size() / 2;
    for (int i = 0; i < attackPixelCount; i++) {
        const Pixel newP = queueToWorkOn.top().second;
        queueToWorkOn.pop();
        GetOwnershipOfPixel(newP.x, newP.y);


        // update attack queue
        const auto &neighbors = newP.GetNeighborPixels();
        for (const Pixel &neighbor: neighbors) {
            if (_peopleWorkingOnAttack[queueIdx] <= 0) break; // no new Pixels

            if (neighbor.playerId != _id) {
                if (_pixelsQueuedUp.contains(neighbor)) continue;

                _pixelsQueuedUp.insert(neighbor);
                queueToWorkOn.push({
                    GetPriorityOfPixel(neighbor.x, neighbor.y),
                    neighbor
                });
                _peopleWorkingOnAttack[queueIdx]--;
            }
        }
    }
}

void Player::GetOwnershipOfPixel(const int x, const int y) {
    G::territoryMap[y][x] = {x, y, _id};
    const Pixel &newP = G::territoryMap[y][x];
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
        _id
    };

    _borderPixels.push_back(newP);
    const std::vector<Pixel> neighbors = newP.GetNeighborPixels();
    // update border status of neighbors
    for (const Pixel &neighbor: neighbors) {
        bool wasBorderPixel = _borderSet.contains(neighbor);
        bool nowBorderPixel = false;
        for (const Pixel &nn: neighbor.GetNeighborPixels()) {
            if (nn.playerId != _id) {
                nowBorderPixel = true;
                _borderPixels.push_back(neighbor);
                break;
            }
        }
        if (nowBorderPixel && !wasBorderPixel) {
            _borderPixels.push_back(newP);
        } else if (!nowBorderPixel && wasBorderPixel) {
            _borderPixels.erase(std::ranges::find(_borderPixels, newP));
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

float Player::GetPriorityOfPixel(int x, int y) const {
    const Color terrainColor = static_cast<const Color *>
            (G::perlin.data)[G::perlin.width * y + x];

    float priority = GetInvasionAcceptP(terrainColor);
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
    _cityPositions.push_back(Pixel(pos));
}

void Player::AddCity(const Pixel &pos) {
    _cityCount++;
    _cityPositions.push_back(pos);
}
