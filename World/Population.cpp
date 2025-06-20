#include "Population.h"

#include <iostream>
#include <bits/ostream.tcc>
#include <cmath>
#include <variant>

#include "raylib.h"

int Population::maxPopulation;
int Population::population = 100;
float Population::growth;
float Population::growthFactor = 0.0085f;
float Population::growthRate = 100.0f;
float Population::growthCooldown = growthRate;
int Population::cities;

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

