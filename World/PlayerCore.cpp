//
// Created by yanam on 14.07.2025.
//

#include "Globals.h"
#include "Player.h"

void Player::Expand(const int target, const float percentage) {
    if (_population / 2 < 100) return; // not enough troops

    const int newPeopleLeaving = _population * percentage;
    _population -= newPeopleLeaving;

    // ----- init/find an attack queue -----
    int queueIdx;

    if (!_attackedPlayerIdToQueueIdxMap.contains(target)) {
        queueIdx = _allOnGoingAttackQueues.size();
        _allOnGoingAttackQueues.push_back({
            target,
            newPeopleLeaving,
            {}
        });
        _attackedPlayerIdToQueueIdxMap[target] = queueIdx;
    } else {
        queueIdx = _attackedPlayerIdToQueueIdxMap[target];
        _allOnGoingAttackQueues[queueIdx].troops += newPeopleLeaving;
    }
    ReFillAttackQueueFromScratch(queueIdx);
}


void Player::ProcessAttackQueue(const int queueIdx) {
    AttackQueue &attackToWorkOn = _allOnGoingAttackQueues[queueIdx];
    auto &queueToWorkOn = attackToWorkOn.queue;

    // refilling it from scratch
    ReFillAttackQueueFromScratch(queueIdx);

    // 60fps => ~10 border expansions / 1s
    const int maxPixelsPerFrame = 100;

    for (int i = 0; i < maxPixelsPerFrame && !queueToWorkOn.empty() && attackToWorkOn.troops > 0; i++) {
        Pixel *newP = queueToWorkOn.front();

        if (newP->playerId != _allOnGoingAttackQueues[queueIdx].targetPlayerId) continue;

        attackToWorkOn.troops--;

        // randomly don't get the pixel even though lost troops for it
        if (!newP->acceptRandomly()) continue;

        queueToWorkOn.pop();
        newP->queuedUpForAttack = false;
        GetOwnershipOfPixel(newP);

        // update attack queue
        for (Pixel *neighbor: newP->GetNeighbors()) {
            if (attackToWorkOn.troops <= 0) break; // no new Pixels
            if (neighbor->playerId == _id
                || neighbor->queuedUpForAttack
                || neighbor->invasionAcceptProbability == 0)
                continue;

            neighbor->queuedUpForAttack = true;
            queueToWorkOn.push(neighbor);
        }
    }
}

void Player::ReFillAttackQueueFromScratch(const int queueIdx) {
    // clear the old one
    while (!_allOnGoingAttackQueues[queueIdx].queue.empty()) {
        Pixel *p = _allOnGoingAttackQueues[queueIdx].queue.front();
        p->queuedUpForAttack = false;
        _allOnGoingAttackQueues[queueIdx].queue.pop();
    }

    // refill
    const int target = _allOnGoingAttackQueues[queueIdx].targetPlayerId;
    for (Pixel *borderPixel: _borderPixels) {
        for (Pixel *potentialEnemyBorderPixel: borderPixel->GetNeighbors()) {
            if (potentialEnemyBorderPixel->queuedUpForAttack ||
                potentialEnemyBorderPixel->playerId != target ||
                potentialEnemyBorderPixel->invasionAcceptProbability == 0)
                continue;

            _allOnGoingAttackQueues[queueIdx].queue.push(potentialEnemyBorderPixel);
            potentialEnemyBorderPixel->queuedUpForAttack = true;
        }
    }
}

void Player::GetOwnershipOfPixel(Pixel *newP) {
    Player &attacker = *this;
    attacker._allPixels.insert(newP);

    if (newP->playerId != -1) {
        Player &defender = G::players[newP->playerId];
        defender.LoseOwnershipOfPixel(newP, false);
    }
    newP->playerId = _id;

    attacker.MarkPixelAsDirty(newP);

    ImageDrawPixel(&G::territoryImage, newP->x, newP->y, _color);

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
    }

    // die if too small
    if (_allPixels.empty()) {
        UpdateAllDirty();
        _dead = true;
    }
}
