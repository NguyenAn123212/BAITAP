#include "OptionsMenu.h"
#include <SDL_image.h> 
#include <iostream>   
#include <string>     

OptionsMenu::OptionsMenu(SDL_Renderer* renderer, TTF_Font* font, int screenWidth, int screenHeight, GameOptions& gameSettings)
    : mRenderer(renderer),
    mFont(font),
    mScreenWidth(screenWidth),
    mScreenHeight(screenHeight),
    mGameSettings(gameSettings), 
    mTextColor({ 220, 220, 220, 255 }), 
    mButtonTextColor({ 255, 255, 255, 255 }), 
    mTitleTexture(nullptr),
    mBackButtonTexture(nullptr)
{
    if (!mRenderer || !mFont) {
        std::cerr << "OptionsMenu Error: Renderer or Font is null in constructor!" << std::endl;
    }
}

OptionsMenu::~OptionsMenu() {
    if (mTitleTexture) SDL_DestroyTexture(mTitleTexture);
    if (mBackButtonTexture) SDL_DestroyTexture(mBackButtonTexture);

    for (auto& item : mOptionItems) {
        destroyOptionItemTextures(item);
    }
}

SDL_Texture* OptionsMenu::createTextTexture(const std::string& text, SDL_Color color) {
    if (!mFont || !mRenderer) return nullptr;
    SDL_Surface* surface = TTF_RenderText_Solid(mFont, text.c_str(), color);
    if (!surface) {
        std::cerr << "OptionsMenu Error: TTF_RenderText_Solid failed: " << TTF_GetError() << std::endl;
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(mRenderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        std::cerr << "OptionsMenu Error: SDL_CreateTextureFromSurface failed: " << SDL_GetError() << std::endl;
    }
    return texture;
}

void OptionsMenu::destroyOptionItemTextures(OptionUI& item) {
    if (item.labelTexture) SDL_DestroyTexture(item.labelTexture);
    if (item.valueTexture) SDL_DestroyTexture(item.valueTexture);
    if (item.decreaseButtonTexture) SDL_DestroyTexture(item.decreaseButtonTexture);
    if (item.increaseButtonTexture) SDL_DestroyTexture(item.increaseButtonTexture);
    item.labelTexture = nullptr;
    item.valueTexture = nullptr;
    item.decreaseButtonTexture = nullptr;
    item.increaseButtonTexture = nullptr;
}


void OptionsMenu::setupOptionItemUI(OptionUI& item, const std::string& labelText, int* valuePtr, int minVal, int maxVal,
    OptionsMenuAction decAction, OptionsMenuAction incAction, int yPos) {
    item.label = labelText;
    item.optionValuePtr_int = valuePtr;
    item.minValue = minVal;
    item.maxValue = maxVal;
    item.decreaseAction = decAction;
    item.increaseAction = incAction;

   
    destroyOptionItemTextures(item); 
    item.labelTexture = createTextTexture(labelText + ":", mTextColor);

    item.decreaseButtonTexture = createTextTexture("-", mButtonTextColor);
    item.increaseButtonTexture = createTextTexture("+", mButtonTextColor);

    int labelW = 0, labelH = 0;
    int valW = 0, valH = 0;
    int btnW = 0, btnH = 0;

    if (item.labelTexture) TTF_SizeText(mFont, (labelText + ":").c_str(), &labelW, &labelH);
    if (item.decreaseButtonTexture) TTF_SizeText(mFont, "-", &btnW, &btnH);

    int padding = 10;
    int buttonSize = btnH + padding / 2;

    item.labelRect = { 50, yPos, labelW, labelH };
    item.decreaseButtonRect = { mScreenWidth / 2 + 50, yPos, buttonSize, buttonSize };
    item.valueRect = { item.decreaseButtonRect.x + buttonSize + padding, yPos, 50, labelH }; 
    item.increaseButtonRect = { item.valueRect.x + item.valueRect.w + padding, yPos, buttonSize, buttonSize };
}


bool OptionsMenu::initialize() {
    if (!mRenderer || !mFont) return false;

    mTitleTexture = createTextTexture("Game Options", mButtonTextColor); 
    if (mTitleTexture) {
        int w, h;
        SDL_QueryTexture(mTitleTexture, nullptr, nullptr, &w, &h);
        mTitleRect = { (mScreenWidth - w) / 2, 50, w, h };
    }

    mOptionItems.resize(4); // 4 tùy chọn
    int startY = mTitleRect.y + mTitleRect.h + 50;
    int spacingY = 60;

    setupOptionItemUI(mOptionItems[0], "Player Speed", &mGameSettings.playerSpeedLevel, 1, 9,
        OptionsMenuAction::DECREASE_PLAYER_SPEED, OptionsMenuAction::INCREASE_PLAYER_SPEED, startY);
    setupOptionItemUI(mOptionItems[1], "Enemy Count", &mGameSettings.enemyCount, 1, 9,
        OptionsMenuAction::DECREASE_ENEMY_COUNT, OptionsMenuAction::INCREASE_ENEMY_COUNT, startY + spacingY);
    setupOptionItemUI(mOptionItems[2], "Max Bombs", &mGameSettings.playerMaxActiveBombs, 1, 5,
        OptionsMenuAction::DECREASE_MAX_BOMBS, OptionsMenuAction::INCREASE_MAX_BOMBS, startY + 2 * spacingY);
    setupOptionItemUI(mOptionItems[3], "Bomb Range", &mGameSettings.playerBombRange, 1, 5,
        OptionsMenuAction::DECREASE_BOMB_RANGE, OptionsMenuAction::INCREASE_BOMB_RANGE, startY + 3 * spacingY);

    mBackButtonTexture = createTextTexture("Back to Main Menu", mButtonTextColor);
    if (mBackButtonTexture) {
        int w, h;
        SDL_QueryTexture(mBackButtonTexture, nullptr, nullptr, &w, &h);
        mBackButtonRect = { (mScreenWidth - w) / 2, mScreenHeight - h - 50, w, h };
    }

    updateOptionDisplays(); 
    return true;
}

void OptionsMenu::updateOptionDisplays() {
    for (auto& item : mOptionItems) {
        if (item.valueTexture) {
            SDL_DestroyTexture(item.valueTexture);
            item.valueTexture = nullptr;
        }
        if (item.optionValuePtr_int) {
            item.valueTexture = createTextTexture(std::to_string(*item.optionValuePtr_int), mTextColor);
            if (item.valueTexture) { 
                int valW, valH;
                SDL_QueryTexture(item.valueTexture, nullptr, nullptr, &valW, &valH);
                item.valueRect.w = valW; 
                         }
        }
    }
}

OptionsMenuAction OptionsMenu::handleEvent(SDL_Event& e) {
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        if (e.button.button == SDL_BUTTON_LEFT) {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            SDL_Point mousePoint = { mouseX, mouseY };

            if (SDL_PointInRect(&mousePoint, &mBackButtonRect)) {
                return OptionsMenuAction::BACK_TO_MAIN_MENU;
            }

            
            for (auto& item : mOptionItems) {
                if (SDL_PointInRect(&mousePoint, &item.decreaseButtonRect)) {
                    if (item.optionValuePtr_int && *item.optionValuePtr_int > item.minValue) {
                        (*item.optionValuePtr_int)--;
                        if (item.label == "Player Speed") mGameSettings.updateActualPlayerSpeed(); 
                        updateOptionDisplays();
                        return item.decreaseAction; 
                    }
                }
                else if (SDL_PointInRect(&mousePoint, &item.increaseButtonRect)) {
                    if (item.optionValuePtr_int && *item.optionValuePtr_int < item.maxValue) {
                        (*item.optionValuePtr_int)++;
                        if (item.label == "Player Speed") mGameSettings.updateActualPlayerSpeed();
                        updateOptionDisplays();
                        return item.increaseAction;
                    }
                }
            }
        }
    }
    return OptionsMenuAction::NONE;
}

void OptionsMenu::render() {
    if (!mRenderer) return;

    SDL_SetRenderDrawColor(mRenderer, 40, 40, 60, 255);
    SDL_RenderClear(mRenderer);

    if (mTitleTexture) {
        SDL_RenderCopy(mRenderer, mTitleTexture, nullptr, &mTitleRect);
    }

    for (const auto& item : mOptionItems) {
        if (item.labelTexture) SDL_RenderCopy(mRenderer, item.labelTexture, nullptr, &item.labelRect);
        if (item.valueTexture) SDL_RenderCopy(mRenderer, item.valueTexture, nullptr, &item.valueRect);
        if (item.decreaseButtonTexture) SDL_RenderCopy(mRenderer, item.decreaseButtonTexture, nullptr, &item.decreaseButtonRect);
        if (item.increaseButtonTexture) SDL_RenderCopy(mRenderer, item.increaseButtonTexture, nullptr, &item.increaseButtonRect);
    }

    if (mBackButtonTexture) {
        SDL_RenderCopy(mRenderer, mBackButtonTexture, nullptr, &mBackButtonRect);
    }
}
