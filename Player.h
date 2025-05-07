#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include <vector>
#include <memory>
#include "enemies.h" 
#include "bomb.h"   

class Map;

class Player {
public:
    Player(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y, Map* mapRef); 
    ~Player() = default;

    void handleEvent(SDL_Event& e);
    void update(float deltaTime);
    void render();

   

    void setMap(Map* map); 

    int getX() const { return mX; }
    int getY() const { return mY; }
    int getWidth() const { return mWidth; }
    int getHeight() const { return mHeight; }

    void setPosition(int x, int y);
    void setSpeed(float newSpeed); // << THÊM HÀM NÀY


private:
    SDL_Renderer* mRenderer;
    SDL_Texture* mTexture;

    int mX, mY;
    int mWidth, mHeight;
    int mVelX, mVelY;
    float mSpeed;

    bool mMovingUp;
    bool mMovingDown;
    bool mMovingLeft;
    bool mMovingRight;

    std::vector<SDL_Rect> mSpriteClips;
    float mFrameTime;
    int mCurrentFrame;
    int mTotalFrames;
    Direction mFacingDirection;

    Map* mMap; 

};

#endif // PLAYER_H
