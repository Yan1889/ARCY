//
// Created by yanam on 14.07.2025.
//

#include "Globals.h"
#include "Player.h"

void Player::Expand(const int target, const float percentage) {
    if (_population / 2 < 100) return; // not enough troops

    const int newPeopleLeaving = _population * percentage;
    _population -= newPeopleLeaving;

    // insert if new target
    if (!_targetToAttackMap.contains(target)) {
        _targetToAttackMap[target] = {
            target,
            0,
            {}
        };
    }
    _targetToAttackMap[target].troops += newPeopleLeaving;
    ReFillAttackQueueFromScratch(_targetToAttackMap[target]);
}


void Player::ProcessAttackQueue(Attack& attack) {
    // 60fps => ~10 border expansions / 1s
    constexpr int maxPixelsPerFrame = 100;

    for (int i = 0; i < maxPixelsPerFrame && !attack.set.empty() && attack.troops > 0; i++) {
        Pixel *newP = attack.queue.front();

        if (newP->playerId != attack.targetPlayerId) {
            attack.queue.pop();
            attack.set.erase(newP);
            continue;
        }

        attack.troops--;

        // randomly don't get the pixel even though lost troops for it
        if (!newP->acceptRandomly()) continue;

        attack.queue.pop();
        attack.set.erase(newP);
        GetOwnershipOfPixel(newP);
    }

    // only update queue when empty
    if (attack.set.empty()) {
        UpdateAllDirty();
        ReFillAttackQueueFromScratch(attack);
    }
}

void Player::ReFillAttackQueueFromScratch(Attack& attack) {
    // clear
    attack.queue = {};
    attack.set.clear();

    // refill
    const int target = attack.targetPlayerId;
    for (Pixel *borderPixel: _borderPixels) {
        for (Pixel *p: borderPixel->GetNeighbors()) {
            if (attack.set.contains(p) ||
                p->playerId != target ||
                p->invasionAcceptProbability == 0)
                continue;

            attack.queue.push(p);
            attack.set.insert(p);
        }
    }
}

void Player::GetOwnershipOfPixel(Pixel *newP) {
    Player &attacker = *this;
    attacker._allPixels.insert(newP);

    if (newP->playerId >= 0) {
        Player &defender = G::players[newP->playerId];
        defender.LoseOwnershipOfPixel(newP, false);
    } else if (newP->playerId == -2) {
        // reclaim contaminated pixel
        G::RemoveExplosionPixel(newP);
    }
    newP->playerId = _id;

    attacker.MarkPixelAsDirty(newP);

    ImageDrawPixel(&G::territoryImage, newP->x, newP->y, _color);
    G::territoryTextureDirty = true;

    // center
    AddPixelToCenter(newP);
}

void Player::MarkPixelAsDirty(Pixel *pixel) {
    _dirtyPixels.insert(pixel);

    const std::vector<Pixel *> &affectedPixels = pixel->GetNeighbors();
    for (Pixel *neighbor: affectedPixels) {
        _dirtyPixels.insert(neighbor);
    }
}

void Player::UpdateSingleDirty(Pixel *pixel) {
    if (!_allPixels.contains(pixel)) {
        // pixel isn't owned by player
        RemoveBorderPixel(pixel);
        return;
    }

    const bool wasBorderPixel = _borderSet.contains(pixel);
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

void Player::LoseOwnershipOfPixel(Pixel *pixel, const bool updateTextureToo) {
    _allPixels.erase(pixel);

    pixel->playerId = -1;

    MarkPixelAsDirty(pixel);
    RemovePixelFromCenter(pixel);

    if (updateTextureToo) {
        ImageDrawPixel(&G::territoryImage, pixel->x, pixel->y, BLANK);
        G::territoryTextureDirty = true;
    }

    // die if too small
    if (_allPixels.empty()) {
        UpdateAllDirty();
        _dead = true;
    }
}
