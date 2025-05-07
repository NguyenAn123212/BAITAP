#include "player.h"
#include "map.h" // Cần cho tương tác với map

Player::Player(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y, Map* mapRef)
    : mRenderer(renderer),
    mTexture(texture),
    mX(x),
    mY(y),
    mWidth(25),    
    mHeight(25),
    mVelX(0),
    mVelY(0),
    mSpeed(200.0f), 
    mMovingUp(false),
    mMovingDown(false),
    mMovingLeft(false),
    mMovingRight(false),
    mFrameTime(0.0f),
    mCurrentFrame(0),
    mTotalFrames(4),
    mFacingDirection(Direction::DOWN), 
    mMap(mapRef)
{
    if (mTexture) {
        int textureWidth, textureHeight;
        SDL_QueryTexture(mTexture, nullptr, nullptr, &textureWidth, &textureHeight);
        mSpriteClips.resize(4 * mTotalFrames);
        int frameWidth = textureWidth / mTotalFrames;
        int frameHeight = textureHeight / 4;
        for (int direction = 0; direction < 4; ++direction) {
            for (int frame = 0; frame < mTotalFrames; ++frame) {
                mSpriteClips[direction * mTotalFrames + frame] = {
                    frame * frameWidth,
                    direction * frameHeight,
                    frameWidth,
                    frameHeight
                };
            }
        }
    }
}

void Player::setSpeed(float newSpeed) {
    mSpeed = newSpeed;
}

void Player::setMap(Map* map) {
    mMap = map;
}

void Player::handleEvent(SDL_Event& e) {
    if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
        switch (e.key.keysym.sym) {
        case SDLK_UP:    mMovingUp = true; mFacingDirection = Direction::UP; break;
        case SDLK_DOWN:  mMovingDown = true; mFacingDirection = Direction::DOWN; break;
        case SDLK_LEFT:  mMovingLeft = true; mFacingDirection = Direction::LEFT; break;
        case SDLK_RIGHT: mMovingRight = true; mFacingDirection = Direction::RIGHT; break;
           
        }
    }
    else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
        switch (e.key.keysym.sym) {
        case SDLK_UP:    mMovingUp = false; break;
        case SDLK_DOWN:  mMovingDown = false; break;
        case SDLK_LEFT:  mMovingLeft = false; break;
        case SDLK_RIGHT: mMovingRight = false; break;
        }
    }
}

void Player::update(float deltaTime) {
    mVelX = 0;
    mVelY = 0;

    if (mMovingUp)    mVelY -= 1;
    if (mMovingDown)  mVelY += 1;
    if (mMovingLeft)  mVelX -= 1;
    if (mMovingRight) mVelX += 1;

    if (mVelX != 0 && mVelY != 0) {
        
        float factor = 0.7071f;
        mX += static_cast<int>(mVelX * mSpeed * factor * deltaTime);
        mY += static_cast<int>(mVelY * mSpeed * factor * deltaTime);
    }
    else {
        mX += static_cast<int>(mVelX * mSpeed * deltaTime);
        mY += static_cast<int>(mVelY * mSpeed * deltaTime);
    }

  
    if (mMap) { 
        int mapPixelWidth = mMap->getColumns() * mMap->getTileSize();
        int mapPixelHeight = mMap->getRows() * mMap->getTileSize();
        if (mX < 0) mX = 0;
        if (mY < 0) mY = 0;
        if (mX + mWidth > mapPixelWidth) mX = mapPixelWidth - mWidth;
        if (mY + mHeight > mapPixelHeight) mY = mapPixelHeight - mHeight;
    }


   
    if (mVelX != 0 || mVelY != 0) {
        mFrameTime += deltaTime;
        if (mFrameTime > 0.15f) {
            mFrameTime = 0;
            mCurrentFrame = (mCurrentFrame + 1) % mTotalFrames;
        }
    }
    else {
        mCurrentFrame = 0; 
    }
}

void Player::render() {
    if (mTexture && !mSpriteClips.empty()) {
        int clipIndex = static_cast<int>(mFacingDirection) * mTotalFrames + mCurrentFrame;
        if (clipIndex < 0 || clipIndex >= mSpriteClips.size()) {
            clipIndex = 0; 
        }
        SDL_Rect* currentClip = &mSpriteClips[clipIndex];
        SDL_Rect destRect = { mX, mY, mWidth, mHeight };
        SDL_RenderCopy(mRenderer, mTexture, currentClip, &destRect);
    }
    else if (mTexture) { 
        SDL_Rect destRect = { mX, mY, mWidth, mHeight };
        SDL_RenderCopy(mRenderer, mTexture, nullptr, &destRect);
    }
}

void Player::setPosition(int x, int y) {
    mX = x;
    mY = y;
}


