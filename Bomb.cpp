#include "bomb.h"
#include "map.h"
#include <iostream> // Để debug

Bomb::Bomb(int x, int y, int size, float fuseTime, int explosionRange)
    : mX(x),
    mY(y),
    mSize(size),
    mFuseTime(fuseTime),
    mTimer(0.0f),
    mExplosionRange(explosionRange),
    mExplosionDuration(0.8f),
    mExplosionTimer(0.0f),
    mExploding(false),
    mDone(false),
    mMap(nullptr),
    mCurrentFrame(0),
    mFrameTime(0.0f),
    mExplosionSoundPlayed(false) 
{
}

void Bomb::update(float deltaTime) {
    if (mDone) return;

    if (!mExploding) {
        mTimer += deltaTime;
        mFrameTime += deltaTime;
        if (mFrameTime >= mFrameDuration) {
            mFrameTime = 0.0f;
            mCurrentFrame = (mCurrentFrame + 1) % mTotalBombFrames;
        }

        if (mTimer >= mFuseTime) {
            mExploding = true;
            mTimer = 0.0f; 
            mCurrentFrame = 0; 
            createExplosion();
           
        }
    }
    else {
        mExplosionTimer += deltaTime;
        if (mExplosionTimer >= mExplosionDuration) {
            mDone = true;
        }
    }
}

void Bomb::render(SDL_Renderer* renderer, SDL_Texture* bombTexture, SDL_Texture* explosionTexture) {
    if (mDone) return;

    if (!mExploding) {
        if (!bombTexture) return;
        int frameWidth = 0, frameHeight = 0;
        SDL_QueryTexture(bombTexture, nullptr, nullptr, &frameWidth, &frameHeight);

        if (mTotalBombFrames > 0) { 
            frameWidth /= mTotalBombFrames;
        }
        else {
        }


        SDL_Rect srcRect = { mCurrentFrame * frameWidth, 0, frameWidth, frameHeight };
        SDL_Rect destRect = { mX, mY, mSize, mSize };
        SDL_RenderCopy(renderer, bombTexture, &srcRect, &destRect);
    }
    else {
        if (!explosionTexture) return;
        for (const auto& part : mExplosion.parts) {
            SDL_Rect destRect = { part.x, part.y, mSize, mSize };
            SDL_RenderCopy(renderer, explosionTexture, nullptr, &destRect);
        }
    }
}

void Bomb::createExplosion() {
    mExplosion.parts.clear();
    mExplosion.parts.push_back({ mX, mY }); 

    
    int currentExplosionRange = 1; 

   
    for (int i = 1; i <= currentExplosionRange; ++i) {
        int newX = mX + i * mSize;
        int newY = mY;
        if (mMap) {
            int tileRow = newY / mMap->getTileSize();
            int tileCol = newX / mMap->getTileSize();
            if (mMap->getTileType(tileRow, tileCol) == TileType::HARD_WALL) break;
            mExplosion.parts.push_back({ newX, newY });
            if (mMap->getTileType(tileRow, tileCol) == TileType::SOFT_WALL) break;
        }
        else {
            mExplosion.parts.push_back({ newX, newY }); 
        }
    }

    for (int i = 1; i <= currentExplosionRange; ++i) {
        int newX = mX - i * mSize;
        int newY = mY;
        if (mMap) {
            int tileRow = newY / mMap->getTileSize();
            int tileCol = newX / mMap->getTileSize();
            if (mMap->getTileType(tileRow, tileCol) == TileType::HARD_WALL) break;
            mExplosion.parts.push_back({ newX, newY });
            if (mMap->getTileType(tileRow, tileCol) == TileType::SOFT_WALL) break;
        }
        else {
            mExplosion.parts.push_back({ newX, newY });
        }
    }
 
    for (int i = 1; i <= currentExplosionRange; ++i) {
        int newX = mX;
        int newY = mY + i * mSize;
        if (mMap) {
            int tileRow = newY / mMap->getTileSize();
            int tileCol = newX / mMap->getTileSize();
            if (mMap->getTileType(tileRow, tileCol) == TileType::HARD_WALL) break;
            mExplosion.parts.push_back({ newX, newY });
            if (mMap->getTileType(tileRow, tileCol) == TileType::SOFT_WALL) break;
        }
        else {
            mExplosion.parts.push_back({ newX, newY });
        }
    }
  
    for (int i = 1; i <= currentExplosionRange; ++i) {
        int newX = mX;
        int newY = mY - i * mSize;
        if (mMap) {
            int tileRow = newY / mMap->getTileSize();
            int tileCol = newX / mMap->getTileSize();
            if (mMap->getTileType(tileRow, tileCol) == TileType::HARD_WALL) break;
            mExplosion.parts.push_back({ newX, newY });
            if (mMap->getTileType(tileRow, tileCol) == TileType::SOFT_WALL) break;
        }
        else {
            mExplosion.parts.push_back({ newX, newY });
        }
    }
}
