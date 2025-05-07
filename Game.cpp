#include "game.h"
#include <SDL_image.h>
#include <iostream>
#include <random>
#include <ctime>
#include <algorithm>
#include <iomanip>
#include <sstream>

#include "player.h"
#include "map.h"
#include "bomb.h"
#include "enemies.h"


Game::Game(SDL_Renderer* renderer, int screenWidth, int screenHeight)
    : mRenderer(renderer),
    mScreenWidth(screenWidth),
    mScreenHeight(screenHeight),
    mGameOver(false),
    mCurrentState(GameState::MAIN_MENU),
    mMainMenu(nullptr),
    mOptionsMenu(nullptr),
    mGameSettings(),
    mPlayer(nullptr),
    mMap(nullptr),
    mPlayerTexture(nullptr),
    mEnemyTexture(nullptr),
    mBackgroundTexture(nullptr),
    mHardWallTexture(nullptr),
    mBorderWallTexture(nullptr),
    mBombTexture(nullptr),
    mExplosionTexture(nullptr),
    mGameFont(nullptr),
    mUiFont(nullptr),
    mMenuMusic(nullptr),
    mIngameMusic(nullptr),
    mBombExplosionSound(nullptr),
    mCurrentScore(0),
    mHighScore(0),
    mGameTimerSeconds(MAX_GAME_TIME_SECONDS),
    mScoreTextTexture(nullptr),
    mTimerTextTexture(nullptr),
    mUiTextColor({ 255, 255, 255, 255 }),
    mGameOverStateTitleTexture(nullptr),
    mFinalScoreTextTexture(nullptr),
    mHighScoreTextTexture(nullptr),
    mContinueButtonTexture(nullptr),
    mEndGameButtonTexture(nullptr)

{
    for (auto& texture : mSoftWallTextures) {
        texture = nullptr;
    }
    loadHighScore();
    mGameSettings.updateActualPlayerSpeed();
}

Game::~Game() {
    if (mPlayerTexture) SDL_DestroyTexture(mPlayerTexture);
    if (mEnemyTexture) SDL_DestroyTexture(mEnemyTexture);
    if (mBackgroundTexture) SDL_DestroyTexture(mBackgroundTexture);
    if (mHardWallTexture) SDL_DestroyTexture(mHardWallTexture);
    if (mBorderWallTexture) SDL_DestroyTexture(mBorderWallTexture);
    if (mBombTexture) SDL_DestroyTexture(mBombTexture);
    if (mExplosionTexture) SDL_DestroyTexture(mExplosionTexture);
    for (auto& texture : mSoftWallTextures) {
        if (texture) SDL_DestroyTexture(texture);
    }

    if (mGameFont) TTF_CloseFont(mGameFont);
    if (mUiFont && mUiFont != mGameFont) TTF_CloseFont(mUiFont);
    else if (mUiFont == mGameFont) mUiFont = nullptr;

    if (mMenuMusic) Mix_FreeMusic(mMenuMusic);
    if (mIngameMusic) Mix_FreeMusic(mIngameMusic);
    if (mBombExplosionSound) Mix_FreeChunk(mBombExplosionSound);

    if (mScoreTextTexture) SDL_DestroyTexture(mScoreTextTexture);
    if (mTimerTextTexture) SDL_DestroyTexture(mTimerTextTexture);
    if (mGameOverStateTitleTexture) SDL_DestroyTexture(mGameOverStateTitleTexture);
    if (mFinalScoreTextTexture) SDL_DestroyTexture(mFinalScoreTextTexture);
    if (mHighScoreTextTexture) SDL_DestroyTexture(mHighScoreTextTexture);
    if (mContinueButtonTexture) SDL_DestroyTexture(mContinueButtonTexture);
    if (mEndGameButtonTexture) SDL_DestroyTexture(mEndGameButtonTexture);
}

bool Game::initialize() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    mGameFont = TTF_OpenFont("game_font.otf", 48);
    if (!mGameFont) {
        std::cerr << "Game Error: Failed to load main font 'game_font.otf'. TTF_Error: " << TTF_GetError() << std::endl;
        return false;
    }
    mUiFont = TTF_OpenFont("game_font.otf", 28);
    if (!mUiFont) {
        std::cerr << "Game Warning: Failed to load UI font. Using main font instead." << std::endl;
        mUiFont = mGameFont;
    }

    if (!loadAudio()) {
        std::cerr << "Game Warning: Failed to load some audio assets. Game will continue without them." << std::endl;
    }

    mMainMenu = std::make_unique<Menu>(mRenderer, mGameFont, mScreenWidth, mScreenHeight, mMenuMusic);
    if (!mMainMenu || !mMainMenu->initialize("menu_background.png")) {
        std::cerr << "Game Error: Failed to initialize the main menu!" << std::endl;
        return false;
    }

    mOptionsMenu = std::make_unique<OptionsMenu>(mRenderer, mUiFont, mScreenWidth, mScreenHeight, mGameSettings);
    if (!mOptionsMenu || !mOptionsMenu->initialize()) {
        std::cerr << "Game Error: Failed to initialize the options menu!" << std::endl;
    }

    initializeGameOverMenuAssets();

    transitionToMainMenu();

    updateScoreDisplay();
    updateTimerDisplay();

    return true;
}

SDL_Texture* Game::createTextTexture(const std::string& text, SDL_Color color, TTF_Font* fontToUse) {
    if (!fontToUse || !mRenderer) {
        std::cerr << "Game Error: Cannot create text texture, font or renderer is null. Text: " << text << std::endl;
        return nullptr;
    }
    SDL_Surface* textSurface = TTF_RenderText_Solid(fontToUse, text.c_str(), color);
    if (!textSurface) {
        std::cerr << "Game Error: TTF_RenderText_Solid failed for \"" << text << "\". TTF_Error: " << TTF_GetError() << std::endl;
        return nullptr;
    }
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(mRenderer, textSurface);
    SDL_FreeSurface(textSurface);
    if (!textTexture) {
        std::cerr << "Game Error: SDL_CreateTextureFromSurface failed for \"" << text << "\". SDL_Error: " << SDL_GetError() << std::endl;
    }
    return textTexture;
}

void Game::initializeGameOverMenuAssets() {
    mContinueButtonTexture = createTextTexture("Choi Lai (R)", mUiTextColor, mUiFont);
    mEndGameButtonTexture = createTextTexture("Menu Chinh (M)", mUiTextColor, mUiFont);

    if (!mContinueButtonTexture || !mEndGameButtonTexture) {
        std::cerr << "Game Error: Failed to create Game Over Menu button textures." << std::endl;
    }

    int btnWidth, btnHeight;
    if (mContinueButtonTexture && TTF_SizeText(mUiFont, "Choi Lai (R)", &btnWidth, &btnHeight) == 0) {
        mContinueButtonRect = { (mScreenWidth - btnWidth) / 2, mScreenHeight / 2 + 60, btnWidth, btnHeight };
    }
    else {
        mContinueButtonRect = { (mScreenWidth - 220) / 2, mScreenHeight / 2 + 60, 220, 40 };
    }

    if (mEndGameButtonTexture && TTF_SizeText(mUiFont, "Menu Chinh (M)", &btnWidth, &btnHeight) == 0) {
        mEndGameButtonRect = { (mScreenWidth - btnWidth) / 2, mContinueButtonRect.y + mContinueButtonRect.h + 20, btnWidth, btnHeight };
    }
    else {
        mEndGameButtonRect = { (mScreenWidth - 240) / 2, mContinueButtonRect.y + mContinueButtonRect.h + 20, 240, 40 };
    }
}


void Game::loadHighScore() {
    mHighScore = 0;
    std::cout << "High score loaded (session-based): " << mHighScore << std::endl;
}

void Game::saveHighScore() {
    if (mCurrentScore > mHighScore) {
        mHighScore = mCurrentScore;
        std::cout << "New high score achieved: " << mHighScore << std::endl;
    }
}

void Game::updateScoreDisplay() {
    if (mScoreTextTexture) SDL_DestroyTexture(mScoreTextTexture);
    std::ostringstream scoreStream;
    scoreStream << "Score: " << std::setw(4) << std::setfill('0') << mCurrentScore;
    mScoreTextTexture = createTextTexture(scoreStream.str(), mUiTextColor, mUiFont);
}

void Game::updateTimerDisplay() {
    if (mTimerTextTexture) SDL_DestroyTexture(mTimerTextTexture);
    int minutes = static_cast<int>(mGameTimerSeconds) / 60;
    int seconds = static_cast<int>(mGameTimerSeconds) % 60;
    if (minutes < 0) minutes = 0;
    if (seconds < 0) seconds = 0;

    std::ostringstream timerStream;
    timerStream << "Time: " << std::setw(2) << std::setfill('0') << minutes
        << ":" << std::setw(2) << std::setfill('0') << seconds;
    mTimerTextTexture = createTextTexture(timerStream.str(), mUiTextColor, mUiFont);
}

bool Game::loadAudio() {
    bool success = true;
    mMenuMusic = Mix_LoadMUS("menu_music.mp3");
    if (!mMenuMusic) {
        std::cerr << "Game Warning: Failed to load menu_music.mp3. Mix_Error: " << Mix_GetError() << std::endl;
        success = false;
    }
    mIngameMusic = Mix_LoadMUS("ingame_music.mp3");
    if (!mIngameMusic) {
        std::cerr << "Game Warning: Failed to load ingame_music.mp3. Mix_Error: " << Mix_GetError() << std::endl;
        success = false;
    }
    mBombExplosionSound = Mix_LoadWAV("explosion_sound.wav");
    if (!mBombExplosionSound) {
        std::cerr << "Game Warning: Failed to load explosion_sound.wav. Mix_Error: " << Mix_GetError() << std::endl;
        success = false;
    }
    return success;
}

void Game::playIngameMusic() {
    if (mIngameMusic) {
        stopMusic();
        Mix_PlayMusic(mIngameMusic, -1);
    }
}

void Game::stopMusic() {
    Mix_HaltMusic();
}

void Game::playBombSoundEffect() {
    if (mBombExplosionSound) {
        Mix_PlayChannel(-1, mBombExplosionSound, 0);
    }
}

void Game::startGame() {
    resetGame();
    mGameSettings.updateActualPlayerSpeed();

    mPlayerTexture = loadTexture("player.png");
    mEnemyTexture = loadTexture("enemies.png");
    mBackgroundTexture = loadTexture("background.png");
    mHardWallTexture = loadTexture("hard_wall.png");
    mBorderWallTexture = loadTexture("border_wall.png");
    mBombTexture = loadTexture("bomb.png");
    mExplosionTexture = loadTexture("explosion.png");
    mSoftWallTextures[0] = loadTexture("soft_wall_1.png");
    mSoftWallTextures[1] = loadTexture("soft_wall_2.png");
    mSoftWallTextures[2] = loadTexture("soft_wall_3.png");

    bool texturesLoaded = mPlayerTexture && mEnemyTexture && mBackgroundTexture &&
        mHardWallTexture && mBorderWallTexture && mBombTexture && mExplosionTexture &&
        mSoftWallTextures[0] && mSoftWallTextures[1] && mSoftWallTextures[2];

    if (!texturesLoaded) {
        std::cerr << "Game Error: Failed to load essential game textures! Returning to main menu." << std::endl;
        transitionToMainMenu();
        return;
    }

    mMap = std::make_unique<Map>(mRenderer, mBackgroundTexture, mHardWallTexture, mBorderWallTexture, mSoftWallTextures);
    if (!mMap || !mMap->initialize(mScreenWidth, mScreenHeight)) {
        std::cerr << "Game Error: Failed to initialize map! Returning to main menu." << std::endl;
        transitionToMainMenu();
        return;
    }

    mPlayer = std::make_unique<Player>(mRenderer, mPlayerTexture, 0, 0, mMap.get());
    if (mPlayer && mMap) {
        mPlayer->setPosition(mMap->getTileSize(), mMap->getTileSize());
        mPlayer->setSpeed(mGameSettings.actualPlayerSpeed);
    }
    else {
        std::cerr << "Game Error: Failed to initialize player! Returning to main menu." << std::endl;
        transitionToMainMenu();
        return;
    }

    createEnemiesBasedOnOptions();

    mCurrentScore = 0;
    mGameTimerSeconds = MAX_GAME_TIME_SECONDS;
    mGameOver = false;
    updateScoreDisplay();
    updateTimerDisplay();

    mCurrentState = GameState::PLAYING;
    playIngameMusic();
}

void Game::resetGame() {
    mPlayer.reset();
    mMap.reset();
    mEnemies.clear();
    mBombs.clear();
    mGameOver = false;
}

void Game::createEnemiesBasedOnOptions() {
    if (!mMap || !mEnemyTexture) {
        std::cerr << "Game Warning: Cannot create enemies. Essential components (map or enemy texture) are missing." << std::endl;
        return;
    }
    mEnemies.clear();
    int tileSize = mMap->getTileSize();
    int enemyCountToCreate = mGameSettings.enemyCount;

    for (int i = 0; i < enemyCountToCreate; ++i) {
        auto enemy = std::make_unique<Enemy>(mRenderer, mEnemyTexture, 0, 0);
        enemy->setSize(tileSize, tileSize);

        bool foundPos = false;
        if (mMap) foundPos = enemy->findSafePosition(mMap.get());

        if (foundPos) {
            mEnemies.push_back(std::move(enemy));
        }
        else {
            std::cerr << "Game Warning: Could not find a safe position for enemy " << (i + 1) << ". Enemy not created." << std::endl;
        }
    }
    if (mEnemies.empty() && enemyCountToCreate > 0 && mMap) {
        std::cerr << "Game Info: Fallback - placing at least one enemy." << std::endl;
        auto enemy = std::make_unique<Enemy>(mRenderer, mEnemyTexture, 0, 0);
        enemy->setSize(tileSize, tileSize);
        if (enemy->findSafePosition(mMap.get())) {
            mEnemies.push_back(std::move(enemy));
        }
        else {
            int farX = (mMap->getColumns() - 2) * tileSize;
            int farY = (mMap->getRows() - 2) * tileSize;
            if (farX < tileSize && mMap->getColumns() > 1) farX = tileSize; else if (farX < 0) farX = 0;
            if (farY < tileSize && mMap->getRows() > 1) farY = tileSize; else if (farY < 0) farY = 0;

            if (!isColliding(farX, farY, enemy->getWidth(), enemy->getHeight())) {
                enemy->setPosition(farX, farY);
                mEnemies.push_back(std::move(enemy));
            }
            else {
                std::cerr << "Game Critical Error: Could not place any enemy even in fallback." << std::endl;
            }
        }
    }
}


void Game::handleEvent(SDL_Event& e) {
    if (e.type == SDL_QUIT) {
        return;
    }
    switch (mCurrentState) {
    case GameState::MAIN_MENU:
        handleMainMenuEvents(e);
        break;
    case GameState::OPTIONS_MENU:
        handleOptionsMenuEvents(e);
        break;
    case GameState::PLAYING:
        if (!mGameOver) {
            if (mPlayer) mPlayer->handleEvent(e);
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) {
                placeBomb();
            }
        }
        break;
    case GameState::GAME_OVER_MENU:
        handleGameOverMenuEvents(e);
        break;
    }
    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_o &&
        (mCurrentState == GameState::MAIN_MENU || mCurrentState == GameState::GAME_OVER_MENU)) {
        bool menuHandledOptionKey = false;
        if (mCurrentState == GameState::MAIN_MENU && mMainMenu) {


        }
        if (!menuHandledOptionKey) {
            transitionToOptionsMenu();
        }
    }
}

void Game::handleMainMenuEvents(SDL_Event& e) {
    if (mMainMenu) {
        MenuAction action = mMainMenu->handleEvent(e);
        switch (action) {
        case MenuAction::START_GAME:
            startGame();
            break;
        case MenuAction::OPEN_OPTIONS:
            transitionToOptionsMenu();
            break;
        case MenuAction::EXIT_GAME:
        {
            SDL_Event quitEvent;
            quitEvent.type = SDL_QUIT;
            SDL_PushEvent(&quitEvent);
        }
        break;
        case MenuAction::NONE:
        default:
            break;
        }
    }
}

void Game::handleOptionsMenuEvents(SDL_Event& e) {
    if (mOptionsMenu) {
        OptionsMenuAction action = mOptionsMenu->handleEvent(e);
        if (action == OptionsMenuAction::BACK_TO_MAIN_MENU) {
            mGameSettings.updateActualPlayerSpeed();
            transitionToMainMenu();
        }
    }
    else {
        transitionToMainMenu();
    }
}

void Game::handleGameOverMenuEvents(SDL_Event& e) {
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        if (e.button.button == SDL_BUTTON_LEFT) {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            SDL_Point mousePoint = { mouseX, mouseY };

            if (SDL_PointInRect(&mousePoint, &mContinueButtonRect)) {
                startGame();
            }
            else if (SDL_PointInRect(&mousePoint, &mEndGameButtonRect)) {
                transitionToMainMenu();
            }
        }
    }
    if (e.type == SDL_KEYDOWN) {
        if (e.key.keysym.sym == SDLK_r) {
            startGame();
        }
        else if (e.key.keysym.sym == SDLK_m || e.key.keysym.sym == SDLK_ESCAPE) {
            transitionToMainMenu();
        }
    }
}

void Game::transitionToMainMenu() {
    mCurrentState = GameState::MAIN_MENU;
    stopMusic();
    if (mMainMenu) mMainMenu->playMusic();
}

void Game::transitionToOptionsMenu() {
    mCurrentState = GameState::OPTIONS_MENU;
    stopMusic();
    if (mOptionsMenu) {
        mOptionsMenu->updateOptionDisplays();
    }
}


void Game::update(float deltaTime) {
    if (mCurrentState == GameState::PLAYING && !mGameOver) {
        mGameTimerSeconds -= deltaTime;
        if (mGameTimerSeconds <= 0) {
            mGameTimerSeconds = 0;
            mGameOver = true;
            std::cout << "Time's up! Game Over." << std::endl;
        }
        updateTimerDisplay();

        if (mPlayer) {
            int prevX = mPlayer->getX(); int prevY = mPlayer->getY();
            mPlayer->update(deltaTime);
            if (mMap && isColliding(mPlayer->getX(), mPlayer->getY(), mPlayer->getWidth(), mPlayer->getHeight())) {
                mPlayer->setPosition(prevX, prevY);
            }
        }

        for (auto enemyIt = mEnemies.begin(); enemyIt != mEnemies.end();) {
            if (*enemyIt) {
                (*enemyIt)->update(deltaTime, mMap.get());
                if (mPlayer) {
                    SDL_Rect playerRect = { mPlayer->getX(), mPlayer->getY(), mPlayer->getWidth(), mPlayer->getHeight() };
                    SDL_Rect enemyRect = { (*enemyIt)->getX(), (*enemyIt)->getY(), (*enemyIt)->getWidth(), (*enemyIt)->getHeight() };
                    if (checkCollision(playerRect, enemyRect)) {
                        mGameOver = true;
                        std::cout << "Game Over! Collided with an enemy." << std::endl;
                        break;
                    }
                }
                ++enemyIt;
            }
            else { enemyIt = mEnemies.erase(enemyIt); }
        }
        if (mGameOver) { transitionToGameOver(); return; }

        for (auto it = mBombs.begin(); it != mBombs.end();) {
            if (*it) {
                bool wasBombExploding = (*it)->isExploding();
                (*it)->update(deltaTime);

                if ((*it)->isExploding() && !wasBombExploding && !(*it)->hasExplosionSoundPlayed()) {
                    playBombSoundEffect();
                    (*it)->setExplosionSoundPlayed(true);
                }

                if ((*it)->isExploding()) {
                    Explosion& explosion = (*it)->getExplosion();
                    if (mMap) {
                        int softWallsDestroyed = mMap->handleExplosion(explosion);
                        if (softWallsDestroyed > 0) {
                            mCurrentScore += softWallsDestroyed * 50;
                            updateScoreDisplay();
                        }
                    }

                    if (mPlayer && !mGameOver) {
                        SDL_Rect playerRect = { mPlayer->getX(), mPlayer->getY(), mPlayer->getWidth(), mPlayer->getHeight() };
                        for (const auto& part : explosion.parts) {
                            SDL_Rect explosionRect = { part.x, part.y, mMap ? mMap->getTileSize() : 32, mMap ? mMap->getTileSize() : 32 };
                            if (checkCollision(playerRect, explosionRect)) {
                                mGameOver = true;
                                std::cout << "Game Over! Caught in an explosion." << std::endl;
                                break;
                            }
                        }
                    }
                    if (mGameOver) break;

                    for (auto enemyIt = mEnemies.begin(); enemyIt != mEnemies.end();) {
                        if (*enemyIt) {
                            bool enemyHit = false;
                            SDL_Rect enemyRect = { (*enemyIt)->getX(), (*enemyIt)->getY(), (*enemyIt)->getWidth(), (*enemyIt)->getHeight() };
                            for (const auto& part : explosion.parts) {
                                SDL_Rect explosionRect = { part.x, part.y, mMap ? mMap->getTileSize() : 32, mMap ? mMap->getTileSize() : 32 };
                                if (checkCollision(enemyRect, explosionRect)) {
                                    enemyHit = true; break;
                                }
                            }
                            if (enemyHit) {
                                enemyIt = mEnemies.erase(enemyIt);
                                mCurrentScore += 500;
                                updateScoreDisplay();
                            }
                            else { ++enemyIt; }
                        }
                        else { enemyIt = mEnemies.erase(enemyIt); }
                    }
                }
                if ((*it)->isDone()) {
                    it = mBombs.erase(it);
                }
                else { ++it; }
            }
            else { it = mBombs.erase(it); }
        }
        if (mGameOver) { transitionToGameOver(); return; }

        if (mEnemies.empty() && mGameTimerSeconds > 0) {
            std::cout << "You win! All enemies defeated." << std::endl;
            mGameOver = true;
        }
        if (mGameOver) {
            transitionToGameOver();
        }
    }
    else if (mCurrentState == GameState::OPTIONS_MENU) {

    }
}

void Game::transitionToGameOver() {
    stopMusic();
    calculateFinalScore();
    saveHighScore();

    TTF_Font* titleFont = TTF_OpenFont("game_font.otf", 60);
    if (!titleFont) titleFont = mGameFont;

    if (mGameOverStateTitleTexture) SDL_DestroyTexture(mGameOverStateTitleTexture);
    if (mEnemies.empty() && mGameTimerSeconds > 0) {
        mGameOverStateTitleTexture = createTextTexture("YOU WIN!", { 50, 205, 50, 255 }, titleFont);
    }
    else {
        mGameOverStateTitleTexture = createTextTexture("GAME OVER", { 255, 69, 0, 255 }, titleFont);
    }

    if (mFinalScoreTextTexture) SDL_DestroyTexture(mFinalScoreTextTexture);
    std::ostringstream finalScoreStream;
    finalScoreStream << "Final Score: " << std::setw(4) << std::setfill('0') << mCurrentScore;
    mFinalScoreTextTexture = createTextTexture(finalScoreStream.str(), mUiTextColor, mUiFont);

    if (mHighScoreTextTexture) SDL_DestroyTexture(mHighScoreTextTexture);
    std::ostringstream highScoreStream;
    highScoreStream << "High Score: " << std::setw(4) << std::setfill('0') << mHighScore;
    mHighScoreTextTexture = createTextTexture(highScoreStream.str(), mUiTextColor, mUiFont);

    if (titleFont && titleFont != mGameFont) TTF_CloseFont(titleFont);

    mCurrentState = GameState::GAME_OVER_MENU;
}

void Game::calculateFinalScore() {
    if (mEnemies.empty() && mGameTimerSeconds > 0) {
        int timeBonus = static_cast<int>(mGameTimerSeconds) * 20;
        mCurrentScore += timeBonus;
        std::cout << "Time Bonus: +" << timeBonus << " points." << std::endl;
    }
}


void Game::render() {
    switch (mCurrentState) {
    case GameState::MAIN_MENU:
        if (mMainMenu) mMainMenu->render();
        break;
    case GameState::OPTIONS_MENU:
        if (mOptionsMenu) mOptionsMenu->render();
        else {
            SDL_SetRenderDrawColor(mRenderer, 30, 30, 30, 255); SDL_RenderClear(mRenderer);
            SDL_Texture* errText = createTextTexture("Options Not Available", mUiTextColor, mGameFont);
            if (errText) {
                int w, h; SDL_QueryTexture(errText, 0, 0, &w, &h);
                SDL_Rect r = { (mScreenWidth - w) / 2, (mScreenHeight - h) / 2,w,h };
                SDL_RenderCopy(mRenderer, errText, 0, &r);
                SDL_DestroyTexture(errText);
            }
        }
        break;
    case GameState::PLAYING:
        renderPlayingState();
        break;
    case GameState::GAME_OVER_MENU:
        renderGameOverMenu();
        break;
    }
}

void Game::renderPlayingState() {
    if (mMap) mMap->render();
    for (const auto& bomb : mBombs) {
        if (bomb) bomb->render(mRenderer, mBombTexture, mExplosionTexture);
    }
    for (const auto& enemy : mEnemies) {
        if (enemy) enemy->render();
    }
    if (mPlayer) mPlayer->render();
    renderScoreAndTimer();
}

void Game::renderScoreAndTimer() {
    if (mScoreTextTexture) {
        int w, h;
        SDL_QueryTexture(mScoreTextTexture, NULL, NULL, &w, &h);
        SDL_Rect destRect = { 20, 10, w, h };
        SDL_RenderCopy(mRenderer, mScoreTextTexture, NULL, &destRect);
    }
    if (mTimerTextTexture) {
        int w, h;
        SDL_QueryTexture(mTimerTextTexture, NULL, NULL, &w, &h);
        SDL_Rect destRect = { mScreenWidth - w - 20, 10, w, h };
        SDL_RenderCopy(mRenderer, mTimerTextTexture, NULL, &destRect);
    }
}

void Game::renderGameOverMenu() {
    renderPlayingState();

    SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 180);
    SDL_Rect overlayRect = { 0, 0, mScreenWidth, mScreenHeight };
    SDL_RenderFillRect(mRenderer, &overlayRect);
    SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_NONE);

    if (mGameOverStateTitleTexture) {
        int w, h;
        SDL_QueryTexture(mGameOverStateTitleTexture, NULL, NULL, &w, &h);
        SDL_Rect titleRect = { (mScreenWidth - w) / 2, mScreenHeight / 4 - h / 2, w, h };
        SDL_RenderCopy(mRenderer, mGameOverStateTitleTexture, NULL, &titleRect);
    }

    if (mFinalScoreTextTexture) {
        int w, h;
        SDL_QueryTexture(mFinalScoreTextTexture, NULL, NULL, &w, &h);
        SDL_Rect scoreRect = { (mScreenWidth - w) / 2, mScreenHeight / 2 - h - 40, w, h };
        SDL_RenderCopy(mRenderer, mFinalScoreTextTexture, NULL, &scoreRect);
    }

    if (mHighScoreTextTexture) {
        int w, h;
        SDL_QueryTexture(mHighScoreTextTexture, NULL, NULL, &w, &h);
        SDL_Rect highScoreRect = { (mScreenWidth - w) / 2, mScreenHeight / 2 - 0, w, h };
        SDL_RenderCopy(mRenderer, mHighScoreTextTexture, NULL, &highScoreRect);
    }

    if (mContinueButtonTexture) {
        SDL_RenderCopy(mRenderer, mContinueButtonTexture, NULL, &mContinueButtonRect);
    }

    if (mEndGameButtonTexture) {
        SDL_RenderCopy(mRenderer, mEndGameButtonTexture, NULL, &mEndGameButtonRect);
    }
}

SDL_Texture* Game::loadTexture(const std::string& path) {
    SDL_Texture* texture = IMG_LoadTexture(mRenderer, path.c_str());
    if (texture == nullptr) {
        std::cerr << "Game Error: Failed to load texture '" << path << "'. SDL_image Error: " << IMG_GetError() << std::endl;
    }
    return texture;
}

void Game::placeBomb() {
    if (!mPlayer || !mMap) return;

    long activeBombsCount = 0;
    for (const auto& currentBomb : mBombs) {
        if (currentBomb && !currentBomb->isExploding() && !currentBomb->isDone()) {
            activeBombsCount++;
        }
    }

    if (activeBombsCount >= mGameSettings.playerMaxActiveBombs) {
        return;
    }

    int tileSize = mMap->getTileSize();
    int bombPlacementX = (mPlayer->getX() + mPlayer->getWidth() / 2) / tileSize * tileSize;
    int bombPlacementY = (mPlayer->getY() + mPlayer->getHeight() / 2) / tileSize * tileSize;

    for (const auto& existingBomb : mBombs) {
        if (existingBomb && !existingBomb->isDone() &&
            existingBomb->getX() == bombPlacementX && existingBomb->getY() == bombPlacementY) {
            return;
        }
    }

    auto newBomb = std::make_unique<Bomb>(bombPlacementX, bombPlacementY, tileSize, 2.0f, mGameSettings.playerBombRange);
    newBomb->setMap(mMap.get());
    newBomb->setExplosionSoundPlayed(false);
    mBombs.push_back(std::move(newBomb));
}

bool Game::checkCollision(SDL_Rect a, SDL_Rect b) {
    int leftA = a.x, rightA = a.x + a.w, topA = a.y, bottomA = a.y + a.h;
    int leftB = b.x, rightB = b.x + b.w, topB = b.y, bottomB = b.y + b.h;
    if (bottomA <= topB || topA >= bottomB || rightA <= leftB || leftA >= rightB) return false;
    return true;
}

bool Game::isColliding(int x, int y, int width, int height) {
    if (!mMap) return true;
    return mMap->isColliding(x, y, width, height);
}
