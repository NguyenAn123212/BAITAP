#include "enemies.h"
#include "map.h" 
#include <random>
#include <ctime>   
#include <iostream> 

Enemy::Enemy(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y)
    : mRenderer(renderer),
    mTexture(texture),
    mX(x),
    mY(y),
    mWidth(40), 
    mHeight(40),
    mSpeed(100.0f),
    mDirection(static_cast<Direction>(rand() % 4)),
    mDirectionChangeTimer(0.0f),
    mDirectionChangeCooldown(2.0f)
{
   
    if (mTexture) {
        SDL_QueryTexture(mTexture, nullptr, nullptr, &mWidth, &mHeight);
    }
   
}


void Enemy::setSize(int width, int height) {
    mWidth = width;
    mHeight = height;
}

// Find a safe spawn position that doesn't overlap with walls
bool Enemy::findSafePosition(Map* map) {
    if (!map) { 
        std::cerr << "Error: Map pointer is null in Enemy::findSafePosition" << std::endl;
        return false;
    }
    int tileSize = map->getTileSize();
    int maxRows = map->getRows();
    int maxCols = map->getColumns();

    if (maxCols <= 2 || maxRows <= 2) { 
        std::cerr << "Error: Map is too small to find a safe position." << std::endl;
        return false; 
    }


    for (int attempts = 0; attempts < 100; attempts++) {
       
        int col = 1 + (rand() % (maxCols - 2));
        int row = 1 + (rand() % (maxRows - 2));

        int testX = col * tileSize;
        int testY = row * tileSize;

        
        if (!map->isColliding(testX, testY, mWidth, mHeight)) {
            mX = testX;
            mY = testY;
            return true;
        }
    }
    std::cerr << "Warning: Enemy::findSafePosition failed after 100 attempts." << std::endl;
    return false; 
}

void Enemy::update(float deltaTime, Map* map) {
    if (!map) return;

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

   
    if (map->isColliding(mX, mY, mWidth, mHeight)) {
        mX = prevX;
        mY = prevY;
        changeDirection(); // Đổi hướng khi va chạm
    }

   
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
}
}

void Enemy::changeDirection() {
    Direction newDirection;
    int attempts = 0; 
    do {
        newDirection = static_cast<Direction>(rand() % 4);
        attempts++;
    } while (newDirection == mDirection && attempts < 8); 

    mDirection = newDirection;
}


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
