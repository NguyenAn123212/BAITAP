#include "menu.h"
#include <SDL_image.h> 
#include <iostream>    

Menu::Menu(SDL_Renderer* renderer, TTF_Font* font, int screenWidth, int screenHeight, Mix_Music* menuMusic)
    : mRenderer(renderer),
    mFont(font),
    mScreenWidth(screenWidth),
    mScreenHeight(screenHeight),
    mMenuBackgroundTexture(nullptr),
    mStartButtonTexture(nullptr),
    mOptionsButtonTexture(nullptr), 
    mExitButtonTexture(nullptr),
    mButtonTextColor({ 255, 255, 255, 255 }), 
    mMenuMusic(menuMusic)
{
    if (!mRenderer) {
        std::cerr << "Menu Error: Renderer is null in Menu constructor!" << std::endl;
    }
    if (!mFont) {
        std::cerr << "Menu Error: Font is null in Menu constructor!" << std::endl;
    }
}

Menu::~Menu() {
    if (mMenuBackgroundTexture) SDL_DestroyTexture(mMenuBackgroundTexture);
    if (mStartButtonTexture) SDL_DestroyTexture(mStartButtonTexture);
    if (mOptionsButtonTexture) SDL_DestroyTexture(mOptionsButtonTexture); 
    if (mExitButtonTexture) SDL_DestroyTexture(mExitButtonTexture);

}

bool Menu::initialize(const std::string& backgroundPath) {
    if (!mRenderer || !mFont) {
        std::cerr << "Menu Error: Cannot initialize Menu without a valid renderer and font." << std::endl;
        return false;
    }

    mMenuBackgroundTexture = loadTexture(backgroundPath);
    if (mMenuBackgroundTexture == nullptr) {
        std::cerr << "Menu Warning: Failed to load menu background texture from path: " << backgroundPath << std::endl;
    }

    mStartButtonTexture = createTextTexture("Start Game", mButtonTextColor);
    mOptionsButtonTexture = createTextTexture("Options", mButtonTextColor);
    mExitButtonTexture = createTextTexture("Exit Game", mButtonTextColor);

    if (!mStartButtonTexture || !mOptionsButtonTexture || !mExitButtonTexture) {
        std::cerr << "Menu Error: Failed to create text textures for one or more menu buttons!" << std::endl;
        return false; 
    }

    int buttonWidth, buttonHeight;
    int commonButtonHeight = 0; 

    if (TTF_SizeText(mFont, "Start Game", &buttonWidth, &buttonHeight) == 0) {
        if (commonButtonHeight == 0) commonButtonHeight = buttonHeight;
        mStartButtonRect = { (mScreenWidth - buttonWidth) / 2, mScreenHeight / 2 - commonButtonHeight - 30, buttonWidth, buttonHeight }; // 30 là khoảng cách
    }
    else {
        std::cerr << "Menu Warning: Failed to get size of 'Start Game' text: " << TTF_GetError() << std::endl;
        if (commonButtonHeight == 0) commonButtonHeight = 50;
        mStartButtonRect = { (mScreenWidth - 200) / 2, mScreenHeight / 2 - commonButtonHeight - 30, 200, 50 };
    }

    if (TTF_SizeText(mFont, "Options", &buttonWidth, &buttonHeight) == 0) {
        if (commonButtonHeight == 0) commonButtonHeight = buttonHeight; 
        mOptionsButtonRect = { (mScreenWidth - buttonWidth) / 2, mScreenHeight / 2 , buttonWidth, buttonHeight }; 
    }
    else {
        std::cerr << "Menu Warning: Failed to get size of 'Options' text: " << TTF_GetError() << std::endl;
        if (commonButtonHeight == 0) commonButtonHeight = 50;
        mOptionsButtonRect = { (mScreenWidth - 150) / 2, mScreenHeight / 2 , 150, 50 };
    }

    // Nút "Exit Game"
    if (TTF_SizeText(mFont, "Exit Game", &buttonWidth, &buttonHeight) == 0) {
        if (commonButtonHeight == 0) commonButtonHeight = buttonHeight;
        mExitButtonRect = { (mScreenWidth - buttonWidth) / 2, mScreenHeight / 2 + commonButtonHeight + 30, buttonWidth, buttonHeight };
    }
    else {
        std::cerr << "Menu Warning: Failed to get size of 'Exit Game' text: " << TTF_GetError() << std::endl;
        if (commonButtonHeight == 0) commonButtonHeight = 50;
        mExitButtonRect = { (mScreenWidth - 200) / 2, mScreenHeight / 2 + commonButtonHeight + 30, 200, 50 };
    }

    return true; 
}

MenuAction Menu::handleEvent(SDL_Event& e) {
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        if (e.button.button == SDL_BUTTON_LEFT) { 
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            SDL_Point mousePoint = { mouseX, mouseY };

            if (SDL_PointInRect(&mousePoint, &mStartButtonRect)) {
                return MenuAction::START_GAME;
            }
            else if (SDL_PointInRect(&mousePoint, &mOptionsButtonRect)) { 
                return MenuAction::OPEN_OPTIONS;
            }
            else if (SDL_PointInRect(&mousePoint, &mExitButtonRect)) {
                return MenuAction::EXIT_GAME;
            }
        }
    }
}

void Menu::render() {
    if (!mRenderer) {
        std::cerr << "Menu Error: Cannot render menu, renderer is null." << std::endl;
        return;
    }

    if (mMenuBackgroundTexture) {
        SDL_RenderCopy(mRenderer, mMenuBackgroundTexture, NULL, NULL); 
    }
    else {
        SDL_SetRenderDrawColor(mRenderer, 20, 20, 50, 255);
        SDL_RenderClear(mRenderer);
    }

    if (mStartButtonTexture) {
        SDL_RenderCopy(mRenderer, mStartButtonTexture, NULL, &mStartButtonRect);
    }

    if (mOptionsButtonTexture) {
        SDL_RenderCopy(mRenderer, mOptionsButtonTexture, NULL, &mOptionsButtonRect);
    }

    if (mExitButtonTexture) {
        SDL_RenderCopy(mRenderer, mExitButtonTexture, NULL, &mExitButtonRect);
    }
}

void Menu::playMusic() {
    if (mMenuMusic) {
        if (Mix_PlayingMusic() == 0) {
            Mix_PlayMusic(mMenuMusic, -1); 
        }
        else if (Mix_PausedMusic() == 1) { 
            Mix_ResumeMusic();
        }
       
    }
    else {
        
    }
}

SDL_Texture* Menu::createTextTexture(const std::string& text, SDL_Color color) {
    if (!mFont || !mRenderer) {
        std::cerr << "Menu Error: Cannot create text texture, font or renderer is null. Text: " << text << std::endl;
        return nullptr;
    }
    SDL_Surface* textSurface = TTF_RenderText_Solid(mFont, text.c_str(), color);
    if (!textSurface) {
        std::cerr << "Menu Error: TTF_RenderText_Solid failed for \"" << text << "\". TTF_Error: " << TTF_GetError() << std::endl;
        return nullptr;
    }
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(mRenderer, textSurface);
    SDL_FreeSurface(textSurface);
    if (!textTexture) {
        std::cerr << "Menu Error: SDL_CreateTextureFromSurface failed for \"" << text << "\". SDL_Error: " << SDL_GetError() << std::endl;
    }
    return textTexture;
}

SDL_Texture* Menu::loadTexture(const std::string& path) {
    if (!mRenderer) {
        std::cerr << "Menu Error: Cannot load texture, renderer is null." << std::endl;
        return nullptr;
    }
    SDL_Texture* newTexture = IMG_LoadTexture(mRenderer, path.c_str());
    if (newTexture == nullptr) {
        std::cerr << "Menu Error: Failed to load texture from path: \"" << path << "\". SDL_image Error: " << IMG_GetError() << std::endl;
    }
    return newTexture;
}
