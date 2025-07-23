//
// Created by yanam on 18.07.2025.
//

#include "display.h"

#include <algorithm>
#include <cmath>

#include "../loaders/TextureCollection.h"
#include <format>
#include <iostream>
#include <ostream>
#include <string>

#include "../Bombs.h"
#include "../Globals.h"
#include "raylib.h"
#include "DayNightCycle.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"


constexpr float buildingRadius = 20;


void displayGame() {
    BeginDrawing();
    ClearBackground(Color{90, 90, 255, 255});

    BeginMode2D(camera);

    displayBGTextures();
    displayPlayers();
    Bombs::Render();
    displayPlayerTags();
    displayCrossHair();

    EndMode2D();

    DayNightCycle::Update();
    displayInfoTexts();
    displayTroopSlider();
    displayBuildMenu();
    displayGameOver();
    displayPlayersInfo();

    EndDrawing();
}


void displayInfoTexts() {
    // population
    const std::string populationStr = formatNumber(MAIN_PLAYER._totalPopulation);
    const std::string maxPopulationStr = formatNumber(MAIN_PLAYER._maxTotalPopulation);
    const std::string totalPopStr = "Total population: " + populationStr + " / " + maxPopulationStr;
    DrawText(totalPopStr.c_str(), 0 + 25, GetScreenHeight() - 50, 20, MAIN_PLAYER_COLOR);


    // neutral send
    const char *sendText = TextFormat(
        "People exploring neutral land: %d",
        MAIN_PLAYER._targetToAttackMap.contains(-1) ? MAIN_PLAYER._targetToAttackMap[-1].troops : 0
    );
    DrawText(sendText, 0 + 25, GetScreenHeight() - 25, 20, MAIN_PLAYER_COLOR);


    // money
    const std::string moneyText = "Money: " + formatNumber(MAIN_PLAYER._money.moneyBalance);
    DrawText(moneyText.c_str(), 25, GetScreenHeight() - 75, 20, MAIN_PLAYER_COLOR);


    // _pixelsOccupied
    const std::string territorySizeText = "Pixels occupied (your size): " + std::to_string(MAIN_PLAYER._allPixels.size());
    DrawText(territorySizeText.c_str(), 0 + 25, GetScreenHeight() - 100, 20, MAIN_PLAYER_COLOR);


    // fps
    const int fps = GetFPS();
    const char *fpsText = TextFormat("FPS: %d", fps);
    const int textWidth = MeasureText(fpsText, 20);
    const int textX = GetScreenWidth() - textWidth - 25;
    const int textY = GetScreenHeight() - 25;
    DrawText(fpsText, textX, textY, 20, MAIN_PLAYER_COLOR);
}

void displayPlayers() {
    for (const Player &p: players) {
        for (Pixel *pixel: p._border_vec) {
            DrawPixel(pixel->x, pixel->y, p._color);
        }
    }

    // display every city for each player
    for (const Player &p: players) {
        for (const Pixel *c: p._cities) {
            DrawTextureEx(
                TextureCollection::city,
                Vector2{c->x - buildingRadius, c->y - buildingRadius},
                0,
                2 * buildingRadius / TextureCollection::city.width,
                WHITE // p._color
            );
        }
    }
    // display every city for each player
    for (const Player &p: players) {
        for (const Pixel *s: p._silos) {
            DrawTextureEx(
                TextureCollection::silo,
                Vector2(s->x - buildingRadius, s->y - buildingRadius),
                0,
                2 * buildingRadius / TextureCollection::silo.width,
                WHITE //p._color
            );
        }
    }

    // territory texture
    if (territoryTextureDirty) {
        UpdateTexture(territoryTexture, territoryImage.data);
        territoryTextureDirty = false;
    }
    DrawTexture(territoryTexture, 0, 0, Fade(WHITE, 0.5));
}

void displayPlayersInfo() {
    std::vector<int> playerIdxOrder(players.size());
    for (int i = 0; i < players.size(); i++) {
        playerIdxOrder[i] = i;
    }
    std::ranges::sort(playerIdxOrder, [](const int i1, const int i2) {
        return players[i1]._allPixels.size() > players[i2]._allPixels.size();
    });

    for (int i = 0; i <  10; i++) {
        const Player& p = players[playerIdxOrder[i]];
        const std::string troopsStr = "Troops: " + formatNumber(p._troops) + " / " + formatNumber(p._maxTroops);
        const std::string workersStr = "Workers: " + formatNumber(p._workers) + " / " + formatNumber(p._maxWorkers);
        const std::string moneyStr = "Money: " + formatNumber(p._money.returnMoney());
        const std::string deadStr = p._dead ? "(Defeated)" : "";
        const std::string numberStr = std::to_string(i + 1);
        const std::string infoStr = numberStr + ". " + p._name + ": " + troopsStr + "; " + workersStr + "; " + moneyStr + deadStr;
        DrawText(infoStr.c_str(), 25, 20 + 30 * i, 20, p._color);
    }
}

void displayPlayerTags() {
    for (auto & p : players) {
        if (p._dead) continue;

        // A = π * r^2 => r = sqrt(A / π)
        const float diameter = 2 * std::sqrt(p._allPixels.size() / PI);
        const int charCount = p._name.length();
        const float pxWidthPerChar = diameter / charCount;

        const int fontSize = pxWidthPerChar;
        constexpr int spacing = 1;


        const Vector2 textSize = MeasureTextEx(GetFontDefault(), p._name.c_str(), fontSize, 1);
        const Vector2 textPos = {
            p._centerPixel_x - textSize.x / 2,
            p._centerPixel_y - textSize.y / 2
        };
        DrawTextEx(GetFontDefault(), p._name.c_str(), textPos, fontSize, spacing, WHITE);
    }
}

void displayCrossHair() {
    constexpr int size = 6;
    constexpr int thickness = 1;
    DrawRectangle((int) playerPos.x - size, (int) playerPos.y - thickness / 2, size * 2 + 1, thickness, WHITE);
    DrawRectangle((int) playerPos.x - thickness / 2, (int) playerPos.y - size, thickness, size * 2 + 1, WHITE);
}

void displayBGTextures() {
    // terrain bg texture
    DrawTextureV(perlinTexture, Vector2{0, 0}, WHITE);
    // nuke bg texture
    if (explosionTextureDirty) {
        UpdateTexture(explosionTexture, explosionImage.data);
        explosionTextureDirty = false;
    }
    DrawTextureV(explosionTexture, Vector2{0, 0}, WHITE);
}

void displayBuildMenu() {
    if (IsKeyPressed(KEY_M)) {
        // menu state is changed when 'm' is pressed
        // not shown -> shown -> not shown -> ...;
        buildMenuShown = !buildMenuShown;
    }
    if (!buildMenuShown) return;

    const Rectangle menuRect{
        0.f,
        GetScreenHeight() * 0.9f,
        (float) GetScreenWidth(),
        GetScreenHeight() * 0.1f
    };
    // black transparent bg
    DrawRectangleRec(menuRect, Color{0, 0, 0, 150});

    const Rectangle cityButtonRect{
        menuRect.x,
        menuRect.y + menuRect.height * 0.1f,
        menuRect.width * 0.2f,
        menuRect.height * 0.8f,
    };
    const Rectangle siloButtonRect{
        menuRect.x + menuRect.width * 0.3f,
        menuRect.y + menuRect.height * 0.1f,
        menuRect.width * 0.2f,
        menuRect.height * 0.8f,
    };
    DrawRectangleRec(cityButtonRect, DARKBLUE);
    DrawText("Build city", cityButtonRect.x + 20, cityButtonRect.y + 10, 50, WHITE);
    DrawRectangleRec(siloButtonRect, DARKBLUE);
    DrawText("Build silo", siloButtonRect.x + 20, siloButtonRect.y + 10, 50, WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(GetMousePosition(), cityButtonRect)) {
            buildingTypeDragging = CITY;
        } else if (CheckCollisionPointRec(GetMousePosition(), siloButtonRect)) {
            buildingTypeDragging = SILO;
        }
    }
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && buildingTypeDragging != UNKNOWN) {
        switch (buildingTypeDragging) {
            case CITY:
                MAIN_PLAYER.TryAddCity(GetPixelOnMouse());
            case SILO:
                MAIN_PLAYER.TryAddSilo(GetPixelOnMouse());
                break;
            case UNKNOWN:
                std::cerr << "Unknown building type" << std::endl;
        }
        buildingTypeDragging = UNKNOWN;
    }


    // displaying the dragged object
    if (buildingTypeDragging != UNKNOWN) {
        const Texture2D *t = nullptr;
        Color color = Fade(RED, 0.5);
        switch (buildingTypeDragging) {
            case CITY:
                t = &TextureCollection::city;
                color = MAIN_PLAYER.CanBuildCity(GetPixelOnMouse()) ? Fade(GREEN, 0.5) : Fade(RED, 0.5);
                break;
            case SILO:
                t = &TextureCollection::silo;
                color = MAIN_PLAYER.CanBuildSilo(GetPixelOnMouse()) ? Fade(GREEN, 0.5) : Fade(RED, 0.5);
                break;
            case UNKNOWN:
                std::cerr << "Unknown building type" << std::endl;
        }
        DrawTextureEx(
            *t,
            Vector2(GetMousePosition().x - buildingRadius, GetMousePosition().y - buildingRadius),
            0,
            2 * buildingRadius / t->width,
            color
        );
    }
}

void displayTroopSlider() {
    const Rectangle sliderRect{
        100,
        GetScreenHeight() * 0.8f,
        150,
        GetScreenHeight() * 0.05f
    };
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
    GuiSlider(sliderRect, "workers", "troops", &players[0]._troopPercentage, 0, 1);
}

void displayGameOver() {
    if (gameOver) {
        constexpr int fontSize = 200;
        const std::string winnerText = winnerId == -1? std::string("Y'all lost!") : std::string(players[winnerId]._name + " win(s)!");
        const int textWidth = MeasureText(winnerText.c_str(), fontSize);
        DrawText(winnerText.c_str(), GetScreenWidth() / 2 - textWidth / 2, GetScreenHeight() / 2, fontSize, WHITE);
    }
}

std::string formatNumber(const int number) {
    if (number < 1'000) {
        return std::format("{}", number);
    }
    if (number < 1'000'000) {
        return std::format("{:.1f}K", number / 1'000.f);
    }
    if (number < 1'000'000'000) {
        return std::format("{:.1f}M", number / 1'000'000.f);
    }
    return std::format("{:.1f}B", number / 1'000'000'000.f);
}
