#ifndef MAP_H
#define MAP_H

#include <SDL.h>
#include <vector>
#include <array>
#include <string> 

struct Explosion;

enum class TileType {
    EMPTY,
    SOFT_WALL,
    HARD_WALL,
    BORDER_WALL 
};

class Map {
public:
    Map(SDL_Renderer* renderer,
        SDL_Texture* backgroundTexture,
        SDL_Texture* hardWallTexture,
        SDL_Texture* borderWallTexture,
        const std::array<SDL_Texture*, 3>& softWallTextures); 

    ~Map(); 

    
    bool initialize(int screenWidth, int screenHeight);

    void render();

    bool isColliding(int x, int y, int entityWidth, int entityHeight) const;

    TileType getTileType(int row, int col) const;

    int handleExplosion(const Explosion& explosion);

    int getTileSize() const { return mTileSize; }
    int getRows() const { return mRows; }
    int getColumns() const { return mColumns; }

private:
    SDL_Renderer* mRenderer; 

    SDL_Texture* mBackgroundTexture;
    SDL_Texture* mHardWallTexture;
    SDL_Texture* mBorderWallTexture;
    std::array<SDL_Texture*, 3> mSoftWallTextures; 
    int mCurrentSoftWallTextureIndex; 

    std::vector<std::vector<TileType>> mLayout;

    int mTileSize; 
    int mRows;    
    int mColumns;  

    void generateInitialLayout();
};

#endif // MAP_H
