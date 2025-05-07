#ifndef MAP_H
#define MAP_H

#include <SDL.h>
#include <vector>
#include <array>
#include <string> // Mặc dù không dùng trực tiếp, nhưng có thể cần cho việc tải map từ file sau này

// Forward declaration cho Explosion struct (được định nghĩa trong Bomb.h)
struct Explosion;

// Enum để định nghĩa các loại ô trên bản đồ
enum class TileType {
    EMPTY,
    SOFT_WALL,
    HARD_WALL,
    BORDER_WALL // Tường biên không thể phá hủy
};

class Map {
public:
    // Constructor: Nhận renderer và các textures cần thiết
    Map(SDL_Renderer* renderer,
        SDL_Texture* backgroundTexture,
        SDL_Texture* hardWallTexture,
        SDL_Texture* borderWallTexture,
        const std::array<SDL_Texture*, 3>& softWallTextures); // Mảng các texture cho tường mềm

    ~Map(); // Destructor (hiện tại không cần làm gì đặc biệt nếu texture được quản lý bên ngoài)

    // Khởi tạo bản đồ: thiết lập kích thước, layout, v.v.
    // screenWidth và screenHeight dùng để tính toán kích thước ô (tile)
    bool initialize(int screenWidth, int screenHeight);

    // Vẽ bản đồ lên màn hình
    void render();

    // Kiểm tra va chạm của một hình chữ nhật (ví dụ: player, enemy) với các ô tường
    // x, y là tọa độ góc trên bên trái của hình chữ nhật
    // width, height là chiều rộng và chiều cao của hình chữ nhật
    bool isColliding(int x, int y, int entityWidth, int entityHeight) const;

    // Lấy loại ô tại một vị trí hàng và cột cụ thể
    TileType getTileType(int row, int col) const;

    // Xử lý khi có một vụ nổ xảy ra trên bản đồ
    // Phá hủy các tường mềm trong phạm vi vụ nổ
    // Trả về số lượng tường mềm đã bị phá hủy
    int handleExplosion(const Explosion& explosion);

    // Getters
    int getTileSize() const { return mTileSize; }
    int getRows() const { return mRows; }
    int getColumns() const { return mColumns; }

private:
    SDL_Renderer* mRenderer; // Con trỏ tới SDL_Renderer (không sở hữu)

    // Textures cho các loại ô (không sở hữu, được truyền từ lớp Game)
    SDL_Texture* mBackgroundTexture;
    SDL_Texture* mHardWallTexture;
    SDL_Texture* mBorderWallTexture;
    std::array<SDL_Texture*, 3> mSoftWallTextures; // Mảng các texture cho tường mềm
    int mCurrentSoftWallTextureIndex; // Để chọn ngẫu nhiên texture cho tường mềm khi render

    // Layout của bản đồ: một vector 2 chiều chứa các TileType
    std::vector<std::vector<TileType>> mLayout;

    int mTileSize; // Kích thước của mỗi ô (ví dụ: 32x32 pixels)
    int mRows;     // Số hàng của bản đồ
    int mColumns;  // Số cột của bản đồ

    // Hàm tiện ích riêng để tạo layout ban đầu cho bản đồ
    void generateInitialLayout();
};

#endif // MAP_H
