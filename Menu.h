#ifndef MENU_H
#define MENU_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <string>
#include <vector> 

// Enum để xác định hành động người dùng chọn từ menu chính
enum class MenuAction {
    NONE,
    START_GAME,
    OPEN_OPTIONS, // << THÊM HÀNH ĐỘNG MỞ OPTIONS
    EXIT_GAME
};

class Menu {
public:
    // Constructor: Nhận renderer, font, kích thước màn hình và nhạc nền
    Menu(SDL_Renderer* renderer, TTF_Font* font, int screenWidth, int screenHeight, Mix_Music* menuMusic);
    ~Menu();

    // Khởi tạo menu, tải hình nền và tạo các nút
    bool initialize(const std::string& backgroundPath);

    // Xử lý sự kiện cho menu (ví dụ: click chuột)
    MenuAction handleEvent(SDL_Event& e);

    // Vẽ menu lên màn hình
    void render();

    // Phát nhạc nền cho menu
    void playMusic();

private:
    SDL_Renderer* mRenderer;
    TTF_Font* mFont;         // Font được truyền từ lớp Game
    int mScreenWidth;
    int mScreenHeight;

    SDL_Texture* mMenuBackgroundTexture; // Texture cho hình nền của menu

    // Các nút của menu chính
    SDL_Rect mStartButtonRect;
    SDL_Texture* mStartButtonTexture;

    SDL_Rect mOptionsButtonRect;      // << NÚT OPTIONS MỚI
    SDL_Texture* mOptionsButtonTexture; // << TEXTURE CHO NÚT OPTIONS

    SDL_Rect mExitButtonRect;
    SDL_Texture* mExitButtonTexture;

    SDL_Color mButtonTextColor;       // Màu chữ cho các nút

    Mix_Music* mMenuMusic; // Nhạc nền được truyền từ lớp Game

    // Hàm tiện ích riêng để tạo texture từ text
    SDL_Texture* createTextTexture(const std::string& text, SDL_Color color);
    // Hàm tiện ích riêng để tải texture hình ảnh
    SDL_Texture* loadTexture(const std::string& path);
};

#endif // MENU_H
