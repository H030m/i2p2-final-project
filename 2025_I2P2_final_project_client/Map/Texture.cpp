#include "Texture.hpp"
std::pair<int, int> getTileCoord(int i, int j, const std::vector<std::vector<int>>& mapState, const int MapHeight, const int MapWidth) {
    bool curGrass = (mapState[i][j] == 0);

    auto isGrass = [&](int x, int y) -> bool {
        if (x < 0 || x >= MapHeight || y < 0 || y >= MapWidth)
            return true;
        return mapState[x][y] == 0;
    };

    bool U = isGrass(i - 1, j);
    bool D = isGrass(i + 1, j);
    bool L = isGrass(i, j - 1);
    bool R = isGrass(i, j + 1);
    bool UL = isGrass(i-1, j-1);
    bool UR = isGrass(i+1, j-1);
    bool DL = isGrass(i-1, j+1);
    bool DR = isGrass(i+1, j+1);
    if (curGrass) {
        if (!U && !L && R && D) return {0, 0};         // ���W����
        if (!U && L && R && D) return {1, 0};     // �W��
        if (!U && !R && L && D) return {2, 0};         // �k�W����
        if (!L && U && D && R) return {0, 1};     // ����
        if (U && D && L && R){
            if(!UL) return {2,5};
            if(!UR) return {2,3};
            if(!DL) return {0,5};
            if(!DR) return {0,3};
            return {1, 1}; 
        }
        if (!R && U && D && L) return {2, 1};     // �k��
        if (!D && !L && R && U) return {0, 2};         // ���U����
        if (!D && L && R && U) return {1, 2};     // �U��
        if (!D && !R && L && U) return {2, 2};         // �k�U����
        if (!U && !L && !R && D) return {3, 0};    // �e�@�W��
        if (U && D && !L && !R) return {3, 1};// �e�@����
        if (!D && !L && !R && U) return {3, 2};    // �e�@�U��
        if (!U && !L && R && !D) return {3, 6};    // �e�@�W��R
        if (!U && !D && L && R) return {3, 7};// �e�@����R
        if (!D && L && !R && !U) return {3, 8};    // �e�@�U��R
        return {4, 0};                       // �@���
    } else {
        return {1, 4}; // ���g
    }
}