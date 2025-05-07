#include "map.h"
#include "bomb.h" // Cần cho struct Explosion
#include <SDL_image.h> // Mặc dù không load texture ở đây, nhưng có thể cần nếu Map tự quản lý texture
#include <random>   // Cho std::rand và std::srand
#include <vector>
#include <iostream> // Cho std::cerr

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
    mCurrentSoftWallTextureIndex(0), // Khởi tạo chỉ số texture tường mềm
    mTileSize(40), // Kích thước ô mặc định, sẽ được tính lại trong initialize
    mRows(0),
    mColumns(0)
{
    if (!mRenderer) {
        std::cerr << "Map Error: Renderer is null in Map constructor!" << std::endl;
    }
    // Kiểm tra các texture khác nếu cần
}

Map::~Map() {
    // Hiện tại Map không sở hữu các texture, nên không cần giải phóng chúng ở đây.
    // Việc giải phóng texture được thực hiện ở lớp Game.
}

bool Map::initialize(int screenWidth, int screenHeight) {
    if (screenWidth <= 0 || screenHeight <= 0) {
        std::cerr << "Map Error: Invalid screen dimensions provided for initialization." << std::endl;
        return false;
    }

    // Tính toán số cột và hàng dựa trên kích thước màn hình và tileSize mong muốn
    // Ví dụ: chúng ta muốn có khoảng 15 hàng và 20 cột (có thể điều chỉnh)
    // Hoặc có thể cố định tileSize và tính số hàng/cột
    // Ở đây, chúng ta sẽ cố định số cột và tính tileSize, sau đó tính số hàng

    const int DESIRED_COLUMNS = 20; // Số cột mong muốn (ví dụ)
    mTileSize = screenWidth / DESIRED_COLUMNS;
    if (mTileSize < 20) mTileSize = 20; // Kích thước tối thiểu cho tile

    mColumns = DESIRED_COLUMNS;
    mRows = screenHeight / mTileSize;

    if (mRows <= 2 || mColumns <= 2) { // Cần ít nhất 3x3 để có tường biên và không gian chơi
        std::cerr << "Map Error: Calculated map dimensions are too small (" << mRows << "x" << mColumns << ") with tileSize " << mTileSize << std::endl;
        // Thử điều chỉnh lại tileSize hoặc số cột/hàng
        mTileSize = 40; // Reset về một giá trị an toàn hơn
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

    // 1. Tạo tường biên (BORDER_WALL)
    for (int r = 0; r < mRows; ++r) {
        for (int c = 0; c < mColumns; ++c) {
            if (r == 0 || r == mRows - 1 || c == 0 || c == mColumns - 1) {
                mLayout[r][c] = TileType::BORDER_WALL;
            }
        }
    }

    // 2. Tạo các khối HARD_WALL bên trong theo kiểu bàn cờ (cách ô)
    for (int r = 2; r < mRows - 2; r += 2) {
        for (int c = 2; c < mColumns - 2; c += 2) {
            mLayout[r][c] = TileType::HARD_WALL;
        }
    }

    // 3. Đặt các khối SOFT_WALL ngẫu nhiên vào các ô trống còn lại
    // Tránh đặt tường mềm ở các vị trí xuất phát của người chơi (ví dụ: 4 góc)
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, 2); // Xác suất đặt tường mềm (ví dụ: 1/3)

    for (int r = 1; r < mRows - 1; ++r) {
        for (int c = 1; c < mColumns - 1; ++c) {
            if (mLayout[r][c] == TileType::EMPTY) {
                // Tránh các vị trí xuất phát tiềm năng
                bool isSpawnArea = (r <= 2 && c <= 2) || // Góc trên trái
                    (r <= 2 && c >= mColumns - 3) || // Góc trên phải
                    (r >= mRows - 3 && c <= 2) || // Góc dưới trái
                    (r >= mRows - 3 && c >= mColumns - 3); // Góc dưới phải

                if (!isSpawnArea && distrib(gen) == 0) { // Xác suất 1/3 đặt tường mềm
                    mLayout[r][c] = TileType::SOFT_WALL;
                }
            }
        }
    }
    // Đảm bảo ô (1,1), (1,2), (2,1) là EMPTY cho vị trí xuất phát của player
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

    // 1. Vẽ nền (nếu có)
    if (mBackgroundTexture) {
        SDL_Rect destRect = { 0, 0, mColumns * mTileSize, mRows * mTileSize };
        // Điều chỉnh để background vừa với kích thước map tính toán, không phải screenWidth/Height
        SDL_RenderCopy(mRenderer, mBackgroundTexture, NULL, &destRect);
    }
    else {
        // Vẽ màu nền mặc định nếu không có texture
        SDL_SetRenderDrawColor(mRenderer, 100, 150, 100, 255); // Xanh lá cây nhạt
        SDL_RenderClear(mRenderer); // Chỉ clear nếu không có background texture
    }

    // 2. Vẽ các ô của bản đồ
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
                // Chọn một trong các texture tường mềm một cách ngẫu nhiên hoặc theo thứ tự
                // Để đơn giản, ta có thể dùng một texture cố định hoặc xoay vòng
                // Hoặc chọn ngẫu nhiên mỗi lần render (có thể không hiệu quả)
                // Ở đây, chúng ta sẽ dùng một texture cố định từ mảng dựa trên vị trí
                // để có sự đa dạng mà không cần rand() mỗi frame.
                mCurrentSoftWallTextureIndex = (r + c) % mSoftWallTextures.size();
                if (mSoftWallTextures[mCurrentSoftWallTextureIndex]) {
                    currentTileTexture = mSoftWallTextures[mCurrentSoftWallTextureIndex];
                }
                else if (mSoftWallTextures[0]) { // Fallback nếu texture cụ thể null
                    currentTileTexture = mSoftWallTextures[0];
                }
                break;
            case TileType::EMPTY:
            default:
                // Không vẽ gì cho ô trống, nền đã được vẽ
                break;
            }

            if (currentTileTexture) {
                SDL_RenderCopy(mRenderer, currentTileTexture, NULL, &tileRect);
            }
        }
    }
}

bool Map::isColliding(int x, int y, int entityWidth, int entityHeight) const {
    if (mLayout.empty()) return true; // Nếu map chưa được khởi tạo, coi như va chạm

    // Kiểm tra 4 góc của thực thể
    // Góc trên-trái
    int topLeftCol = x / mTileSize;
    int topLeftRow = y / mTileSize;
    if (getTileType(topLeftRow, topLeftCol) != TileType::EMPTY) return true;

    // Góc trên-phải
    int topRightCol = (x + entityWidth - 1) / mTileSize; // -1 để tránh tràn sang ô kế tiếp nếu vừa khít
    int topRightRow = y / mTileSize;
    if (getTileType(topRightRow, topRightCol) != TileType::EMPTY) return true;

    // Góc dưới-trái
    int bottomLeftCol = x / mTileSize;
    int bottomLeftRow = (y + entityHeight - 1) / mTileSize;
    if (getTileType(bottomLeftRow, bottomLeftCol) != TileType::EMPTY) return true;

    // Góc dưới-phải
    int bottomRightCol = (x + entityWidth - 1) / mTileSize;
    int bottomRightRow = (y + entityHeight - 1) / mTileSize;
    if (getTileType(bottomRightRow, bottomRightCol) != TileType::EMPTY) return true;

    // Có thể thêm kiểm tra các điểm giữa các cạnh nếu muốn chính xác hơn cho các thực thể lớn
    // hoặc khi di chuyển nhanh. Hiện tại, 4 góc là đủ cho game kiểu Bomberman.

    return false; // Không có va chạm
}

TileType Map::getTileType(int row, int col) const {
    // Kiểm tra biên
    if (row < 0 || row >= mRows || col < 0 || col >= mColumns) {
        return TileType::BORDER_WALL; // Coi như là tường biên nếu ra ngoài bản đồ
    }
    if (mLayout.empty() || mLayout[row].empty()) { // Kiểm tra layout có hợp lệ không
        return TileType::HARD_WALL; // Trả về tường cứng nếu layout không hợp lệ
    }
    return mLayout[row][col];
}

int Map::handleExplosion(const Explosion& explosion) {
    int softWallsDestroyedCount = 0;
    if (mLayout.empty()) return 0;

    for (const auto& part : explosion.parts) {
        // Chuyển tọa độ pixel của vụ nổ thành tọa độ ô
        int tileCol = part.x / mTileSize;
        int tileRow = part.y / mTileSize;

        // Kiểm tra xem ô có nằm trong bản đồ không
        if (tileRow >= 0 && tileRow < mRows && tileCol >= 0 && tileCol < mColumns) {
            if (mLayout[tileRow][tileCol] == TileType::SOFT_WALL) {
                mLayout[tileRow][tileCol] = TileType::EMPTY; // Phá hủy tường mềm
                softWallsDestroyedCount++;
                // TODO: Có thể thêm logic tạo item rơi ra từ tường bị phá ở đây
                // Ví dụ: if (rand() % 3 == 0) { createPowerUp(tileCol, tileRow); }
            }
            // Vụ nổ không phá hủy HARD_WALL hoặc BORDER_WALL
        }
    }
    return softWallsDestroyedCount; // Trả về số lượng tường mềm đã bị phá hủy
}
