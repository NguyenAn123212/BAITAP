#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <vector>
#include <memory> // Cần cho std::unique_ptr
#include <string>
#include <array>
#include <iomanip> // Cần cho std::setw và std::setfill
#include <sstream> // Cần cho std::ostringstream

#include "Menu.h"          // Menu chính
#include "GameOptions.h"   // Cấu trúc lưu trữ tùy chọn game
#include "OptionsMenu.h"   // Menu tùy chọn

// Forward declarations cho các lớp game khác
class Player;
class Map;
class Bomb;
class Enemy;
struct Explosion; // Định nghĩa Explosion nên ở Bomb.h

// Enum trạng thái của game
enum class GameState {
    MAIN_MENU,
    OPTIONS_MENU,
    PLAYING,
    GAME_OVER_MENU
};

class Game {
public:
    Game(SDL_Renderer* renderer, int screenWidth, int screenHeight);
    ~Game();

    bool initialize();
    void handleEvent(SDL_Event& e);
    void update(float deltaTime);
    void render();

    bool checkCollision(SDL_Rect a, SDL_Rect b);
    bool isColliding(int x, int y, int width, int height); // Đảm bảo hàm này được khai báo nếu Game cần

private:
    SDL_Renderer* mRenderer;
    int mScreenWidth;
    int mScreenHeight;
    bool mGameOver;

    GameState mCurrentState;

    std::unique_ptr<Menu> mMainMenu;
    std::unique_ptr<OptionsMenu> mOptionsMenu;

    GameOptions mGameSettings;

    std::unique_ptr<Player> mPlayer;
    std::unique_ptr<Map> mMap;
    std::vector<std::unique_ptr<Bomb>> mBombs;
    std::vector<std::unique_ptr<Enemy>> mEnemies;

    SDL_Texture* mPlayerTexture;
    SDL_Texture* mEnemyTexture;
    SDL_Texture* mBackgroundTexture;
    SDL_Texture* mHardWallTexture;
    SDL_Texture* mBorderWallTexture;
    std::array<SDL_Texture*, 3> mSoftWallTextures;
    SDL_Texture* mBombTexture;
    SDL_Texture* mExplosionTexture;

    TTF_Font* mGameFont;
    TTF_Font* mUiFont;

    Mix_Music* mMenuMusic;
    Mix_Music* mIngameMusic;
    Mix_Chunk* mBombExplosionSound;

    int mCurrentScore;
    int mHighScore;
    float mGameTimerSeconds;
    const float MAX_GAME_TIME_SECONDS = 180.0f;
    SDL_Texture* mScoreTextTexture;
    SDL_Texture* mTimerTextTexture;
    SDL_Color mUiTextColor;

    // Các thành phần của Game Over Menu
    SDL_Texture* mGameOverStateTitleTexture;
    SDL_Texture* mFinalScoreTextTexture;
    SDL_Texture* mHighScoreTextTexture;
    SDL_Rect mContinueButtonRect;      // Khai báo cho nút
    SDL_Rect mEndGameButtonRect;       // Khai báo cho nút
    SDL_Texture* mContinueButtonTexture; // Khai báo cho texture nút
    SDL_Texture* mEndGameButtonTexture;  // Khai báo cho texture nút

    // --- Hàm tiện ích nội bộ ---
    SDL_Texture* loadTexture(const std::string& path);
    // Khai báo cho createTextTexture, đảm bảo tên tham số khớp (ví dụ: fontToUse)
    SDL_Texture* createTextTexture(const std::string& text, SDL_Color color, TTF_Font* fontToUse);

    // --- Quản lý trạng thái và logic game ---
    void startGame();
    void resetGame();
    void transitionToMainMenu();
    void transitionToOptionsMenu();
    void transitionToGameOver();

    void calculateFinalScore();
    void updateScoreDisplay();
    void updateTimerDisplay();
    void loadHighScore();
    void saveHighScore();

    // --- Quản lý audio ---
    bool loadAudio();
    void playIngameMusic();
    void stopMusic();
    void playBombSoundEffect();

    // --- Xử lý sự kiện cho các menu con ---
    void handleMainMenuEvents(SDL_Event& e);
    void handleOptionsMenuEvents(SDL_Event& e);
    void handleGameOverMenuEvents(SDL_Event& e);

    // --- Render các phần khác nhau ---
    void renderPlayingState();
    void renderScoreAndTimer();
    void renderGameOverMenu();

    // --- Logic cụ thể của game ---
    void placeBomb();
    void createEnemiesBasedOnOptions();
    void initializeGameOverMenuAssets(); // << KHAI BÁO CHO HÀM NÀY
};

#endif // GAME_H
