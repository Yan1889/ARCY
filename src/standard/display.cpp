//
// Created by yanam on 18.07.2025.
//

#include "display.h"

#include <cmath>

#include "loaders/TextureCollection.h"
#include <cstring>
#include <format>
#include <iostream>
#include <ostream>
#include <string>

#include "Bombs.h"
#include "Globals.h"
#include "raylib.h"


constexpr float buildingRadius = 20;


void displayGame() {
    BeginDrawing();
    ClearBackground(Color{90, 90, 255, 255});

    BeginMode2D(camera);

    displayBGTextures();
    displayPlayers();
    displayPlayerTags();
    displayCrossHair();
    Bombs::Render();

    EndMode2D();

    displayInfoTexts();
    displayBuildMenu();
    displayGameOver();

    EndDrawing();
}


void displayInfoTexts() {
    // instructions
    DrawText("Move with WASD", GetScreenWidth() / 20 - 25, GetScreenHeight() / 20, 20, MAIN_PLAYER_COLOR);
    DrawText("Up or Down Arrow to zoom", GetScreenWidth() / 20 - 25, GetScreenHeight() / 20 + 40, 20,
             MAIN_PLAYER_COLOR);
    DrawText("Right-click to build a city ($10K * city count)", GetScreenWidth() / 20 - 25, GetScreenHeight() / 20 + 80,
             20, MAIN_PLAYER_COLOR);
    DrawText("Esc to exit the game", GetScreenWidth() / 20 - 25, GetScreenHeight() / 20 + 120, 20, MAIN_PLAYER_COLOR);
    DrawText("1 to drop a atom bomb ($10K), 2 a hydrogen bomb ($100K)", GetScreenWidth() / 20 - 25,
             GetScreenHeight() / 20 + 160, 20, MAIN_PLAYER_COLOR);
    DrawText("F11 to toggle fullscreen", GetScreenWidth() / 20 - 25, GetScreenHeight() / 20 + 200, 20,
             MAIN_PLAYER_COLOR);
    DrawText("Space to expand your territory", GetScreenWidth() / 20 - 25, GetScreenHeight() / 20 + 240, 20,
             MAIN_PLAYER_COLOR);
    DrawText("Left-Click to attack another player", GetScreenWidth() / 20 - 25, GetScreenHeight() / 20 + 280, 20,
             MAIN_PLAYER_COLOR);

    // fps
    const int fps = GetFPS();
    const char *fpsText = TextFormat("FPS: %d", fps);
    const int textWidth = MeasureText(fpsText, 20);
    const int textX = GetScreenWidth() - textWidth - 25;
    const int textY = GetScreenHeight() - 25;
    DrawText(fpsText, textX, textY, 20, MAIN_PLAYER_COLOR);

    // population
    float populationDisplay = static_cast<float>(MAIN_PLAYER._population);
    float maxPopulationDisplay = static_cast<float>(MAIN_PLAYER._maxPopulation);
    const char *populationText;

    if (maxPopulationDisplay >= 1000) {
        maxPopulationDisplay /= 1000;

        if (populationDisplay >= 1000) {
            populationDisplay /= 1000;

            populationText = TextFormat("Population: %.2fK / %.2fK", populationDisplay, maxPopulationDisplay);
        } else {
            populationText = TextFormat("Population: %.0f / %.2fK", populationDisplay, maxPopulationDisplay);
        }
    }

    DrawText(populationText, 0 + 25, GetScreenHeight() - 50, 20, MAIN_PLAYER_COLOR);

    const char *sendText = TextFormat(
        "People exploring neutral land: %d",
        MAIN_PLAYER._targetToAttackMap.contains(-1) ? MAIN_PLAYER._targetToAttackMap[-1].troops : 0
    );
    DrawText(sendText, 0 + 25, GetScreenHeight() - 25, 20, MAIN_PLAYER_COLOR);


    // money
    float moneyBalanceDisplay = static_cast<float>(MAIN_PLAYER._money.moneyBalance);
    const char *moneyText;

    if (moneyBalanceDisplay >= 1000000) {
        moneyBalanceDisplay /= 1000000;

        moneyText = TextFormat("Money Balance: $%.2fM", moneyBalanceDisplay);
    } else if (moneyBalanceDisplay >= 1000) {
        moneyBalanceDisplay /= 1000;

        moneyText = TextFormat("Money Balance: $%.2fK", moneyBalanceDisplay);
    } else {
        moneyText = TextFormat("Money Balance: $%.0f", moneyBalanceDisplay);
    }

    DrawText(moneyText, 25, GetScreenHeight() - 75, 20, MAIN_PLAYER_COLOR);

    // _pixelsOccupied
    const char *territorySizeText = ("pixels occupied (your size): " + std::to_string(MAIN_PLAYER._allPixels.size())).
            c_str();
    DrawText(territorySizeText, 0 + 25, GetScreenHeight() - 100, 20, MAIN_PLAYER_COLOR);
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

void displayPlayerTags() {
    for (int i = 0; i < players.size(); i++) {
        if (players[i]._dead) continue;

        const float diameter = 2 * std::sqrt(players[i]._allPixels.size() / PI); // A = π * r^2 => r = sqrt(A / π)
        // Shows who you are
        const char *name = i == 0 ? "You" : ("NPC " + std::to_string(i)).c_str();
        const int charCount = strlen(name);
        const float pxWidthPerChar = diameter / charCount;

        const int fontSize = pxWidthPerChar;
        const int spacing = 1;


        Vector2 textSize = MeasureTextEx(GetFontDefault(), name, fontSize, 1);
        Vector2 textPos = {
            players[i]._centerPixel_x - textSize.x / 2,
            players[i]._centerPixel_y - textSize.y / 2
        };
        DrawTextEx(GetFontDefault(), name, textPos, fontSize, spacing, WHITE);
    }
}

void displayCrossHair() {
    int size = 6;
    int thickness = 1;
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
        switch (buildingTypeDragging) {
            case CITY:
                t = &TextureCollection::city;
                break;
            case SILO:
                t = &TextureCollection::silo;
                break;
            case UNKNOWN:
                std::cerr << "Unknown building type" << std::endl;
        }
        DrawTextureEx(
            *t,
            Vector2(GetMousePosition().x - buildingRadius, GetMousePosition().y - buildingRadius),
            0,
            2 * buildingRadius / t->width,
            MAIN_PLAYER.CanBuildCity(GetPixelOnMouse()) || MAIN_PLAYER.CanBuildSilo(GetPixelOnMouse())
                ? Fade(GREEN, 0.5)
                : Fade(RED, 0.5)
        );
    }
}

void displayGameOver() {
    if (gameOver) {
        constexpr int fontSize = 200;

        const std::string winnerText = winnerId == 0 ? "You won" : std::format("NPC {} won", winnerId);
        const int textWidth = MeasureText(winnerText.c_str(), fontSize);
        DrawText(winnerText.c_str(), GetScreenWidth() / 2 - textWidth / 2, GetScreenHeight() / 2, fontSize, WHITE);
    }
}
