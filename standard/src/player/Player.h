//
// Created by minhp on 19.06.2025.
//

#ifndef POPULATION_H
#define POPULATION_H
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Money.h"
#include "raylib.h"
#include "../map/Pixel.h"
#include "../map/Buildings.h"

using namespace Buildings;

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
    std::string _name;

    bool _dead{};
    bool _bot{};

    // population
    int _maxTotalPopulation{};
    int _totalPopulation{};
    int _maxTroops{};
    int _troops{};
    int _maxWorkers{};
    int _workers{};
    float _troopPercentage = 0.80f;

    // buildings
    std::vector<Building> _buildings{};

    // money
    float _cooldownTime = 1.0f;
    float _lastActionTime;
    Money _money;

    // territory
    int _pixelCount{};
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

    Player(Pixel* startPos, const std::string& name);

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
    void UpdatePopulationMaxValues();
    bool Borders(int otherId) const;
    void GrowPopulation();
    void IncreaseMoney();
    void RemovePixelFromCenter(Pixel* newP);
    void AddPixelToCenter(Pixel* newP);
    bool TryAddBuilding(BUILDING_TYPE t, Pixel* pos);
    bool CanBuildType(BUILDING_TYPE t, Pixel *pos);
    std::vector<Building *> GetBuildingsOfType(BUILDING_TYPE t);
    Pixel* GetNearestSiloFromPixel(Pixel* point);

    bool CanLaunchBomb(Pixel* pixel, int cost);
    void TryLaunchBomb(Pixel *target, bool isAtom);
};


#endif //POPULATION_H
