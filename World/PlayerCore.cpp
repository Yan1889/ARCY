//
// Created by yanam on 14.07.2025.
//

#include "Globals.h"
#include "Player.h"

void Player::Expand(const int target, const float percentage) {
    const int newPeopleLeaving = _population * percentage;
    _population -= newPeopleLeaving;

    // ----- init/find an attack queue -----
    int queueIdx;

    auto iter = _attackedPlayerIdToQueueIdxMap.find(target);
    if (iter == _attackedPlayerIdToQueueIdxMap.end()) {
        queueIdx = _allOnGoingAttackQueues.size();
        _allOnGoingAttackQueues.push_back({
            target,
            newPeopleLeaving,
            {}
        });
        _attackedPlayerIdToQueueIdxMap[target] = queueIdx;
    } else {
        queueIdx = iter->second;
        _allOnGoingAttackQueues[queueIdx].troops += newPeopleLeaving;
    }
    // ----- fill attack queue -----
    for (Pixel *borderPixel: _borderPixels) {
        for (Pixel *potentialEnemyBorderPixel: borderPixel->GetNeighbors()) {
            if (potentialEnemyBorderPixel->queuedUpForAttack ||
                potentialEnemyBorderPixel->playerId != target ||
                potentialEnemyBorderPixel->invasionAcceptProbability == 0)
                continue;

            _allOnGoingAttackQueues[queueIdx].queue.push(potentialEnemyBorderPixel);
            potentialEnemyBorderPixel->queuedUpForAttack = true;
            if (_allOnGoingAttackQueues[queueIdx].troops <= 0) return;
        }
    }
}


void Player::ProcessAttackQueue(const int queueIdx) {
    AttackQueue &attackToWorkOn = _allOnGoingAttackQueues[queueIdx];
    auto &queueToWorkOn = attackToWorkOn.queue;

    // 60fps => ~10 border expansions / 1s
    const int maxPixelsPerFrame = 100;

    for (int i = 0; i < maxPixelsPerFrame && !queueToWorkOn.empty() && attackToWorkOn.troops > 0; i++) {
        Pixel *newP = queueToWorkOn.front();
        queueToWorkOn.pop();
        newP->queuedUpForAttack = false;

        if (newP->playerId != _allOnGoingAttackQueues[queueIdx].targetPlayerId) continue;
        attackToWorkOn.troops--;

        GetOwnershipOfPixel(newP);

        // update attack queue
        const auto &neighbors = newP->GetNeighbors();
        for (Pixel *neighbor: neighbors) {
            if (attackToWorkOn.troops <= 0) break; // no new Pixels

            if (neighbor->playerId == _id || neighbor->queuedUpForAttack) continue;

            if (neighbor->invasionAcceptProbability == 0) continue; // water or mountain are impossible

            neighbor->queuedUpForAttack = true;
            queueToWorkOn.push(neighbor);
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

    // attacker.UpdateBorderAroundPixel(newP);
    _pixelsToBeUpdated.insert(newP);
    for (Pixel* p : newP->GetNeighbors()) _pixelsToBeUpdated.insert(p);

    G::ChangeColorOfPixel(newP, _color);

    // center
    AddPixelToCenter(newP);
}

void Player::UpdateBorderAroundPixel(Pixel *pixel) {
    UpdateBorderStatusOfPixel(pixel);

    const std::vector<Pixel *> &affectedPixels = pixel->GetNeighbors();
    for (Pixel *neighbor: affectedPixels) {
        UpdateBorderStatusOfPixel(neighbor);
    }
}

void Player::UpdateBorderStatusOfPixel(Pixel *pixel) {
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

void Player::LoseOwnershipOfPixel(Pixel *pixel, bool updateTextureToo) {
    _allPixels.erase(pixel);

    pixel->playerId = -1;

    UpdateBorderAroundPixel(pixel);
    RemovePixelFromCenter(pixel);

    if (updateTextureToo) {
        G::ChangeColorOfPixel(pixel, BLANK);
    }

    // die if too small
    if (_allPixels.size() == 0) {
        _dead = true;
    }
}
