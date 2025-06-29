#include "Player.h"

#include <cassert>
#include <iostream>
#include <cmath>
#include <variant>

#include "raylib.h"

/*
int Population::maxPopulation;
int Population::population = 100;
float Population::growth;
float Population::growthFactor = 0.0085f;
float Population::growthRate = 100.0f;
float Population::growthCooldown = growthRate;
int Population::cities;
*/

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

    _growth = _growthFactor * (1.0f - exp(-_population / 10));

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
}

void Player::Expand(const float percentage) {
    const int peopleSend_limit = static_cast<int>(_population * percentage);
    int peopleSend = 0;
    while (peopleSend < peopleSend_limit) {
        ExpandOnceOnAllFrontierPixels(peopleSend, peopleSend_limit);
    }
    _population -= peopleSend;
}

void Player::ExpandOnceOnAllFrontierPixels(int& totalSend, const int maxPeople) {
    std::vector<Pixel> expansionFrontier = _frontierPixels; // snapshot
    std::set<Pixel> newPixels;

    while (totalSend < maxPeople && !expansionFrontier.empty()) {
        const int randIdx = rand() % expansionFrontier.size();
        auto iter = expansionFrontier.begin();
        std::advance(iter, randIdx);
        const Pixel& randomFrontierPixel = *iter;

        for (const Pixel& n : randomFrontierPixel.GetNeighborPixels()) {
            if (!_allPixels.contains(n) && !newPixels.contains(n)) {
                newPixels.insert(n);
                totalSend++;
                if (totalSend == maxPeople) break;
            }
        }

        expansionFrontier.erase(iter);
    }

    _allPixels.insert(newPixels.begin(), newPixels.end());
    UpdateFrontier();
}

void Player::UpdateFrontier() {
    _frontierPixels.clear();

    for (const Pixel p: _allPixels) {
        const std::set<Pixel> neighbors = p.GetNeighborPixels();

        for (const Pixel &n: neighbors) {
            if (!_allPixels.contains(n)) {
                _frontierPixels.push_back(p);
                break;
            }
        }
    }
}

void Player::AddCity(const Vector2 pos) {
    _cityCount++;
    _cityPositions.push_back(pos);
}
