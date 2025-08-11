//
// Created by yanam on 18.07.2025.
//

#ifndef DISPLAY_H
#define DISPLAY_H

#define MAIN_PLAYER G::players[0]
#define MAIN_PLAYER_COLOR G::players[0]._color
#include <string>

#include "raylib.h"

enum MenuOption {
    MENU_OPTION_CITY,
    MENU_OPTION_SILO,
    MENU_OPTION_ATOM_BOMB,
    MENU_OPTION_HYDROGEN_BOMB,
    MENU_OPTION_NONE,
};

inline bool buildMenuShown{};
inline MenuOption currentMenuOption = MENU_OPTION_NONE;
inline Rectangle menuRect;


void displayGame();
void displayInfoTexts();
void displayPlayers();
void displayPlayersInfo();
void displayTroopSlider();
void displayPlayerTags();
void displayBGTextures();
void displayCrossHair();
void displayAndHandleBuildMenu();
void displayGameOver();
void initDisplay();

std::string formatNumber(int number);

#endif //DISPLAY_H
