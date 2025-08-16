//
// Created by yanam on 14.07.2025.
//

#include <algorithm>
#include <iostream>
#include <ostream>

#include "Player.h"
#include "../Globals.h"

using namespace G;

void Player::Expand(const int target, const float percentage) {
    const int newPeopleLeaving = _troops * percentage;
    // target is self or not enough troops or odes not border
    if (target == _id || _troops - newPeopleLeaving < 30 || !Borders(target)) return;
    _troops -= newPeopleLeaving;

    auto &attack = _targetToAttackMap[target];
    attack.targetPlayerId = target;
    attack.troops += newPeopleLeaving;
    ReFillAttackQueueFromScratch(attack);

    // Attack sounds if main character is targeting a player/bot
    if (_id == 0 && target != -1 && !attack.set.empty()) mySounds.Play(mySounds.attackPool);
}


void Player::ProcessAttackQueue(Attack &attack) {
    constexpr int maxPixelsPerFrame = 100;

    for (int i = 0; i < maxPixelsPerFrame && !attack.set.empty() && attack.troops > 0; i++) {
        Pixel *newP = attack.queue.front();
        attack.queue.pop();
        attack.set.erase(newP);

        if (newP->playerId != attack.targetPlayerId) {
            // invalid pixel
            continue;
        }

        attack.troops--;

        // randomly don't get the pixel even though lost troops for it
        if (!newP->acceptRandomly()) {
            // add the pixel to the queue in the back
            attack.set.insert(newP);
            attack.queue.push(newP);
            continue;
        }
        GetOwnershipOfPixel(newP);

        // update attack queue
        for (Pixel *neighbor: newP->GetNeighbors()) {
            if (attack.troops <= 0) break; // no new Pixels
            if (neighbor->playerId == _id
                || attack.set.contains(neighbor)
                || Terrain::GetInvasionProbability(neighbor->kind) == 0)
                continue;

            attack.set.insert(neighbor);
            attack.queue.push(neighbor);
        }
    }

    // only update queue when empty
    if (attack.set.empty()) {
        ReFillAttackQueueFromScratch(attack);
    }
}

void Player::ReFillAttackQueueFromScratch(Attack &attack) {
    UpdateAllDirtyBorder();
    attack.queue = {};
    attack.set.clear();

    const int target = attack.targetPlayerId;
    for (Pixel *borderPixel: _border_vec) {
        for (Pixel *p: borderPixel->GetNeighbors()) {
            if (attack.set.contains(p) ||
                p->playerId != target ||
                Terrain::GetInvasionProbability(p->kind) == 0)
                continue;

            attack.queue.push(p);
            attack.set.insert(p);
        }
    }
}


void Player::GetOwnershipOfPixel(Pixel *newP) {
    _pixelCount++;
    AddPixelToCenter(newP);

    if (newP->playerId >= 0) {
        const std::vector<Building> collectedBuildings = players[newP->playerId].LoseOwnershipOfPixel(newP, false);
        for (const Building building: collectedBuildings) {
            switch (building.type) {
                case CITY:
                    _cities.push_back(building.pos);
                    break;
                case SILO:
                    _silos.push_back(building.pos);
                    break;
                case UNKNOWN:
                    std::cerr << "Unknown Building" << std::endl;
            }
        }
    } else if (newP->playerId == -2) {
        // reclaim contaminated pixel
        newP->contaminated = false;
        ImageDrawPixel(&explosionImage, newP->x, newP->y, BLANK);
        explosionTextureDirty = true;
    }
    newP->playerId = _id;

    MarkAsDirty(newP);

    ImageDrawPixel(&territoryImage, newP->x, newP->y, _color);
    territoryTextureDirty = true;
}

std::vector<Building> Player::LoseOwnershipOfPixel(Pixel *pixel, const bool updateTextureToo) {
    _pixelCount--;
    RemovePixelFromCenter(pixel);

    pixel->playerId = -1;

    MarkAsDirty(pixel);

    if (updateTextureToo) {
        ImageDrawPixel(&territoryImage, pixel->x, pixel->y, BLANK);
        territoryTextureDirty = true;
    }

    // die if too small
    if (_pixelCount == 0) {
        UpdateAllDirtyBorder();
        _dead = true;
    }

    std::vector<Building> buildingsLost{};
    for (auto iter = _cities.begin(); iter != _cities.end();) {
        const Pixel *c = *iter;
        if (c == pixel) {
            iter = _cities.erase(iter);
            buildingsLost.push_back(Building{pixel, CITY});
        } else {
            ++iter;
        }
    }
    for (auto iter = _silos.begin(); iter != _silos.end();) {
        const Pixel *s = *iter;
        if (s == pixel) {
            iter = _silos.erase(iter);
            buildingsLost.push_back(Building{pixel, SILO});
        } else {
            ++iter;
        }
    }
    return buildingsLost;
}


// ----- Helper methods -----

void Player::MarkAsDirty(Pixel *pixel) {
    if (_dirtyBorderPixels_set.insert(pixel).second) {
        _dirtyBorderPixels_vec.push_back(pixel);
    }
    for (Pixel *p: pixel->GetNeighbors()) {
        if (_dirtyBorderPixels_set.insert(p).second) {
            _dirtyBorderPixels_vec.push_back(p);
        }
    }
}

void Player::UpdateAllDirtyBorder() {
    for (Pixel *p: _dirtyBorderPixels_vec) {
        UpdateBorderSingle(p);
    }
    _dirtyBorderPixels_vec.clear();
    _dirtyBorderPixels_set.clear();
}


void Player::UpdateBorderSingle(Pixel *pixel) {
    if (pixel->playerId != _id) {
        // pixel isn't owned by player
        RemoveBorderPixel(pixel);
        return;
    }

    const bool wasBorderPixel = _border_set.contains(pixel);
    bool nowBorderPixel = false;
    for (Pixel *nn: pixel->GetNeighbors()) {
        if (nn->playerId != _id) {
            nowBorderPixel = true;
            break;
        }
    }
    if (nowBorderPixel && !wasBorderPixel) {
        AddBorderPixel(pixel);
    } else if (!nowBorderPixel && wasBorderPixel) {
        RemoveBorderPixel(pixel);
    }
}

void Player::AddBorderPixel(Pixel *p) {
    if (_border_set.insert(p).second) {
        _border_vec.push_back(p);
    }
}

void Player::RemoveBorderPixel(Pixel *p) {
    if (_border_set.erase(p)) {
        _border_vec.erase(std::ranges::find(_border_vec, p));
    }
}

bool Player::Borders(const int otherId) const {
    return std::ranges::any_of(_border_vec, [otherId](const Pixel *borderP) {
        return std::ranges::any_of(borderP->GetNeighbors(), [otherId](const Pixel *n) {
            return n->playerId == otherId;
        });
    });
}
