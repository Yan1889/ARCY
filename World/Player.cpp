#include "Player.h"
#include "Globals.h"

#include <cassert>
#include <iostream>
#include <cmath>
#include <variant>
#include <algorithm>

// Test
#include <stack>

#include "raylib.h"


Player::Player(Pixel startPos, int startRadius, Image &perlin): _bgImage(perlin) {
    G::playerCount++;

    _money = Money();

    for (int x = startPos.x - startRadius; x < startPos.x + startRadius; x++) {
        const int dx = x - startPos.x;
        const int dh = std::sqrt(startRadius * startRadius - dx * dx);

        const int yMin = startPos.y - dh;
        const int yMax = startPos.y + dh;

        for (int y = yMin; y <= yMax; y++) {
            Pixel p = G::territoryMap[y][x];
            p.playerId = G::playerCount; // 0 = not occupied
            G::SetPixelOnTerritory(x, y, p, ORANGE);
            _allPixels.insert(p);
        }
    }
    UpdateFrontier();
}


void Player::Update() {
    // expand
    if (_peopleCurrentlyExploring > 0) {
        ExpandOnceOnAllFrontierPixels();
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

void Player::Expand(const float percentage) {
    const int peopleAllowedToLeave = _maxPeopleExploring - _peopleCurrentlyExploring;
    const int newPeopleLeaving = std::min(peopleAllowedToLeave, static_cast<int>(_population * percentage));

    _peopleCurrentlyExploring += newPeopleLeaving;
    _population -= newPeopleLeaving;
}


void Player::ExpandOnceOnAllFrontierPixels() {
    std::vector<Pixel> expansionFrontier = _frontierPixels; // snapshot
    std::unordered_set<Pixel, Pixel::Hasher> newPixels;

    while (_peopleCurrentlyExploring > 0 && !expansionFrontier.empty()) {
        const int randIdx = rand() % expansionFrontier.size();
        const Pixel &randomFrontierPixel = expansionFrontier[randIdx];

        const std::vector<Pixel> neighborPixels = randomFrontierPixel.GetNeighborPixels();
        int randNeighborIdx = rand() % neighborPixels.size();

        const Pixel &newP = neighborPixels[randNeighborIdx];

        Color terrainOnPixel = static_cast<const Color *>(_bgImage.data)[_bgImage.width * newP.y + newP.x];

        const float invasionAcceptP = GetInvasionAcceptP(terrainOnPixel);

        if (invasionAcceptP == 0 || invasionAcceptP < static_cast<float>(rand()) / RAND_MAX) {
            expansionFrontier.erase(expansionFrontier.begin() + randIdx);
            continue;
        }
        if (!_allPixels.contains(newP) && !newPixels.contains(newP)) {
            newPixels.insert(newP);
            _allPixels.insert(newP);
            G::SetPixelOnTerritory(newP.x, newP.y, newP, ORANGE);
            _frontierPixels.push_back(newP);
            _frontierSet.insert(newP);
            _peopleCurrentlyExploring--;

            for (const Pixel &potentialInvalidFrontierP: newP.GetNeighborPixels()) {
                if (_frontierSet.contains(potentialInvalidFrontierP)) {
                    if (!IsFrontierPixel(potentialInvalidFrontierP)) {
                        std::erase(_frontierPixels, potentialInvalidFrontierP);
                        _frontierSet.erase(potentialInvalidFrontierP);
                    }
                }
            }

            if (_peopleCurrentlyExploring == 0) break;
        }
        expansionFrontier.erase(expansionFrontier.begin() + randIdx);
    }

    for (auto iter = _frontierPixels.begin(); iter != _frontierPixels.end();) {
        if (!IsFrontierPixel(*iter)) {
            _frontierSet.erase(*iter);
            iter = _frontierPixels.erase(iter);
        } else {
            ++iter;
        }
    }
}

bool Player::IsFrontierPixel(const Pixel &p) const {
    for (const Pixel &n: p.GetNeighborPixels()) {
        if (!_allPixels.contains(n)) {
            return true;
        }
    }
    return false;
}

void Player::UpdateFrontier() {
    std::cout << "bad" << std::endl;
    _frontierPixels.clear();
    _frontierSet.clear();

    for (const Pixel p: _allPixels) {
        const std::vector<Pixel> neighbors = p.GetNeighborPixels();

        for (const Pixel &n: neighbors) {
            if (!_allPixels.contains(n)) {
                _frontierPixels.push_back(p);
                _frontierSet.insert(p);
                break;
            }
        }
    }
}
void Player::IncreaseMoney() {
    float currentTime = GetTime();

    if (currentTime - lastActionTime >= cooldownTime) {
        int peopleAddition = 2;
        int totalAddition = peopleAddition * _population;
        _money.moneyBalance += totalAddition;

        lastActionTime = currentTime;
    }
}

float Player::GetInvasionAcceptP(const Color &terrainColor) {
    for (auto & mapPart : G::mapParts) {
        if (terrainColor.r == mapPart.color.r) {
            return mapPart.difficulty;
        }
    }
    return -1;
}

void Player::AddCity(const Vector2 pos) {
    _cityCount++;
    _cityPositions.push_back(pos);
}
