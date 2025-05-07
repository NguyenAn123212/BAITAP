#ifndef GAME_OPTIONS_H
#define GAME_OPTIONS_H

#include <algorithm>

struct GameOptions {
    int playerSpeedLevel;
    float actualPlayerSpeed;

    int enemyCount;

    int playerMaxActiveBombs;

    int playerBombRange;

    GameOptions()
        : playerSpeedLevel(5),
        enemyCount(3),
        playerMaxActiveBombs(1),
        playerBombRange(1)
    {
        updateActualPlayerSpeed();
    }

    void updateActualPlayerSpeed() {
        int level = std::max(1, std::min(9, playerSpeedLevel));
        actualPlayerSpeed = 100.0f + (level - 1) * 25.0f;
    }

    void setPlayerSpeedLevel(int level) {
        playerSpeedLevel = std::max(1, std::min(9, level));
        updateActualPlayerSpeed();
    }

    void setEnemyCount(int count) {
        enemyCount = std::max(1, std::min(9, count));
    }

    void setPlayerMaxActiveBombs(int count) {
        playerMaxActiveBombs = std::max(1, std::min(5, count));
    }

    void setPlayerBombRange(int range) {
        playerBombRange = std::max(1, std::min(5, range));
    }
};

#endif // GAME_OPTIONS_H
