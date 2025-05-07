#include "OptionsMenu.h"
#include <SDL_image.h> // Mặc dù không load texture nền ở đây, nhưng có thể cần
#include <iostream>    // Cho std::cerr
#include <string>      // Cho std::to_string

OptionsMenu::OptionsMenu(SDL_Renderer* renderer, TTF_Font* font, int screenWidth, int screenHeight, GameOptions& gameSettings)
    : mRenderer(renderer),
    mFont(font),
    mScreenWidth(screenWidth),
    mScreenHeight(screenHeight),
    mGameSettings(gameSettings), // Lưu tham chiếu
    mTextColor({ 220, 220, 220, 255 }), // Màu xám nhạt cho nhãn
    mButtonTextColor({ 255, 255, 255, 255 }), // Màu trắng cho nút
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

    // Tạo texture cho nhãn
    destroyOptionItemTextures(item); // Xóa texture cũ nếu có
    item.labelTexture = createTextTexture(labelText + ":", mTextColor);

    // Tạo texture cho nút giảm và tăng
    item.decreaseButtonTexture = createTextTexture("-", mButtonTextColor);
    item.increaseButtonTexture = createTextTexture("+", mButtonTextColor);

    // Tính toán Rects (ví dụ đơn giản, cần tinh chỉnh cho đẹp)
    int labelW = 0, labelH = 0;
    int valW = 0, valH = 0;
    int btnW = 0, btnH = 0;

    if (item.labelTexture) TTF_SizeText(mFont, (labelText + ":").c_str(), &labelW, &labelH);
    // Giá trị sẽ được cập nhật trong updateOptionDisplays
    if (item.decreaseButtonTexture) TTF_SizeText(mFont, "-", &btnW, &btnH);

    int padding = 10;
    int buttonSize = btnH + padding / 2; // Làm nút vuông vắn hơn một chút

    item.labelRect = { 50, yPos, labelW, labelH };
    item.decreaseButtonRect = { mScreenWidth / 2 + 50, yPos, buttonSize, buttonSize };
    item.valueRect = { item.decreaseButtonRect.x + buttonSize + padding, yPos, 50, labelH }; // Ước lượng chiều rộng cho giá trị
    item.increaseButtonRect = { item.valueRect.x + item.valueRect.w + padding, yPos, buttonSize, buttonSize };
}


bool OptionsMenu::initialize() {
    if (!mRenderer || !mFont) return false;

    // Tạo tiêu đề
    mTitleTexture = createTextTexture("Game Options", mButtonTextColor); // Dùng màu nút cho tiêu đề
    if (mTitleTexture) {
        int w, h;
        SDL_QueryTexture(mTitleTexture, nullptr, nullptr, &w, &h);
        mTitleRect = { (mScreenWidth - w) / 2, 50, w, h };
    }

    // Khởi tạo các mục tùy chọn
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

    // Tạo nút "Back"
    mBackButtonTexture = createTextTexture("Back to Main Menu", mButtonTextColor);
    if (mBackButtonTexture) {
        int w, h;
        SDL_QueryTexture(mBackButtonTexture, nullptr, nullptr, &w, &h);
        mBackButtonRect = { (mScreenWidth - w) / 2, mScreenHeight - h - 50, w, h };
    }

    updateOptionDisplays(); // Cập nhật hiển thị giá trị ban đầu
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
            if (item.valueTexture) { // Cập nhật lại valueRect nếu cần
                int valW, valH;
                SDL_QueryTexture(item.valueTexture, nullptr, nullptr, &valW, &valH);
                item.valueRect.w = valW; // Chỉ cập nhật chiều rộng, vị trí x,y,h giữ nguyên từ setup
                // Căn giữa giá trị nếu muốn:
                // item.valueRect.x = item.decreaseButtonRect.x + item.decreaseButtonRect.w + padding + (50 - valW)/2 ; // 50 là ước lượng ban đầu
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

            // Kiểm tra nút "Back"
            if (SDL_PointInRect(&mousePoint, &mBackButtonRect)) {
                return OptionsMenuAction::BACK_TO_MAIN_MENU;
            }

            // Kiểm tra các nút +/- cho từng tùy chọn
            for (auto& item : mOptionItems) {
                if (SDL_PointInRect(&mousePoint, &item.decreaseButtonRect)) {
                    if (item.optionValuePtr_int && *item.optionValuePtr_int > item.minValue) {
                        (*item.optionValuePtr_int)--;
                        if (item.label == "Player Speed") mGameSettings.updateActualPlayerSpeed(); // Cập nhật tốc độ thực tế
                        updateOptionDisplays();
                        return item.decreaseAction; // Hoặc có thể chỉ return NONE và để Game áp dụng khi thoát
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

    // Vẽ nền (ví dụ màu xám đậm)
    SDL_SetRenderDrawColor(mRenderer, 40, 40, 60, 255);
    SDL_RenderClear(mRenderer);

    // Vẽ tiêu đề
    if (mTitleTexture) {
        SDL_RenderCopy(mRenderer, mTitleTexture, nullptr, &mTitleRect);
    }

    // Vẽ các mục tùy chọn
    for (const auto& item : mOptionItems) {
        if (item.labelTexture) SDL_RenderCopy(mRenderer, item.labelTexture, nullptr, &item.labelRect);
        if (item.valueTexture) SDL_RenderCopy(mRenderer, item.valueTexture, nullptr, &item.valueRect);
        if (item.decreaseButtonTexture) SDL_RenderCopy(mRenderer, item.decreaseButtonTexture, nullptr, &item.decreaseButtonRect);
        if (item.increaseButtonTexture) SDL_RenderCopy(mRenderer, item.increaseButtonTexture, nullptr, &item.increaseButtonRect);
    }

    // Vẽ nút "Back"
    if (mBackButtonTexture) {
        SDL_RenderCopy(mRenderer, mBackButtonTexture, nullptr, &mBackButtonRect);
    }
}
