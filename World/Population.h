//
// Created by minhp on 19.06.2025.
//

#ifndef POPULATION_H
#define POPULATION_H



class Population {
public:
    static void Update();
    static void Start();
    static void GrowPopulation();

    static int population;
    static int maxPopulation;
    static float growthCooldown;
    static float growthRate;
    static int cities;
    static float growth;
    static float growthFactor;
};



#endif //POPULATION_H
