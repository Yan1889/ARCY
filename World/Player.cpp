#include "Player.h"
#include "Globals.h"

#include <cassert>
#include <iostream>
#include <cmath>
#include <variant>
#include <algorithm>

// Test
#include <random>
#include <stack>

#include "raylib.h"

Player::Player(Pixel startPos, int startRadius) {
    G::playerCount++;
    _id = G::playerCount;
    do {
        _color = Color{
            static_cast<unsigned char>(GetRandomValue(0, 255)), // Red
            static_cast<unsigned char>(GetRandomValue(0, 255)), // Green
            static_cast<unsigned char>(GetRandomValue(0, 255)), // Blue
            255 // Alpha
        };
    } while (_color.g > _color.r + 40 && _color.g > _color.b + 40);

    _money = Money();

    for (int x = startPos.x - startRadius; x < startPos.x + startRadius; x++) {
        const int dx = x - startPos.x;
        const int dh = std::sqrt(startRadius * startRadius - dx * dx);

        const int yMin = startPos.y - dh;
        const int yMax = startPos.y + dh;

        for (int y = yMin; y <= yMax; y++) {
            Pixel p = G::territoryMap[y][x];
            GetOwnershipOfPixel(x, y);
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
    int frontierSnapshotSize = _frontierPixels.size();
    std::unordered_set<Pixel, Pixel::Hasher> newPixels;

    static std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    for (int i = 0; i < frontierSnapshotSize && _peopleCurrentlyExploring > 0; i++) {
        auto neighborPixels = _frontierPixels[i].GetNeighborPixels();
        std::ranges::shuffle(neighborPixels, rng);

        for (const Pixel& newP : neighborPixels) {
            if (newP.playerId != 0) continue;

            Color terrain = static_cast<const Color *>(G::perlin.data)[G::perlin.width * newP.y + newP.x];
            float invasionP = GetInvasionAcceptP(terrain);

            if (invasionP == 0 || invasionP < dist(rng)) {
                _peopleCurrentlyExploring--;
                break;
            }

            if (_allPixels.contains(newP) || !newPixels.insert(newP).second) continue;

            _allPixels.insert(newP);
            GetOwnershipOfPixel(newP.x, newP.y);
            _frontierPixels.push_back(newP);
            _frontierSet.insert(newP);
            _peopleCurrentlyExploring--;
            break; // only one pixel per frontier per turn
        }
    }

    std::vector<Pixel> updatedFrontier;
    updatedFrontier.reserve(_frontierPixels.size());

    for (const Pixel& p : _frontierPixels) {
        if (IsFrontierPixel(p)) {
            updatedFrontier.push_back(p);
        } else {
            _frontierSet.erase(p);
        }
    }
    _frontierPixels = std::move(updatedFrontier);
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

void Player::GetOwnershipOfPixel(const int x, const int y) {
    G::territoryMap[y][x] = {x, y, _id};
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

float Player::GetInvasionAcceptP(const Color &terrainColor) {
    for (auto &mapPart: G::mapParts) {
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
