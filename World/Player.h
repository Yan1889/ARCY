//
// Created by minhp on 19.06.2025.
//

#ifndef POPULATION_H
#define POPULATION_H
#include <set>
#include <unordered_set>
#include <vector>

#include "raylib.h"

struct Pixel {
    int x, y;

    std::set<Pixel> GetNeighborPixels() const {
        std::set<Pixel> result;
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                if (dx == 0 && dy == 0) continue;

                result.insert({x + dx, y + dy});
            }
        }
        return result;
    }

    bool operator<(const Pixel& other) const {
        return std::tie(x, y) < std::tie(other.x, other.y);
    }
    bool operator==(const Pixel& other) const {
        return x == other.x && y == other.y;
    }

    struct Hasher {
        std::size_t operator()(const Pixel& p) const {
            return std::hash<int>()(p.x) ^ (std::hash<int>()(p.y) << 1);
        }
    };
};



class Player {
public:
    // population
    int _population = 100;
    int _maxPopulation{};
    float _growthCooldown = 100;
    float _growthRate = 100;
    int _cityCount{};
    std::vector<Vector2> _cityPositions;
    float _growth{};
    float _growthFactor = 0.0085f;

    // territory
    std::unordered_set<Pixel, Pixel::Hasher> _allPixels;
    std::vector<Pixel> _frontierPixels;

    explicit Player(Pixel startPos, int startRadius);

    void Expand(float percentage);

    void ExpandOnceOnAllFrontierPixels(int& totalSend, int maxPeople);

    void UpdateFrontier();


    void Update();
    void GrowPopulation();
    void AddCity(Vector2 pos);
};


#endif //POPULATION_H
