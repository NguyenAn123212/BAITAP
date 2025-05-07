#include "map.h"
#include "bomb.h"
#include <SDL_image.h>
#include <random>
#include <vector>
#include <iostream>

Map::Map(SDL_Renderer* renderer,
    SDL_Texture* backgroundTexture,
    SDL_Texture* hardWallTexture,
    SDL_Texture* borderWallTexture,
    const std::array<SDL_Texture*, 3>& softWallTextures)
    : mRenderer(renderer),
    mBackgroundTexture(backgroundTexture),
    mHardWallTexture(hardWallTexture),
    mBorderWallTexture(borderWallTexture),
    mSoftWallTextures(softWallTextures),
    mCurrentSoftWallTextureIndex(0),
    mTileSize(40),
    mRows(0),
    mColumns(0)
{
    if (!mRenderer) {
        std::cerr << "Map Error: Renderer is null in Map constructor!" << std::endl;
    }
}

Map::~Map() {
}

bool Map::initialize(int screenWidth, int screenHeight) {
    if (screenWidth <= 0 || screenHeight <= 0) {
        std::cerr << "Map Error: Invalid screen dimensions provided for initialization." << std::endl;
        return false;
    }

    const int DESIRED_COLUMNS = 20;
    mTileSize = screenWidth / DESIRED_COLUMNS;
    if (mTileSize < 20) mTileSize = 20;

    mColumns = DESIRED_COLUMNS;
    mRows = screenHeight / mTileSize;

    if (mRows <= 2 || mColumns <= 2) {
        std::cerr << "Map Error: Calculated map dimensions are too small (" << mRows << "x" << mColumns << ") with tileSize " << mTileSize << std::endl;
        mTileSize = 40;
        mColumns = screenWidth / mTileSize;
        mRows = screenHeight / mTileSize;
        if (mRows <= 2 || mColumns <= 2) {
            std::cerr << "Map Critical Error: Still too small after adjustment. Check screen dimensions and tileSize logic." << std::endl;
            return false;
        }
    }

    std::cout << "Map Initialized: " << mRows << " rows, " << mColumns << " columns, TileSize: " << mTileSize << std::endl;

    generateInitialLayout();
    return true;
}

void Map::generateInitialLayout() {
    mLayout.assign(mRows, std::vector<TileType>(mColumns, TileType::EMPTY));

    for (int r = 0; r < mRows; ++r) {
        for (int c = 0; c < mColumns; ++c) {
            if (r == 0 || r == mRows - 1 || c == 0 || c == mColumns - 1) {
                mLayout[r][c] = TileType::BORDER_WALL;
            }
        }
    }

    for (int r = 2; r < mRows - 2; r += 2) {
        for (int c = 2; c < mColumns - 2; c += 2) {
            mLayout[r][c] = TileType::HARD_WALL;
        }
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, 2);

    for (int r = 1; r < mRows - 1; ++r) {
        for (int c = 1; c < mColumns - 1; ++c) {
            if (mLayout[r][c] == TileType::EMPTY) {
                bool isSpawnArea = (r <= 2 && c <= 2) ||
                    (r <= 2 && c >= mColumns - 3) ||
                    (r >= mRows - 3 && c <= 2) ||
                    (r >= mRows - 3 && c >= mColumns - 3);

                if (!isSpawnArea && distrib(gen) == 0) {
                    mLayout[r][c] = TileType::SOFT_WALL;
                }
            }
        }
    }
    if (mRows > 2 && mColumns > 2) {
        mLayout[1][1] = TileType::EMPTY;
    }
    if (mRows > 2 && mColumns > 3) {
        mLayout[1][2] = TileType::EMPTY;
    }
    if (mRows > 3 && mColumns > 2) {
        mLayout[2][1] = TileType::EMPTY;
    }

}

void Map::render() {
    if (!mRenderer) return;

    if (mBackgroundTexture) {
        SDL_Rect destRect = { 0, 0, mColumns * mTileSize, mRows * mTileSize };
        SDL_RenderCopy(mRenderer, mBackgroundTexture, NULL, &destRect);
    }
    else {
        SDL_SetRenderDrawColor(mRenderer, 100, 150, 100, 255);
        SDL_RenderClear(mRenderer);
    }

    for (int r = 0; r < mRows; ++r) {
        for (int c = 0; c < mColumns; ++c) {
            SDL_Rect tileRect = { c * mTileSize, r * mTileSize, mTileSize, mTileSize };
            SDL_Texture* currentTileTexture = nullptr;

            switch (mLayout[r][c]) {
            case TileType::BORDER_WALL:
                currentTileTexture = mBorderWallTexture;
                break;
            case TileType::HARD_WALL:
                currentTileTexture = mHardWallTexture;
                break;
            case TileType::SOFT_WALL:
                mCurrentSoftWallTextureIndex = (r + c) % mSoftWallTextures.size();
                if (mSoftWallTextures[mCurrentSoftWallTextureIndex]) {
                    currentTileTexture = mSoftWallTextures[mCurrentSoftWallTextureIndex];
                }
                else if (mSoftWallTextures[0]) {
                    currentTileTexture = mSoftWallTextures[0];
                }
                break;
            case TileType::EMPTY:
            default:
                break;
            }

            if (currentTileTexture) {
                SDL_RenderCopy(mRenderer, currentTileTexture, NULL, &tileRect);
            }
        }
    }
}

bool Map::isColliding(int x, int y, int entityWidth, int entityHeight) const {
    if (mLayout.empty()) return true;

    int topLeftCol = x / mTileSize;
    int topLeftRow = y / mTileSize;
    if (getTileType(topLeftRow, topLeftCol) != TileType::EMPTY) return true;

    int topRightCol = (x + entityWidth - 1) / mTileSize;
    int topRightRow = y / mTileSize;
    if (getTileType(topRightRow, topRightCol) != TileType::EMPTY) return true;

    int bottomLeftCol = x / mTileSize;
    int bottomLeftRow = (y + entityHeight - 1) / mTileSize;
    if (getTileType(bottomLeftRow, bottomLeftCol) != TileType::EMPTY) return true;

    int bottomRightCol = (x + entityWidth - 1) / mTileSize;
    int bottomRightRow = (y + entityHeight - 1) / mTileSize;
    if (getTileType(bottomRightRow, bottomRightCol) != TileType::EMPTY) return true;

    return false;
}

TileType Map::getTileType(int row, int col) const {
    if (row < 0 || row >= mRows || col < 0 || col >= mColumns) {
        return TileType::BORDER_WALL;
    }
    if (mLayout.empty() || mLayout[row].empty()) {
        return TileType::HARD_WALL;
    }
    return mLayout[row][col];
}

int Map::handleExplosion(const Explosion& explosion) {
    int softWallsDestroyedCount = 0;
    if (mLayout.empty()) return 0;

    for (const auto& part : explosion.parts) {
        int tileCol = part.x / mTileSize;
        int tileRow = part.y / mTileSize;

        if (tileRow >= 0 && tileRow < mRows && tileCol >= 0 && tileCol < mColumns) {
            if (mLayout[tileRow][tileCol] == TileType::SOFT_WALL) {
                mLayout[tileRow][tileCol] = TileType::EMPTY;
                softWallsDestroyedCount++;
            }
        }
    }
    return softWallsDestroyedCount;
}
