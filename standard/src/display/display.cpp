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
#include "../map/CameraClipping.h"

using namespace CameraClipping;


constexpr float buildingRadius = 20;


void displayGame() {
    auto visibleChunks = ChunkGeneration::GetVisibleChunks(camera, GetScreenWidth(), GetScreenHeight());

    BeginDrawing();
    ClearBackground(Color{90, 90, 255, 255});

    BeginMode2D(camera);
    ChunkGeneration::DrawChunks(visibleChunks);
    displayBGTextures();
    displayPlayers();
    Bombs::RenderBomb();
    displayPlayerTags();
    displayCrossHair();
    DayNightCycle::Update();
    Bombs::RenderFlash();

    EndMode2D();

    DayNightCycle::Time();
    if (showControls) displayControls();
    displayInfoTexts();
    displayTroopSlider();
    displayAndHandleBuildMenu();
    displayGameOver();

    displayPlayersInfo();

    EndDrawing();
}

void displayControls() {
    Rectangle backgroundRec = {
        static_cast<float>(GetScreenWidth()) - 400,
        170,
        420,
        300
    };
    DrawRectangleRounded(backgroundRec, 0.1, 1, Fade(BLACK, 0.5));
    DrawText("Controls", GetScreenWidth() - 150, 190, 25, WHITE);
    DrawText("[W][A][S][D] Move camera", GetScreenWidth() - 250, 230, 20, WHITE);
    DrawText("[Up Arrow][Down Arrow] Zoom camera", GetScreenWidth() - 380, 260, 20, WHITE);
    DrawText("[M] Toggle build menu", GetScreenWidth() - 225, 290, 20, WHITE);
    DrawText("[F11] Toggle fullscreen", GetScreenWidth() - 235, 320, 20, WHITE);
    DrawText("[Q] Toggle building menu", GetScreenWidth() - 245, 350, 20, WHITE);
    DrawText("[1][2][3][4] Quick-building", GetScreenWidth() - 240, 380, 20, WHITE);
    DrawText("Press [E] to hide/show controls", GetScreenWidth() - 370, 420, 22, WHITE);
}

void displayInfoTexts() {
    Rectangle backgroundRec = {
        -25,
        static_cast<float>(GetScreenHeight()) - 175,
        425,
        200
    };
    DrawRectangleRounded(backgroundRec, 0.25, 1, Fade(BLACK, 0.5));
    // population
    const std::string populationStr = formatNumber(MAIN_PLAYER._totalPopulation);
    const std::string maxPopulationStr = formatNumber(MAIN_PLAYER._maxTotalPopulation);
    const std::string totalPopStr = "Total population: " + populationStr + " / " + maxPopulationStr;
    Vector2 textPos = {0 + 25, GetScreenHeight() - 50.f - (buildMenuShown ? menuRect.height : 0)};
    DrawText(totalPopStr.c_str(), textPos.x, textPos.y, 20, MAIN_PLAYER_COLOR);

    // neutral send
    const int toNeutral = MAIN_PLAYER._targetToAttackMap.contains(-1) ? MAIN_PLAYER._targetToAttackMap[-1].troops : 0;
    const std::string sendText = "People exploring neutral land: " + formatNumber(toNeutral);
    textPos = {0 + 25, GetScreenHeight() - 25.f - (buildMenuShown ? menuRect.height : 0)};
    DrawText(sendText.c_str(), textPos.x, textPos.y, 20, MAIN_PLAYER_COLOR);

    // money
    const std::string moneyText = "Money: " + formatNumber(MAIN_PLAYER._money.moneyBalance);
    textPos = {25, GetScreenHeight() - 75 - (buildMenuShown ? menuRect.height : 0)};
    DrawText(moneyText.c_str(), textPos.x, textPos.y, 20, MAIN_PLAYER_COLOR);


    // _pixelsOccupied
    const std::string territorySizeText = "Territory size: " + formatNumber(MAIN_PLAYER._pixelCount) + " pixels";
    textPos = {0 + 25, GetScreenHeight() - 100.f - (buildMenuShown ? menuRect.height : 0)};
    DrawText(territorySizeText.c_str(), textPos.x, textPos.y, 20, MAIN_PLAYER_COLOR);


    // fps
    Rectangle backgroundRecFPS = {
        static_cast<float>(GetScreenWidth()) - 125,
        static_cast<float>(GetScreenHeight()) - 40,
        135,
        50
    };
    DrawRectangleRounded(backgroundRecFPS, 0.25, 1, Fade(BLACK, 0.5));

    const int fps = GetFPS();
    const char *fpsText = TextFormat("FPS: %d", fps);
    const int textWidth = MeasureText(fpsText, 20);
    const int textX = GetScreenWidth() - textWidth - 25;
    const int textY = GetScreenHeight() - 25;
    DrawText(fpsText, textX, textY, 20, WHITE);
}

void displayPlayers() {
    auto viewRect = GetViewRectangle(camera);

    for (const Player &p: players) {
        #pragma omp parallel for
        for (int i = 0; i < p._border_vec.size(); i++) {
            Pixel *pixel = p._border_vec[i];
            if (IsPixelVisible(pixel, viewRect)) DrawPixel(pixel->x, pixel->y, p._color);
        }
    }

    // display every city for each player
    for (const Player &p: players) {
        for (const Pixel *c: p._cities) {
            Rectangle cityRect = {
                c->x - buildingRadius,
                c->y - buildingRadius,
                2 * buildingRadius,
                2 * buildingRadius,
            };
            if (CheckCollisionCameraRec(viewRect, cityRect)) {
                DrawTextureEx(
                    TextureCollection::city,
                    Vector2{c->x - buildingRadius, c->y - buildingRadius},
                    0,
                    2 * buildingRadius / TextureCollection::city.width,
                    p._color
                );
            }
        }
    }
    // display every silo for each player
    for (const Player &p: players) {
        for (const Pixel *s: p._silos) {
            Rectangle siloRect = {
                s->x - buildingRadius,
                s->y - buildingRadius,
                2 * buildingRadius,
                2 * buildingRadius,
            };
            if (CheckCollisionCameraRec(viewRect, siloRect)) {
                DrawTextureEx(
                    TextureCollection::silo,
                    Vector2(s->x - buildingRadius, s->y - buildingRadius),
                    0,
                    2 * buildingRadius / TextureCollection::silo.width,
                    p._color
                );
            }
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
        return players[i1]._pixelCount > players[i2]._pixelCount;
    });

    Rectangle backgroundRec = {
        -25,
        -25,
        static_cast<float>(showLeaderboard ? 550 : 350),
        static_cast<float>(showLeaderboard ? 425 : 100)
    };
    DrawRectangleRounded(backgroundRec, showLeaderboard ? 0.1 : 0.25, 1, Fade(BLACK, 0.5));

    for (int i = 0; i < players.size() && i < 10; i++) {
        const Player &p = players[playerIdxOrder[i]];
        const std::string troopsStr = "Troops: " + formatNumber(p._troops) + " / " + formatNumber(p._maxTroops);
        const std::string workersStr = "Workers: " + formatNumber(p._workers) + " / " + formatNumber(p._maxWorkers);
        const std::string moneyStr = "Money: " + formatNumber(p._money.returnMoney());
        const std::string deadStr = p._dead ? "(Defeated)" : "";
        const std::string numberStr = std::to_string(i + 1);
        const std::string infoStr = numberStr + ". " + p._name + ": " + troopsStr + "; " + moneyStr
                                    + " " + deadStr;
        if (showLeaderboard) DrawText(infoStr.c_str(), 25, 75 + 30 * i, 20, p._color);
    }

    DrawText("Leaderboard", 25, 20, 40, WHITE);
}

void displayPlayerTags() {
    for (auto &p: players) {
        if (p._dead) continue;

        // A = π * r^2 => r = sqrt(A / π)
        const float diameter = 2 * std::sqrt(p._pixelCount / PI);
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
    /* terrain bg texture
    SetTextureWrap(perlinTexture, TEXTURE_WRAP_CLAMP);

    Rectangle src = GetViewRectangle(camera);
    Rectangle dest = GetViewRectangle(camera);

    DrawTexturePro(perlinTexture, src, dest, Vector2{0, 0}, 0.0f, WHITE);
    */

    // nuke bg texture
    SetTextureWrap(explosionTexture, TEXTURE_WRAP_CLAMP);

    if (explosionTextureDirty) {
        UpdateTexture(explosionTexture, explosionImage.data);
        explosionTextureDirty = false;
    }

    Rectangle srcExplosion = GetViewRectangle(camera);
    Rectangle destExplosion = GetViewRectangle(camera);

    DrawTexturePro(explosionTexture, srcExplosion, destExplosion, Vector2{0, 0}, 0.0f, WHITE);
}

void displayAndHandleBuildMenu() {
    if (IsKeyPressed(KEY_M)) {
        buildMenuShown = !buildMenuShown;
    }
    if (!buildMenuShown) return;

    DrawRectangleRec(menuRect, Color{0, 0, 0, 150});

    const Rectangle cityButtonRect{
        menuRect.x,
        menuRect.y + menuRect.height * 0.1f,
        menuRect.width * 0.15f,
        menuRect.height * 0.8f,
    };
    const Rectangle siloButtonRect{
        menuRect.x + menuRect.width * 0.2f,
        menuRect.y + menuRect.height * 0.1f,
        menuRect.width * 0.15f,
        menuRect.height * 0.8f,
    };
    const Rectangle atomButtonRect{
        menuRect.x + menuRect.width * 0.4f,
        menuRect.y + menuRect.height * 0.1f,
        menuRect.width * 0.15f,
        menuRect.height * 0.8f,
    };
    const Rectangle hydrogenButtonRect{
        menuRect.x + menuRect.width * 0.6f,
        menuRect.y + menuRect.height * 0.1f,
        menuRect.width * 0.15f,
        menuRect.height * 0.8f,
    };
    DrawRectangleRec(cityButtonRect, GRAY);
    DrawText("Build city", cityButtonRect.x + 20, cityButtonRect.y + 10, 30, WHITE);
    const std::string cityCostStr = "$" + formatNumber(players[0].cityCost * (players[0]._cities.size() + 1));
    DrawText(cityCostStr.c_str(), cityButtonRect.x + 75, cityButtonRect.y + 40, 15, WHITE);
    DrawRectangleRec(siloButtonRect, RED);
    DrawText("Build silo", siloButtonRect.x + 20, siloButtonRect.y + 10, 30, WHITE);
    const std::string siloCostStr = "$" + formatNumber(players[0].siloCost * (players[0]._silos.size() + 1));
    DrawText(siloCostStr.c_str(), siloButtonRect.x + 75, siloButtonRect.y + 40, 15, WHITE);
    DrawRectangleRec(atomButtonRect, LIME);
    DrawText("Atom bomb", atomButtonRect.x + 20, atomButtonRect.y + 10, 30, WHITE);
    const std::string atomBombCostStr = "$" + formatNumber(Bombs::atomBombCost);
    DrawText(atomBombCostStr.c_str(), atomButtonRect.x + 75, atomButtonRect.y + 40, 15, WHITE);
    DrawRectangleRec(hydrogenButtonRect, DARKBLUE);
    DrawText("H-Bomb", hydrogenButtonRect.x + 20, hydrogenButtonRect.y + 10, 30, WHITE);
    const std::string hydrogenBombCostStr = "$" + formatNumber(Bombs::hydrogenBombCost);
    DrawText(hydrogenBombCostStr.c_str(), hydrogenButtonRect.x + 75, hydrogenButtonRect.y + 40, 15, WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(GetMousePosition(), cityButtonRect)) {
            currentMenuOption = MENU_OPTION_CITY;
        } else if (CheckCollisionPointRec(GetMousePosition(), siloButtonRect)) {
            currentMenuOption = MENU_OPTION_SILO;
        } else if (CheckCollisionPointRec(GetMousePosition(), atomButtonRect)) {
            currentMenuOption = MENU_OPTION_ATOM_BOMB;
        } else if (CheckCollisionPointRec(GetMousePosition(), hydrogenButtonRect)) {
            currentMenuOption = MENU_OPTION_HYDROGEN_BOMB;
        }
    }
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && currentMenuOption != MENU_OPTION_NONE) {
        Pixel *mPixel = GetPixelOnMouse();
        switch (currentMenuOption) {
            case MENU_OPTION_CITY:
                MAIN_PLAYER.TryAddCity(mPixel);
                break;
            case MENU_OPTION_SILO:
                MAIN_PLAYER.TryAddSilo(mPixel);
                break;
            case MENU_OPTION_ATOM_BOMB:
                MAIN_PLAYER.TryLaunchAtomBomb(mPixel);
                break;
            case MENU_OPTION_HYDROGEN_BOMB:
                MAIN_PLAYER.TryLaunchHydrogenBomb(mPixel);
                break;
        }
        currentMenuOption = MENU_OPTION_NONE;
    }

    if (currentMenuOption != MENU_OPTION_NONE) {
        // displaying the dragged object
        const Texture2D *t = nullptr;
        Color color{};
        float yOffset = 0.f;
        Pixel *mPixel = GetPixelOnMouse();
        switch (currentMenuOption) {
            case MENU_OPTION_CITY:
                t = &TextureCollection::city;
                color = MAIN_PLAYER.CanBuildCity(mPixel) ? Fade(GREEN, 0.5) : Fade(RED, 0.5);
                break;
            case MENU_OPTION_SILO:
                t = &TextureCollection::silo;
                color = MAIN_PLAYER.CanBuildSilo(mPixel) ? Fade(GREEN, 0.5) : Fade(RED, 0.5);
                break;
            case MENU_OPTION_ATOM_BOMB:
                t = &TextureCollection::mapIcon;
                yOffset = -t->height / 2.f * 2 * buildingRadius / t->width;
                color = MAIN_PLAYER.CanLaunchAtomBomb(mPixel) ? GREEN : RED;
                DrawCircleV(GetMousePosition(), camera.zoom * 50, Fade(color, 0.5));
                break;
            case MENU_OPTION_HYDROGEN_BOMB:
                t = &TextureCollection::mapIcon;
                color = MAIN_PLAYER.CanLaunchHydrogenBomb(mPixel) ? GREEN : RED;
                yOffset = -t->height / 2.f * 2 * buildingRadius / t->width;
                DrawCircleV(GetMousePosition(), camera.zoom * 350, Fade(color, 0.5));
                break;
        }
        DrawTextureEx(
            *t,
            Vector2{GetMousePosition().x - buildingRadius, GetMousePosition().y - buildingRadius + yOffset},
            0,
            2 * buildingRadius / t->width,
            color
        );
    }
}

void displayTroopSlider() {
    Rectangle sliderRect{
        100,
        GetScreenHeight() * 0.8f,
        150,
        GetScreenHeight() * 0.05f
    };
    if (buildMenuShown) sliderRect.y -= menuRect.height;

    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
    GuiSlider(sliderRect, "Troops", "Workers", &players[0]._troopPercentage, 0, 1);
}

void displayGameOver() {
    if (gameOver) {
        constexpr int fontSize = 200;
        std::string winnerText = winnerId == -1
                                     ? std::string("Y'all lost!")
                                     : std::string(players[winnerId]._name + " win");
        if (winnerId > 0) {
            winnerText += 's';
        }
        winnerText += '!';
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

void initDisplay() {
    menuRect = {
        0.f,
        GetScreenHeight() * 0.9f,
        (float) GetScreenWidth(),
        GetScreenHeight() * 0.1f
    };
}
