#include "Texture.hpp"

std::pair<int, int> getTileCoord(int i, int j, const std::vector<std::vector<int>>& mapState, const int MapHeight, const int MapWidth) {
    bool curGrass = (mapState[i][j] == 0);

    auto isGrass = [&](int x, int y) -> bool {
        // Treat out-of-bound tiles as grass
        if (x < 0 || x >= MapHeight || y < 0 || y >= MapWidth)
            return true;
        return mapState[x][y] == 0;
    };

    bool U = isGrass(i - 1, j);
    bool D = isGrass(i + 1, j);
    bool L = isGrass(i, j - 1);
    bool R = isGrass(i, j + 1);

    // Grass tile rules
    if (curGrass) {
        if (!U && !L) return {0, 0};         // top-left corner grass
        if (!U && L && R) return {1, 0};     // top edge grass
        if (!U && !R) return {2, 0};         // top-right corner grass
        if (!L && U && D) return {0, 1};     // left edge grass
        if (U && D && L && R) return {1, 1}; // center grass
        if (!R && U && D) return {2, 1};     // right edge grass
        if (!D && !L) return {0, 2};         // bottom-left corner grass
        if (!D && L && R) return {1, 2};     // bottom edge grass
        if (!D && !R) return {2, 2};         // bottom-right corner grass
        if (!U && L && !R) return {3, 0};    // wide-top grass
        if (U && D && L && !R) return {3, 1};// wide-center grass
        if (!D && L && !R) return {3, 2};    // wide-bottom grass
        return {4, 0};                       // standalone grass
    }
    // Dirt tile rules
    else {
        if (!U && !L) return {0, 3};         // top-left corner dirt
        if (!U && L && R) return {1, 3};     // top edge dirt
        if (!U && !R) return {2, 3};         // top-right corner dirt
        if (!L && U && D) return {0, 4};     // left edge dirt
        if (U && D && L && R) return {1, 4}; // center dirt
        if (!R && U && D) return {2, 4};     // right edge dirt
        if (!D && !L) return {0, 5};         // bottom-left corner dirt
        if (!D && L && R) return {1, 5};     // bottom edge dirt
        if (!D && !R) return {2, 5};         // bottom-right corner dirt
        if (!U && L && !R) return {4, 0};    // wide-top dirt (placeholder)
        if (U && D && L && !R) return {4, 1};// wide-center dirt
        if (!D && L && !R) return {4, 2};    // wide-bottom dirt
        return {4, 0};                       // standalone dirt (placeholder)
    }
}