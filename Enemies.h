#ifndef ENEMIES_H
#define ENEMIES_H

#include <SDL.h>
#include "map.h" // Đảm bảo Map được include nếu Enemy tương tác trực tiếp với nó



enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class Enemy {
public:
    Enemy(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y);
    ~Enemy() = default; 

    void update(float deltaTime, Map* map);
    void render();
    bool findSafePosition(Map* map);

    int getX() const { return mX; }
    int getY() const { return mY; }
    int getWidth() const { return mWidth; }
    int getHeight() const { return mHeight; }

    void setPosition(int x, int y) { mX = x; mY = y; }
    void setSize(int width, int height); // <<< THÊM DÒNG NÀY
    void changeDirection();

private:
    SDL_Renderer* mRenderer;
    SDL_Texture* mTexture;

    int mX, mY;
    int mWidth, mHeight;
    float mSpeed;
    Direction mDirection;

    float mDirectionChangeTimer;
    float mDirectionChangeCooldown;

    bool canMoveInDirection(Direction dir, Map* map);
};

#endif 
