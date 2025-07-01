#include "Player.h"

#include <cassert>
#include <iostream>
#include <cmath>
#include <variant>
#include <algorithm>

#include "raylib.h"


Player::Player(const Pixel startPos, const int startRadius) {
    for (int x = startPos.x - startRadius; x < startPos.x + startRadius; x++) {
        const int dx = x - startPos.x;
        const int dh = std::sqrt(startRadius * startRadius - dx * dx);

        const int yMin = startPos.y - dh;
        const int yMax = startPos.y + dh;

        for (int y = yMin; y <= yMax; y++) {
            _allPixels.insert({x, y});
        }
    }
    UpdateFrontier();
}


void Player::Update() {
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
            _population += static_cast<int>(ceil(totalGrowth)); // ich glaube ich spinne @Colin: (int) ts ts ts
        }

        if (_population > _maxPopulation) {
            _population = _maxPopulation;
        }

        _growthCooldown = _growthRate;
    }

    // expand
    if (_peopleCurrentlyExploring > 0) {
        ExpandOnceOnAllFrontierPixels();
    }
}

void Player::Expand(const float percentage) {
    _peopleCurrentlyExploring += static_cast<int>(_population * percentage);
    _population -= _peopleCurrentlyExploring;
}

void Player::ExpandOnceOnAllFrontierPixels() {
    std::vector<Pixel> expansionFrontier = _frontierPixels; // snapshot
    std::unordered_set<Pixel, Pixel::Hasher> newPixels;

    while (_peopleCurrentlyExploring > 0 && !expansionFrontier.empty()) {
        const int randIdx = rand() % expansionFrontier.size();
        const Pixel& randomFrontierPixel = expansionFrontier[randIdx];

        for (const Pixel& newP : randomFrontierPixel.GetNeighborPixels()) {
            if (!_allPixels.contains(newP) && !newPixels.contains(newP)) {
                newPixels.insert(newP);
                _allPixels.insert(newP);
                _frontierPixels.push_back(newP);
                _frontierSet.insert(newP);
                _peopleCurrentlyExploring--;

                for (const Pixel& potentialInvalidFrontierP : newP.GetNeighborPixels()) {
                    if (_frontierSet.contains(potentialInvalidFrontierP)) {
                        if (!IsFrontierPixel(potentialInvalidFrontierP)) {
                            std::erase(_frontierPixels, potentialInvalidFrontierP);
                            _frontierSet.erase(potentialInvalidFrontierP);
                        }
                    }
                }

                if (_peopleCurrentlyExploring == 0) break;
            }
        }
        expansionFrontier.erase(expansionFrontier.begin() + randIdx);
    }
}

bool Player::IsFrontierPixel(const Pixel& p) const {
    for (const Pixel& n : p.GetNeighborPixels()) {
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

void Player::AddCity(const Vector2 pos) {
    _cityCount++;
    _cityPositions.push_back(pos);
}
