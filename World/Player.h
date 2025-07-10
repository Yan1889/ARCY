//
// Created by minhp on 19.06.2025.
//

#ifndef POPULATION_H
#define POPULATION_H
#include <unordered_set>
#include <vector>

#include "Money.h"
#include "Pixel.h"
#include "raylib.h"



class Player {
public:
    int _id;
    Color _color{};

    // population
    int _population = 100;
    int _maxPopulation{};
    float _growthCooldown = 100;
    float _growthRate = 100;
    int _cityCount{};
    std::vector<Vector2> _cityPositions;
    float _growth{};
    float _growthFactor = 0.0085f;

    int _peopleCurrentlyExploring{};
    int _maxPeopleExploring = 10000;

    // money
    float _cooldownTime{};
    float _lastActionTime{};
    Money _money;

    // territory
    std::unordered_set<Pixel, Pixel::Hasher> _allPixels;
    std::vector<Pixel> _frontierPixels;
    std::unordered_set<Pixel, Pixel::Hasher> _frontierSet;
    Pixel _allPixelsSummed;
    Pixel _centerPixel;


    Player(Pixel startPos, int startRadius);

    void Expand(float percentage);

    void ExpandOnceOnAllFrontierPixels();

    void UpdateFrontier();

    bool IsFrontierPixel(const Pixel& p) const;

    static float GetInvasionAcceptP(const Color& terrainColor);

    void GetOwnershipOfPixel(int x, int y);

    void Update();
    void GrowPopulation();
    void IncreaseMoney();
    void AddCity(Vector2 pos);
};


#endif //POPULATION_H
