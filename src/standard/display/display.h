//
// Created by yanam on 18.07.2025.
//

#ifndef DISPLAY_H
#define DISPLAY_H

#define MAIN_PLAYER G::players[0]
#define MAIN_PLAYER_COLOR G::players[0]._color
#include <string>

#include "../Building.h"


inline bool buildMenuShown{};
inline BUILDING_TYPE buildingTypeDragging = UNKNOWN;


void displayGame();
void displayInfoTexts();
void displayPlayers();
void displayPlayersInfo();
void displayTroopSlider();
void displayPlayerTags();
void displayBGTextures();
void displayCrossHair();
void displayBuildMenu();
void displayGameOver();

std::string formatNumber(int number);

#endif //DISPLAY_H
