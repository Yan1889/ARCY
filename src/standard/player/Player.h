//
// Created by minhp on 19.06.2025.
//

#ifndef POPULATION_H
#define POPULATION_H
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Money.h"
#include "raylib.h"
#include "../map/Pixel.h"
#include "../Building.h"


struct Attack {
    int targetPlayerId;
    int troops{};
    std::queue<Pixel *> queue{};
    std::unordered_set<Pixel *> set{};
};

class Player {
public:
    int _id;
    Color _color{};

    bool _dead{};

    // population
    int _population = 1000;
    int _maxPopulation{};
    float _growthCooldown = 100;
    float _growthRate = 100;
    float _growth{};
    float _growthFactor = 0.0085f;

    // buildings
    std::vector<Pixel *> _cities;
    std::vector<Pixel *> _silos;


    // money
    float _cooldownTime = 1.0f;
    float _lastActionTime;
    Money _money;

    // territory
    std::unordered_set<Pixel *> _allPixels;
    std::vector<Pixel *> _border_vec;
    std::unordered_set<Pixel *> _border_set;
    std::vector<Pixel *> _dirtyBorderPixels_vec;
    std::unordered_set<Pixel *> _dirtyBorderPixels_set;

    // attack
    std::unordered_map<int, Attack> _targetToAttackMap;

    int _allPixelsSummed_x{};
    int _allPixelsSummed_y{};
    int _centerPixel_x{};
    int _centerPixel_y{};

    Player(Pixel* startPos, int startRadius);

    void Update();
    void Expand(int target, float percentage);

    void ReFillAttackQueueFromScratch(Attack& attack);
    void ProcessAttackQueue(Attack& attack);
    void GetOwnershipOfPixel(Pixel* newP);
    std::vector<Building> LoseOwnershipOfPixel(Pixel * pixel, bool updateTextureToo);

    void MarkAsDirty(Pixel* p);
    void UpdateAllDirtyBorder();
    void UpdateBorderSingle(Pixel* pixel);
    void AddBorderPixel(Pixel* pixel);
    void RemoveBorderPixel(Pixel* pixel);

    void BotLogic();
    void BotLogic_Bombing();
    void BotLogic_Building();
    void BotLogic_Expanding();
    void GrowPopulation();
    void IncreaseMoney();
    void RemovePixelFromCenter(Pixel* newP);
    void AddPixelToCenter(Pixel* newP);
    bool TryAddCity(Pixel* pos);
    bool TryAddSilo(Pixel* pos);
    bool CanBuildCity(Pixel* pos) const;
    bool CanBuildSilo(Pixel* pos) const;
    Pixel* GetNearestSiloFromPixel(Pixel* point) const;
};


#endif //POPULATION_H
