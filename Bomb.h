#ifndef BOMB_H
#define BOMB_H

#include <SDL.h>
#include <vector>

class Map;

struct ExplosionPart {
    int x;
    int y;
};

struct Explosion {
    std::vector<ExplosionPart> parts;
};

class Bomb {
public:
    Bomb(int x, int y, int size, float fuseTime, int explosionRange);
    ~Bomb() = default;

    void update(float deltaTime);
    void render(SDL_Renderer* renderer, SDL_Texture* bombTexture, SDL_Texture* explosionTexture);

    void setMap(Map* map) { mMap = map; }
    void createExplosion(); // Đảm bảo hàm này công khai nếu Game cần gọi trực tiếp

    // Getters
    bool isExploding() const { return mExploding; }
    bool isDone() const { return mDone; }
    int getX() const { return mX; }
    int getY() const { return mY; }
    int getSize() const { return mSize; }
    const Explosion& getExplosion() const { return mExplosion; }
    Explosion& getExplosion() { return mExplosion; }

    bool hasExplosionSoundPlayed() const { return mExplosionSoundPlayed; } // << Getter cho cờ âm thanh
    void setExplosionSoundPlayed(bool played) { mExplosionSoundPlayed = played; } // << Setter cho cờ âm thanh


private:
    int mX, mY;
    int mSize;
    float mFuseTime;
    float mTimer;
    int mExplosionRange;
    float mExplosionDuration;
    float mExplosionTimer;
    bool mExploding;
    bool mDone;
    Map* mMap = nullptr;

    int mCurrentFrame;
    float mFrameTime;
    const float mFrameDuration = 0.2f;
    const int mTotalBombFrames = 3;

    Explosion mExplosion;
    bool mExplosionSoundPlayed; // << Cờ để theo dõi âm thanh nổ đã phát chưa
};

#endif // BOMB_H
