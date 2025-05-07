#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <vector>
#include <memory> 
#include <string>
#include <array>
#include <iomanip> 
#include <sstream> 

#include "Menu.h"         
#include "GameOptions.h"   
#include "OptionsMenu.h"  

class Player;
class Map;
class Bomb;
class Enemy;
struct Explosion; 

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
    bool isColliding(int x, int y, int width, int height);

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

    SDL_Texture* mGameOverStateTitleTexture;
    SDL_Texture* mFinalScoreTextTexture;
    SDL_Texture* mHighScoreTextTexture;
    SDL_Rect mContinueButtonRect;    
    SDL_Rect mEndGameButtonRect;      
    SDL_Texture* mContinueButtonTexture; 
    SDL_Texture* mEndGameButtonTexture;  

    SDL_Texture* loadTexture(const std::string& path);
    SDL_Texture* createTextTexture(const std::string& text, SDL_Color color, TTF_Font* fontToUse);

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

    bool loadAudio();
    void playIngameMusic();
    void stopMusic();
    void playBombSoundEffect();

    void handleMainMenuEvents(SDL_Event& e);
    void handleOptionsMenuEvents(SDL_Event& e);
    void handleGameOverMenuEvents(SDL_Event& e);

    void renderPlayingState();
    void renderScoreAndTimer();
    void renderGameOverMenu();

    void placeBomb();
    void createEnemiesBasedOnOptions();
    void initializeGameOverMenuAssets();
};

#endif // GAME_H
