#ifndef MENU_H
#define MENU_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <string>
#include <vector> 

enum class MenuAction {
    NONE,
    START_GAME,
    OPEN_OPTIONS, 
    EXIT_GAME
};

class Menu {
public:
    Menu(SDL_Renderer* renderer, TTF_Font* font, int screenWidth, int screenHeight, Mix_Music* menuMusic);
    ~Menu();

    bool initialize(const std::string& backgroundPath);

    MenuAction handleEvent(SDL_Event& e);

    void render();

    void playMusic();

private:
    SDL_Renderer* mRenderer;
    TTF_Font* mFont;       
    int mScreenWidth;
    int mScreenHeight;

    SDL_Texture* mMenuBackgroundTexture;

    SDL_Rect mStartButtonRect;
    SDL_Texture* mStartButtonTexture;

    SDL_Rect mOptionsButtonRect;     
    SDL_Texture* mOptionsButtonTexture;

    SDL_Rect mExitButtonRect;
    SDL_Texture* mExitButtonTexture;

    SDL_Color mButtonTextColor;      

    Mix_Music* mMenuMusic;

    SDL_Texture* createTextTexture(const std::string& text, SDL_Color color);
    SDL_Texture* loadTexture(const std::string& path);
};

#endif // MENU_H
