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

    // Test
    _territoryTexture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    _territoryDirty = true;
}


void Player::Update() {
    // expand
    if (_peopleCurrentlyExploring > 0) {
        ExpandOnceOnAllFrontierPixels();
    }


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

        const float difficulty = GetDifficulty(terrainOnPixel);

        if (difficulty == -1 || difficulty / G::maxDifficulty > static_cast<float>(rand()) / RAND_MAX) {
            expansionFrontier.erase(expansionFrontier.begin() + randIdx);
            continue;
        }
        if (!_allPixels.contains(newP) && !newPixels.contains(newP)) {
            newPixels.insert(newP);
            _allPixels.insert(newP);
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

    // Test
    _territoryDirty = true;
}

// Test
float Cross(const Pixel& O, const Pixel& A, const Pixel& B) {
    return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
}

std::vector<Pixel> Player::GetConvexHull(const std::vector<Pixel>& points) {
    std::vector<Pixel> sorted = points;
    std::sort(sorted.begin(), sorted.end(), [](const Pixel& a, const Pixel& b) {
        return a.x < b.x || (a.x == b.x && a.y < b.y);
    });

    std::vector<Pixel> hull;

    for (int phase = 0; phase < 2; ++phase) {
        int start = hull.size();
        for (const Pixel& p : sorted) {
            while (hull.size() >= start + 2 &&
                   Cross(hull[hull.size() - 2], hull[hull.size() - 1], p) <= 0) {
                hull.pop_back();
                   }
            hull.push_back(p);
        }
        std::reverse(sorted.begin(), sorted.end());
    }

    hull.pop_back(); // letzter Punkt == erster
    return hull;
}

void Player::DrawTerritory() const {
    if (_frontierPixels.size() < 3) return;

    std::vector<Vector2> polygon;

    for (const Pixel& p : _frontierPixels) {
        polygon.push_back({ (float)p.x, (float)p.y });
    }

    float minX = polygon[0].x, maxX = polygon[0].x;
    float minY = polygon[0].y, maxY = polygon[0].y;

    for (const Vector2& p : polygon) {
        if (p.x < minX) minX = p.x;
        if (p.x > maxX) maxX = p.x;
        if (p.y < minY) minY = p.y;
        if (p.y > maxY) maxY = p.y;
    }

    for (int y = (int)minY; y <= (int)maxY; y++) {
        for (int x = (int)minX; x <= (int)maxX; x++) {
            Vector2 pt = { (float)x + 0.5f, (float)y + 0.5f };
            if (CheckCollisionPointPoly(pt, polygon.data(), polygon.size())) {
                DrawPixel(x, y, RED);
            }
        }
    }
}

void Player::SortFrontierPixels() {
    if (_frontierPixels.size() < 3) return;

    float centerX = 0;
    float centerY = 0;
    for (const Pixel& p : _frontierPixels) {
        centerX += p.x;
        centerY += p.y;
    }
    centerX /= _frontierPixels.size();
    centerY /= _frontierPixels.size();

    std::sort(_frontierPixels.begin(), _frontierPixels.end(),
              [centerX, centerY](const Pixel& a, const Pixel& b) {
                  float angleA = atan2(a.y - centerY, a.x - centerX);
                  float angleB = atan2(b.y - centerY, b.x - centerX);
                  return angleA < angleB;
              });
}

void Player::UpdateTerritoryTexture() {
    if (!_territoryDirty) return;

    BeginTextureMode(_territoryTexture);
    ClearBackground(BLANK); // Vollständig transparent oder weiß

    if (_frontierPixels.size() >= 3) {
        std::vector<Pixel> hull = GetConvexHull(_frontierPixels);

        // Umwandeln zu Vector2
        std::vector<Vector2> polygon;
        for (const Pixel& p : hull) {
            polygon.push_back(Vector2{static_cast<float>(p.x), static_cast<float>(p.y)});
        }

        if (polygon.size() >= 3) {
            DrawTriangleFan(polygon.data(), polygon.size(), RED);
        }
    }

    EndTextureMode();

    _territoryDirty = false;
}

void Player::DrawTerritoryTexture() const {
    DrawTextureRec(
        _territoryTexture.texture,
        {0, 0, (float)_territoryTexture.texture.width, -(float)_territoryTexture.texture.height},
        {0, 0},
        WHITE
    );
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

float Player::GetDifficulty(const Color &terrainColor) {
    for (int i = 0; i < G::map.size(); ++i) {
        if (terrainColor.r == G::map[i].color.r) {
            return G::map[i].difficulty;
        }
    }
    std::cerr << "error" << std::endl;
    return 0;
}

void Player::AddCity(const Vector2 pos) {
    _cityCount++;
    _cityPositions.push_back(pos);
}
