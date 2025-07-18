//
// Created by yanam on 14.07.2025.
//

#include "Globals.h"
#include "Player.h"

void Player::Expand(const int target, const float percentage) {
    if (_population / 2 < 100) return; // not enough troops

    const int newPeopleLeaving = _population * percentage;
    _population -= newPeopleLeaving;

    auto& attack = _targetToAttackMap[target];
    attack.targetPlayerId = target;
    _targetToAttackMap[target].troops += newPeopleLeaving;

    ReFillAttackQueueFromScratch(_targetToAttackMap[target]);
}


void Player::ProcessAttackQueue(Attack& attack) {
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

        // update attack queue
        for (Pixel *neighbor: newP->GetNeighbors()) {
            if (attack.troops <= 0) break; // no new Pixels
            if (neighbor->playerId == _id
                || attack.set.contains(neighbor)
                || neighbor->invasionAcceptProbability == 0)
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

void Player::ReFillAttackQueueFromScratch(Attack& attack) {
    UpdateAllDirtyBorder();
    attack.queue = {};
    attack.set.clear();

    const int target = attack.targetPlayerId;
    for (Pixel *borderPixel: _border_vec) {
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

    attacker.MarkAsDirty(newP);

    ImageDrawPixel(&G::territoryImage, newP->x, newP->y, _color);
    G::territoryTextureDirty = true;

    // center
    AddPixelToCenter(newP);
}

void Player::MarkAsDirty(Pixel *pixel) {
    if (_dirtyBorderPixels_set.insert(pixel).second) {
        _dirtyBorderPixels_vec.push_back(pixel);
    }
    for (Pixel* p : pixel->GetNeighbors()) {
        if (_dirtyBorderPixels_set.insert(p).second) {
            _dirtyBorderPixels_vec.push_back(p);
        }
    }
}

void Player::UpdateAllDirtyBorder() {
    for (Pixel* p : _dirtyBorderPixels_vec) {
        UpdateBorderSingle(p);
    }
    _dirtyBorderPixels_vec.clear();
    _dirtyBorderPixels_set.clear();
}


void Player::UpdateBorderSingle(Pixel *pixel) {
    if (!_allPixels.contains(pixel)) {
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

void Player::LoseOwnershipOfPixel(Pixel *pixel, const bool updateTextureToo) {
    _allPixels.erase(pixel);

    pixel->playerId = -1;

    MarkAsDirty(pixel);
    RemovePixelFromCenter(pixel);

    if (updateTextureToo) {
        ImageDrawPixel(&G::territoryImage, pixel->x, pixel->y, BLANK);
        G::territoryTextureDirty = true;
    }

    // die if too small
    if (_allPixels.empty()) {
        UpdateAllDirtyBorder();
        _dead = true;
    }
}
