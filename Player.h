#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include <vector>
#include <memory>
#include "enemies.h" // Sử dụng enum Direction từ đây (nếu có)
#include "bomb.h"    // Sử dụng lớp Bomb

// Forward declaration
class Map;

class Player {
public:
    Player(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y, Map* mapRef); // Thêm mapRef
    ~Player() = default;

    void handleEvent(SDL_Event& e);
    void update(float deltaTime);
    void render();

    // Player không trực tiếp quản lý việc render list bomb nữa, Game sẽ làm điều đó.
    // void renderBomb(SDL_Texture* bombTexture, SDL_Texture* explosionTexture); 

    // Player sẽ yêu cầu Game đặt bom, thay vì tự đặt.
    // void placeBomb(); 
    // bool canPlaceBomb() const; // Game sẽ quyết định dựa trên GameOptions

    void setMap(Map* map); // Có thể vẫn cần nếu Player tương tác trực tiếp với Map

    // Getters
    int getX() const { return mX; }
    int getY() const { return mY; }
    int getWidth() const { return mWidth; }
    int getHeight() const { return mHeight; }
    // std::shared_ptr<Bomb> getBomb() const; // Player không còn quản lý bomb trực tiếp

    // Setters
    void setPosition(int x, int y);
    void setSpeed(float newSpeed); // << THÊM HÀM NÀY

    // Các thuộc tính liên quan đến bom như mBombSize, mBombFuseTime, mBombRange
    // sẽ được Game quản lý thông qua GameOptions khi tạo đối tượng Bomb.
    // Player không cần lưu trữ chúng nữa nếu việc tạo Bomb do Game đảm nhiệm.

private:
    SDL_Renderer* mRenderer;
    SDL_Texture* mTexture;
    // SDL_Texture* mBombTexture;      // Game sẽ truyền khi render bomb
    // SDL_Texture* mExplosionTexture; // Game sẽ truyền khi render bomb

    int mX, mY;
    int mWidth, mHeight;
    int mVelX, mVelY;
    float mSpeed; // << THAY ĐỔI: không còn là const, sẽ được set từ Game

    bool mMovingUp;
    bool mMovingDown;
    bool mMovingLeft;
    bool mMovingRight;

    std::vector<SDL_Rect> mSpriteClips;
    float mFrameTime;
    int mCurrentFrame;
    int mTotalFrames;
    Direction mFacingDirection; // Giả sử enum Direction được định nghĩa (ví dụ trong enemies.h hoặc file riêng)

    // std::shared_ptr<Bomb> mBomb; // Player không còn quản lý một bomb duy nhất. Game sẽ quản lý danh sách bomb.
    Map* mMap; // Con trỏ tới map để kiểm tra va chạm hoặc thông tin khác

    // Các hằng số liên quan đến bom được chuyển sang GameOptions hoặc truyền khi tạo Bomb
    // const int mBombSize = 25;
    // const float mBombFuseTime = 3.0f;
    // const int mBombRange = 1;
};

#endif // PLAYER_H
