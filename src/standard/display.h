//
// Created by yanam on 18.07.2025.
//

#ifndef DISPLAY_H
#define DISPLAY_H

#define MAIN_PLAYER G::players[0]
#define MAIN_PLAYER_COLOR G::players[0]._color

enum BuildingTypeDragging {
    NONE,
    CITY,
    SILO
};


inline bool buildMenuShown{};
inline BuildingTypeDragging buildingTypeDragging = NONE;


void displayGame();
void displayInfoTexts();
void displayPlayers();
void displayPlayerTags();
void displayBGTextures();
void displayCrossHair();
void displayBuildMenu();

#endif //DISPLAY_H
