//
// Created by minhp on 19.06.2025.
//

#include "Population.h"

#include <iostream>
#include <bits/ostream.tcc>
#include <cmath>
#include <variant>

#include "raylib.h"

int Population::maxPopulation;
int Population::population;
float Population::growth;
float Population::growthFactor;
float Population::growthRate;
float Population::growthCooldown ;
int Population::cities;

void Population::Start()
{
    population = 100;
    growthFactor = 0.0085f;
    growthRate = 100.0f;
    growthCooldown = growthRate;
}

void Population::Update()
{
    growthCooldown -= GetTime();
    maxPopulation = 1000 + 1000 * cities;

    growth = growthFactor * (1.0f - exp(-population / 10));

    if (population < 50000)
    {
        growthFactor = 0.0085f;
    }
    else
    {
        growthFactor = 0.000085f;
    }

    GrowPopulation();
}


void Population::GrowPopulation()
{
    if (growthCooldown <= 0)
    {
        float totalGrowth = population * growth;

        if (population < maxPopulation)
        {
            population += (int)ceil(totalGrowth);
        }

        if (population > maxPopulation)
        {
            population = maxPopulation;
        }

        growthCooldown = growthRate;
    }
}

