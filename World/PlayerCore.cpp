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


void Player::ProcessAttackQueue(AttackQueue& attackQueue) {

    // refilling it from scratch
    std::unordered_set<Pixel *> pixelsAlreadyInTheQueue = ReFillAttackQueueFromScratch(attackQueue);

    // 60fps => ~10 border expansions / 1s
    const int maxPixelsPerFrame = 100;

    for (int i = 0; i < maxPixelsPerFrame && !attackQueue.queue.empty() && attackQueue.troops > 0; i++) {
        Pixel *newP = attackQueue.queue.front();

        if (newP->playerId != attackQueue.targetPlayerId) continue;

        attackQueue.troops--;

        // randomly don't get the pixel even though lost troops for it
        if (!newP->acceptRandomly()) continue;

        attackQueue.queue.pop();
        pixelsAlreadyInTheQueue.erase(newP);
        GetOwnershipOfPixel(newP);

        // update attack queue
        for (Pixel *neighbor: newP->GetNeighbors()) {
            if (attackQueue.troops <= 0) break; // no new Pixels
            if (neighbor->playerId == _id
                || pixelsAlreadyInTheQueue.contains(neighbor)
                || neighbor->invasionAcceptProbability == 0)
                continue;

            pixelsAlreadyInTheQueue.insert(neighbor);
            attackQueue.queue.push(neighbor);
        }
    }
}

std::unordered_set<Pixel *> Player::ReFillAttackQueueFromScratch(AttackQueue& attackQueue) {
    // clear the old one
    attackQueue.queue = {};

    std::unordered_set<Pixel *> pixelsAlreadyInTheQueue = {};

    // refill
    const int target = attackQueue.targetPlayerId;
    for (Pixel *borderPixel: _borderPixels) {
        for (Pixel *p: borderPixel->GetNeighbors()) {
            if (pixelsAlreadyInTheQueue.contains(p) ||
                p->playerId != target ||
                p->invasionAcceptProbability == 0)
                continue;

            attackQueue.queue.push(p);
            pixelsAlreadyInTheQueue.insert(p);
        }
    }
    return std::move(pixelsAlreadyInTheQueue);
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
