#ifndef OPTIONSMENU_H
#define OPTIONSMENU_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>
#include "GameOptions.h" // Để truy cập và sửa đổi GameOptions

// Enum xác định hành động từ OptionsMenu
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

// Lớp để quản lý giao diện và logic của màn hình tùy chọn
class OptionsMenu {
public:
    OptionsMenu(SDL_Renderer* renderer, TTF_Font* font, int screenWidth, int screenHeight, GameOptions& gameSettings);
    ~OptionsMenu();

    // Khởi tạo các thành phần UI của menu tùy chọn
    bool initialize();

    // Xử lý sự kiện đầu vào (ví dụ: click chuột)
    OptionsMenuAction handleEvent(SDL_Event& e);

    // Vẽ menu tùy chọn lên màn hình
    void render();

    // Cập nhật hiển thị các giá trị tùy chọn (khi chúng thay đổi)
    void updateOptionDisplays();

private:
    SDL_Renderer* mRenderer;
    TTF_Font* mFont; // Font được truyền từ lớp Game (ví dụ: mUiFont)
    int mScreenWidth;
    int mScreenHeight;
    GameOptions& mGameSettings; // Tham chiếu đến đối tượng GameOptions của Game

    SDL_Color mTextColor;       // Màu chữ cho nhãn và giá trị
    SDL_Color mButtonTextColor; // Màu chữ cho nút +/-/Back

    // Tiêu đề
    SDL_Texture* mTitleTexture;
    SDL_Rect mTitleRect;

    // Các thành phần UI cho mỗi tùy chọn
    struct OptionUI {
        std::string label;
        SDL_Texture* labelTexture = nullptr;
        SDL_Rect labelRect;

        SDL_Texture* valueTexture = nullptr; // Hiển thị giá trị hiện tại
        SDL_Rect valueRect;

        SDL_Texture* decreaseButtonTexture = nullptr; // Nút "-"
        SDL_Rect decreaseButtonRect;
        SDL_Texture* increaseButtonTexture = nullptr; // Nút "+"
        SDL_Rect increaseButtonRect;

        OptionsMenuAction decreaseAction;
        OptionsMenuAction increaseAction;
        int* optionValuePtr_int = nullptr; // Con trỏ tới giá trị int trong GameOptions
        // Có thể thêm con trỏ cho float nếu cần

        // Giới hạn min/max cho giá trị
        int minValue;
        int maxValue;
    };

    std::vector<OptionUI> mOptionItems; // Danh sách các mục tùy chọn

    // Nút "Back"
    SDL_Texture* mBackButtonTexture;
    SDL_Rect mBackButtonRect;

    // Hàm tiện ích
    SDL_Texture* createTextTexture(const std::string& text, SDL_Color color);
    void setupOptionItemUI(OptionUI& item, const std::string& labelText, int* valuePtr, int minVal, int maxVal,
        OptionsMenuAction decAction, OptionsMenuAction incAction, int yPos);
    void destroyOptionItemTextures(OptionUI& item);
};

#endif // OPTIONSMENU_H
