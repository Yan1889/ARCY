//
// Created by yanam on 18.07.2025.
//

#ifndef DISPLAY_H
#define DISPLAY_H

#define MAIN_PLAYER G::players[0]
#define MAIN_PLAYER_COLOR G::players[0]._color
#include "raylib.h"


inline Camera2D camera{};
inline Vector2 playerPos;

void displayGame();

void displayInfoTexts();

void displayPlayers();

void displayPlayerTags();

void displayBGTextures();

void displayCrossHair();


#endif //DISPLAY_H
