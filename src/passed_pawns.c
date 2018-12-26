#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include "global.h"
#include "eval.h"
#include "attack.h"
#include "helpers.h"
#include "imbalance.h"
#include "initiative.h"
#include "king.h"
#include "material.h"
#include "mobility.h"
#include "passed_pawns.h"
#include "pawns.h"
#include "pieces.h"
#include "space.h"
#include "threats.h"

double passed_square (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, passed_square, NULL);
    }
    for (int y = 0; y < square->y; y++) {
        if (board(pos, square->x - 1, y) == 'p') {
            return 0;
        }
        if (board(pos, square->x, y) == 'p') {
            return 0;
        }
        if (board(pos, square->x + 1, y) == 'p') {
            return 0;
        }
        if (board(pos, square->x, y) == 'P') {
            return 0;
        }
    }
    return 1;
}

double candidate_passed (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, candidate_passed, NULL);
    }
    if (board(pos, square->x, square->y) != 'P') {
        return 0;
    }
    int ty1 = 8;
    int ty2 = 8;
    int oy = 8;
    for (int y = square->y - 1; y >= 0; y--) {
        if (board(pos, square->x, y) == 'p') {
            ty1 = y;
        }
        if (board(pos, square->x - 1, y) == 'p' || board(pos, square->x + 1, y) == 'p') {
            ty2 = y;
        }
    }
    if (ty2 < square->y - 2 || ty1 < square->y - 1) return 0;
    if (ty2 >= square->y && ty1 == square->y - 1 && square->y < 4) {
        if (board(pos, square->x - 1, square->y + 1) == 'P' && board(pos, square->x - 1, square->y) != 'p' && board(pos, square->x - 2, square->y - 1) != 'p') {
            return 1;
        }
        if (board(pos, square->x + 1, square->y + 1) == 'P' && board(pos, square->x + 1, square->y) != 'p' && board(pos, square->x + 2, square->y - 1) != 'p') {
            return 1;
        }
    }
    if (board(pos, square->x, square->y - 1) == 'p') {
        return 0;
    }
    int count1 = (board(pos, square->x - 1, square->y - 1) == 'p' ? 1 : 0) + (board(pos, square->x + 1, square->y - 1) == 'p' ? 1 : 0) - supported(pos, square, NULL) - 1;
    int count2 = (board(pos, square->x - 1, square->y - 2) == 'p' ? 1 : 0) + (board(pos, square->x + 1, square->y - 2) == 'p' ? 1 : 0) - (board(pos, square->x - 1, square->y) == 'P' ? 1 : 0) - (board(pos, square->x + 1, square->y) == 'P' ? 1 : 0);
    if (count1 > 0 || count2 > 0) return 0;
    return 1;
}

double king_proximity (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, king_proximity, NULL);
    }
    if (!candidate_passed(pos, square, NULL)) {
        return 0;
    }
    double r = rank(pos, square, NULL)-1;
    int a[8] = {0,0,0,3,7,11,20,0};
    int rr = a[(int) r];
    double v = 0;
    if (rr <= 0) {
        return 0;
    }
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            if (board(pos, x, y) == 'k') {
                v += min(max(fabs(y - square->y + 1),fabs(x - square->x)),5) * 5 * rr;
            }
            if (board(pos, x, y) == 'K') {
                v -= min(max(fabs(y - square->y + 1),fabs(x - square->x)),5) * 2 * rr;
                if (square->y > 1) {
                    v -= min(max(fabs(y - square->y + 2),fabs(x - square->x)),5) * rr;
                }
            }
        }
    }
    return v;
}

double passed_block (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, passed_block, NULL);
    }
    if (!candidate_passed(pos, square, NULL)) {
        return 0;
    }
    if (rank(pos, square, NULL) < 4) {
        return 0;
    }
    if (board(pos, square->x, square->y - 1) != '-') {
        return 0;
    }
    Square temp;
    double r = rank(pos, square, NULL) - 1;
    int a[8] = {0,0,0,3,7,11,20,0};
    int rr = a[(int) r];
    Pos pos2;
    colorflip(pos, &pos2);
    int defended = 0;
    int unsafe = 0;
    int defended1 = 0;
    int unsafe1 = 0;
    for (int y = square->y - 1; y >= 0; y--) {
        temp.x = square->x;
        temp.y = y;
        if (attack(pos, &temp, NULL)) {
            defended++;
        }
        temp.x = square->x;
        temp.y = 7-y;
        if (indexOf("pnbrqk", 6, board(pos, square->x, y)) >= 0 || attack(&pos2, &temp, NULL)) {
            unsafe++;
        }
        if (y == square->y - 1) {
            defended1 = defended;
            unsafe1 = unsafe;
        }
    }
    for (int y = square->y + 1; y < 8; y++) {
        if (board(pos, square->x, y) == 'R' || board(pos, square->x, y) == 'Q') {
            defended1 = defended = square->y;
        }
        if (board(pos, square->x, y) == 'r' || board(pos, square->x, y) == 'q') {
            unsafe1 = unsafe = square->y;
        }
        if (board(pos, square->x, y) != '-') {
            break;
        }
    }
    double k = (unsafe == 0 ? 20: unsafe1 == 0 ? 9 : 0) + (defended == square->y ? 6 : defended1 != 0 ? 4 : 0);
    return k * rr;
}

double passed_file (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, passed_file, NULL);
    }
    if (!candidate_passed(pos, square, NULL)) {
        return 0;
    }
    double f = file(pos, square, NULL);
    return min(f, 9 - f);
}

double passed_rank (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, passed_rank, NULL);
    }
    if (!candidate_passed(pos, square, NULL)) {
        return 0;
    }
    return rank(pos, square, NULL) - 1;
}

double passed_mg (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, passed_mg, NULL);
    }
    if (!candidate_passed(pos, square, NULL)) {
        return 0;
    }
    Square temp;
    temp.x = square->x;
    temp.y = square->y-1;
    int a1[7] = {0,5,12,10,57,163,271};
    int a2[5] = {0,-1,0,-9,-30};
    double v = 0;
    v += a1[(int) passed_rank(pos, square, NULL)];
    v += passed_block(pos, square, NULL);
    if (!passed_square(pos,&temp, NULL) || toupper(board(pos, square->x, square->y-1)) == 'P') {
        v = (v / 2);
    }
    v += a2[(int) passed_file(pos, square, NULL)];
    return v;
}

double passed_eg (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, passed_eg, NULL);
    }
    if (!candidate_passed(pos, square, NULL)) {
        return 0;
    }
    Square temp;
    temp.x = square->x;
    temp.y = square->y-1;
    int a1[7] = {0,18,23,31,62,167,250};
    int a2[5] = {0,7,9,-8,-14};
    double v = 0;
    v += king_proximity(pos, square, NULL);
    v += a1[(int) passed_rank(pos, square, NULL)];
    v += passed_block(pos, square, NULL);
    if (!passed_square(pos, &temp, NULL) || toupper(board(pos, square->x, square->y-1)) == 'P') {
        v = (v / 2);
    }
    v += a2[(int) passed_file(pos, square, NULL)];
    return v;
}
