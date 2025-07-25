# ARCY (Adam-Robert-Colin-Yan)
An epic game

## How to run
1. clone the repo 'git clone https://github.com/Yan1889/ARCY.git'
2. open with clion
3. click on run

## Project structure

### Files
+ `src/`
+ + `standard/`
+ + + `display/`
+ + + `loaders/`
+ + + `map/`
+ + + `player/`
+ + + `main.cpp`

### Call stack each frame
`while(!WindowShouldClose())`
+ `frameLogic()`
+ + `handle keyborad/mouse input`
+ + `update bombs`
+ + `update players`
+ + `...`
+ `displayGame()`
+ + `display bg terrain`
+ + `display nuked areas`
+ + `display players`
+ + `display bombs`
+ + `...`

### Map storage
```
struct Pixel {
    int x, y, playerId;
    std::vector<Pixel *> neighborsCached;
    // ...
};
```

In `Globals.h` lives a `std::vector<std::vector<Pixel>> territoryMap` which represents the whole state of the map

Note: oftentimes `Pixel*` will be passed around between functions (instead something like `int x, int y`)


### Player storage
Each player stores:
+ `std::unordered_set<Pixel *> _allPixels`
+ `std::unordered_set<Pixel *> _border_set`
+ `std::vector<Pixel *> _border_vec`

In `Globals.h` lives a `std::vector<Player> players`



# Todo - for devs

## UI
### Sounds
+ better sound for attacking 
### Sprites
+ Icon for missile silo
+ Icon for boat (future)

## Technical
### Fixes
*None*

### Features
+ Boats
+ Alliances
