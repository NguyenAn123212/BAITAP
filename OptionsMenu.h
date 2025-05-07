#ifndef OPTIONSMENU_H
#define OPTIONSMENU_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>
#include "GameOptions.h" 


enum class OptionsMenuAction {
    NONE,
    BACK_TO_MAIN_MENU,
    INCREASE_PLAYER_SPEED,
    DECREASE_PLAYER_SPEED,
    INCREASE_ENEMY_COUNT,
    DECREASE_ENEMY_COUNT,
    INCREASE_MAX_BOMBS,
    DECREASE_MAX_BOMBS,
    INCREASE_BOMB_RANGE,
    DECREASE_BOMB_RANGE
};

class OptionsMenu {
public:
    OptionsMenu(SDL_Renderer* renderer, TTF_Font* font, int screenWidth, int screenHeight, GameOptions& gameSettings);
    ~OptionsMenu();

    bool initialize();

    OptionsMenuAction handleEvent(SDL_Event& e);

    void render();

    void updateOptionDisplays();

private:
    SDL_Renderer* mRenderer;
    TTF_Font* mFont; 
    int mScreenWidth;
    int mScreenHeight;
    GameOptions& mGameSettings; 
    SDL_Color mTextColor;       
    SDL_Color mButtonTextColor;

    SDL_Texture* mTitleTexture;
    SDL_Rect mTitleRect;

    struct OptionUI {
        std::string label;
        SDL_Texture* labelTexture = nullptr;
        SDL_Rect labelRect;

        SDL_Texture* valueTexture = nullptr; 
        SDL_Rect valueRect;

        SDL_Texture* decreaseButtonTexture = nullptr; // Nút "-"
        SDL_Rect decreaseButtonRect;
        SDL_Texture* increaseButtonTexture = nullptr; // Nút "+"
        SDL_Rect increaseButtonRect;

        OptionsMenuAction decreaseAction;
        OptionsMenuAction increaseAction;
        int* optionValuePtr_int = nullptr; // Con trỏ tới giá trị int trong GameOptions

        int minValue;
        int maxValue;
    };

    std::vector<OptionUI> mOptionItems; // Danh sách các mục tùy chọn

    SDL_Texture* mBackButtonTexture;
    SDL_Rect mBackButtonRect;

    SDL_Texture* createTextTexture(const std::string& text, SDL_Color color);
    void setupOptionItemUI(OptionUI& item, const std::string& labelText, int* valuePtr, int minVal, int maxVal,
        OptionsMenuAction decAction, OptionsMenuAction incAction, int yPos);
    void destroyOptionItemTextures(OptionUI& item);
};

#endif // OPTIONSMENU_H
