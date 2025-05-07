#include "enemies.h"
#include "map.h" // Đảm bảo include map.h
#include <random>
#include <ctime>   // Cho std::time (mặc dù rand() không cần time nếu đã seed ở chỗ khác)
#include <iostream> // Để debug (tùy chọn)

Enemy::Enemy(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y)
    : mRenderer(renderer),
    mTexture(texture),
    mX(x),
    mY(y),
    mWidth(40), // Kích thước mặc định ban đầu
    mHeight(40),// Kích thước mặc định ban đầu
    mSpeed(100.0f),
    mDirection(static_cast<Direction>(rand() % 4)),
    mDirectionChangeTimer(0.0f),
    mDirectionChangeCooldown(2.0f)
{
    // Query texture để lấy kích thước thực tế nếu có texture
    // Điều này sẽ ghi đè mWidth, mHeight mặc định nếu texture có kích thước khác
    if (mTexture) {
        SDL_QueryTexture(mTexture, nullptr, nullptr, &mWidth, &mHeight);
    }
    // Sau đó, setSize từ Game.cpp sẽ đặt kích thước cuối cùng dựa trên tileSize
}

// >>> THÊM TRIỂN KHAI CHO SETSIZE <<<
void Enemy::setSize(int width, int height) {
    mWidth = width;
    mHeight = height;
}

// Find a safe spawn position that doesn't overlap with walls
bool Enemy::findSafePosition(Map* map) {
    if (!map) { // Kiểm tra map null
        std::cerr << "Error: Map pointer is null in Enemy::findSafePosition" << std::endl;
        return false;
    }
    int tileSize = map->getTileSize();
    int maxRows = map->getRows();
    int maxCols = map->getColumns();

    if (maxCols <= 2 || maxRows <= 2) { // Kiểm tra kích thước map hợp lệ
        std::cerr << "Error: Map is too small to find a safe position." << std::endl;
        return false; // Không thể tìm vị trí an toàn trong map quá nhỏ
    }


    for (int attempts = 0; attempts < 100; attempts++) {
        // Generate random position (keeping a bit away from edges)
        // Đảm bảo (maxCols - 2) và (maxRows - 2) không âm
        int col = 1 + (rand() % (maxCols - 2));
        int row = 1 + (rand() % (maxRows - 2));

        int testX = col * tileSize;
        int testY = row * tileSize;

        // Kiểm tra xem vị trí này có trống không
        // Chú ý: logic isColliding của Map sẽ đơn giản hơn
        // Nếu bạn muốn kiểm tra từng ô mà enemy chiếm giữ:
        if (!map->isColliding(testX, testY, mWidth, mHeight)) {
            mX = testX;
            mY = testY;
            return true;
        }
    }
    std::cerr << "Warning: Enemy::findSafePosition failed after 100 attempts." << std::endl;
    return false; // Không tìm thấy vị trí an toàn
}

void Enemy::update(float deltaTime, Map* map) {
    if (!map) return; // Không làm gì nếu không có map

    mDirectionChangeTimer += deltaTime;

    if (mDirectionChangeTimer >= mDirectionChangeCooldown) {
        changeDirection();
        mDirectionChangeTimer = 0.0f;
    }

    int prevX = mX;
    int prevY = mY;

    int moveAmount = static_cast<int>(mSpeed * deltaTime);

    switch (mDirection) {
    case UP:    mY -= moveAmount; break;
    case DOWN:  mY += moveAmount; break;
    case LEFT:  mX -= moveAmount; break;
    case RIGHT: mX += moveAmount; break;
    }

    // Kiểm tra va chạm với map
    if (map->isColliding(mX, mY, mWidth, mHeight)) {
        mX = prevX;
        mY = prevY;
        changeDirection(); // Đổi hướng khi va chạm
    }

    // Giữ enemy trong phạm vi map (sau khi đã giải quyết va chạm tường)
    // Điều này có thể không cần thiết nếu logic isColliding của map đã xử lý biên
    int mapPixelWidth = map->getColumns() * map->getTileSize();
    int mapPixelHeight = map->getRows() * map->getTileSize();

    if (mX < 0) { mX = 0; changeDirection(); }
    if (mX + mWidth > mapPixelWidth) { mX = mapPixelWidth - mWidth; changeDirection(); }
    if (mY < 0) { mY = 0; changeDirection(); }
    if (mY + mHeight > mapPixelHeight) { mY = mapPixelHeight - mHeight; changeDirection(); }
}

void Enemy::render() {
    if (mRenderer && mTexture) {
        SDL_Rect destRect = { mX, mY, mWidth, mHeight };
        SDL_RenderCopy(mRenderer, mTexture, nullptr, &destRect);
    }
    // Bạn có thể thêm một hình chữ nhật màu để vẽ nếu không có texture (cho debug)
    // else if (mRenderer) {
    //     SDL_SetRenderDrawColor(mRenderer, 255, 0, 0, 255); // Màu đỏ
    //     SDL_Rect fillRect = { mX, mY, mWidth, mHeight };
    //     SDL_RenderFillRect(mRenderer, &fillRect);
    // }
}

void Enemy::changeDirection() {
    Direction newDirection;
    int attempts = 0; // Tránh vòng lặp vô hạn nếu chỉ có 1 hướng khả thi (hiếm)
    do {
        newDirection = static_cast<Direction>(rand() % 4);
        attempts++;
    } while (newDirection == mDirection && attempts < 8); // Đảm bảo hướng mới khác (nếu có thể)

    mDirection = newDirection;
}

// Phương thức canMoveInDirection có thể hữu ích cho logic AI phức tạp hơn
// Hiện tại nó không được sử dụng tích cực trong logic update ở trên
// nhưng có thể giữ lại cho tương lai.
bool Enemy::canMoveInDirection(Direction dir, Map* map) {
    if (!map) return false;

    // int tileSize = map->getTileSize(); // Không cần thiết nếu dùng isColliding
    int nextX = mX;
    int nextY = mY;
    int moveStep = 1; // Kiểm tra một khoảng nhỏ phía trước

    switch (dir) {
    case UP:    nextY -= moveStep; break;
    case DOWN:  nextY += moveStep; break;
    case LEFT:  nextX -= moveStep; break;
    case RIGHT: nextX += moveStep; break;
    }

    return !map->isColliding(nextX, nextY, mWidth, mHeight);
}