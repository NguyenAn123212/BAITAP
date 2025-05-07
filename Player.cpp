#include "player.h"
#include "map.h" // Cần cho tương tác với map
// #include "enemies.h" // Đã được include trong player.h nếu Direction ở đó

Player::Player(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y, Map* mapRef)
    : mRenderer(renderer),
    mTexture(texture),
    mX(x),
    mY(y),
    mWidth(25),     // Kích thước cố định hoặc có thể thay đổi bằng power-up
    mHeight(25),
    mVelX(0),
    mVelY(0),
    mSpeed(200.0f), // Tốc độ mặc định, sẽ được ghi đè bởi setSpeed()
    mMovingUp(false),
    mMovingDown(false),
    mMovingLeft(false),
    mMovingRight(false),
    mFrameTime(0.0f),
    mCurrentFrame(0),
    mTotalFrames(4),
    mFacingDirection(Direction::DOWN), // Sử dụng Direction:: nếu là enum class
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
            // Việc đặt bom (SDLK_SPACE) sẽ được xử lý ở lớp Game,
            // vì Game quản lý danh sách bom và các tùy chọn liên quan đến bom.
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
        // Di chuyển chéo, chuẩn hóa tốc độ
        float factor = 0.7071f; // 1/sqrt(2)
        mX += static_cast<int>(mVelX * mSpeed * factor * deltaTime);
        mY += static_cast<int>(mVelY * mSpeed * factor * deltaTime);
    }
    else {
        mX += static_cast<int>(mVelX * mSpeed * deltaTime);
        mY += static_cast<int>(mVelY * mSpeed * deltaTime);
    }

    // Giữ player trong màn hình (hoặc để Map xử lý va chạm với biên)
    // Ví dụ đơn giản:
    if (mMap) { // Giả sử Map có getColumns/Rows và getTileSize
        int mapPixelWidth = mMap->getColumns() * mMap->getTileSize();
        int mapPixelHeight = mMap->getRows() * mMap->getTileSize();
        if (mX < 0) mX = 0;
        if (mY < 0) mY = 0;
        if (mX + mWidth > mapPixelWidth) mX = mapPixelWidth - mWidth;
        if (mY + mHeight > mapPixelHeight) mY = mapPixelHeight - mHeight;
    }


    // Animation
    if (mVelX != 0 || mVelY != 0) {
        mFrameTime += deltaTime;
        if (mFrameTime > 0.15f) { // Tốc độ animation
            mFrameTime = 0;
            mCurrentFrame = (mCurrentFrame + 1) % mTotalFrames;
        }
    }
    else {
        mCurrentFrame = 0; // Frame đứng yên
    }
}

void Player::render() {
    if (mTexture && !mSpriteClips.empty()) {
        // Xác định sprite clip dựa trên hướng nhìn và frame hiện tại
        int clipIndex = static_cast<int>(mFacingDirection) * mTotalFrames + mCurrentFrame;
        if (clipIndex < 0 || clipIndex >= mSpriteClips.size()) {
            clipIndex = 0; // Fallback nếu có lỗi
        }
        SDL_Rect* currentClip = &mSpriteClips[clipIndex];
        SDL_Rect destRect = { mX, mY, mWidth, mHeight };
        SDL_RenderCopy(mRenderer, mTexture, currentClip, &destRect);
    }
    else if (mTexture) { // Nếu không có sprite sheet, vẽ toàn bộ texture
        SDL_Rect destRect = { mX, mY, mWidth, mHeight };
        SDL_RenderCopy(mRenderer, mTexture, nullptr, &destRect);
    }
}

void Player::setPosition(int x, int y) {
    mX = x;
    mY = y;
}

// Player không còn trực tiếp đặt bom hoặc render bom. Lớp Game sẽ làm điều này.
// void Player::placeBomb() { ... }
// void Player::renderBomb(...) { ... }
