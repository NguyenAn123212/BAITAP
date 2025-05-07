#ifndef GAME_OPTIONS_H
#define GAME_OPTIONS_H

#include <algorithm> // Cho std::min và std::max

// Cấu trúc để lưu trữ các tùy chọn có thể tùy chỉnh của game
struct GameOptions {
    // 1. Tốc độ Player (cấp độ từ 1-9)
    int playerSpeedLevel;
    float actualPlayerSpeed; // Tốc độ thực tế được tính từ playerSpeedLevel

    // 2. Số lượng quái (từ 1-9)
    int enemyCount;

    // 3. Số lượng bomb tối đa người chơi có thể đặt cùng lúc (từ 1-5)
    int playerMaxActiveBombs;

    // 4. Phạm vi nổ của bomb (ví dụ: số ô, từ 1-5)
    int playerBombRange;

    // Giá trị mặc định cho các tùy chọn
    GameOptions()
        : playerSpeedLevel(5), // Mặc định cấp 5
        enemyCount(3),       // Mặc định 3 quái
        playerMaxActiveBombs(1), // Mặc định 1 bom
        playerBombRange(1)     // Mặc định phạm vi 1 ô
    {
        updateActualPlayerSpeed(); // Tính tốc độ thực tế ban đầu
    }

    // Hàm cập nhật tốc độ thực tế của người chơi dựa trên cấp độ tốc độ
    void updateActualPlayerSpeed() {
        // Ánh xạ cấp độ tốc độ (1-9) sang một giá trị tốc độ float
        // Ví dụ: cấp 1 ~ 100.0f, cấp 5 ~ 200.0f, cấp 9 ~ 300.0f
        // Công thức ví dụ: base_speed + (level - 1) * step
        // Đảm bảo playerSpeedLevel nằm trong khoảng hợp lệ trước khi tính toán
        int level = std::max(1, std::min(9, playerSpeedLevel)); // Giới hạn level trong [1, 9]
        actualPlayerSpeed = 100.0f + (level - 1) * 25.0f;
    }

    // Các hàm setter với kiểm tra giá trị đầu vào
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
        playerBombRange = std::max(1, std::min(5, range)); // Giả sử phạm vi tối đa là 5
    }
};

#endif // GAME_OPTIONS_H
