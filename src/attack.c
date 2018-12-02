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

double pinned_direction (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, pinned_direction,NULL);
    }
    if (indexOf("PNBRQK", 6, toupper(board(pos, square->x, square->y))) < 0) {
        return 0;
    }
    double color = 1;
    if (indexOf("PNBRQK", 6, board(pos, square->x, square->y)) < 0) {
        color = -1;
    }
    for (int i = 0; i < 8; i++) {
        double ix = (i + (i > 3)) % 3 - 1;
        double iy = ((i + (i > 3)) / 3) - 1;
        int king = FALSE;
        for (int d = 1; d < 8; d++) {
            char b = board(pos, square->x + d * ix, square->y + d * iy);
            if (b == 'K') {
                king = TRUE;
            }
            if (b != '-') {
                break;
            }
        }
        if (king) {
            for (int d = 1; d < 8; d++) {
                char b = board(pos, square->x - d * ix, square->y - d * iy);
                if (b == 'q' || b == 'b' && ix * iy != 0 || b == 'r' && ix * iy == 0) {
                    return fabs(ix + iy * 3) * color;
                }
                if (b != '-') {
                    break;
                }
            }
        }
    }
    return 0;
}

double knight_attack (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, knight_attack, NULL);
    }
    Square* s2 = (Square*) param;
    Square temp;
    double v = 0;
    for (int i = 0; i < 8; i++) {
        int ix = ((i > 3) + 1) * (((i % 4) > 1) * 2 - 1);
        int iy = (2 - (i > 3)) * ((i % 2 == 0) * 2 - 1);
        char b = board(pos, square->x + ix, square->y + iy);
        temp.x = square->x + ix;
        temp.y = square->y + iy;
        if (b == 'N' && (s2 == NULL || s2->x == square->x + ix && s2->y == square->y + iy) && !pinned(pos, &temp, NULL)) {
            v++;
        }
    }
    return v;
}

double bishop_xray_attack (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, bishop_xray_attack, NULL);
    }
    Square* s2 = (Square*) param;
    Square temp;
    double v = 0;
    for (int i = 0; i < 4; i++) {
        int ix = ((i > 1) * 2 - 1);
        int iy = ((i % 2 == 0) * 2 - 1);
        for (int d = 1; d < 8; d++) {
            char b = board(pos, square->x + d * ix, square->y + d * iy);
            if (b == 'B' && (s2 == NULL || s2->x == square->x + d * ix && s2->y == square->y + d * iy)) {
                temp.x = square->x+d*ix;
                temp.y = square->y+d*iy;
                double dir = pinned_direction(pos, &temp, NULL);
                if (dir == 0 || fabs(ix+iy*3) == dir) {
                    v++;
                }
            }
            if (b != '-' && b != 'Q' && b != 'q') {
                break;
            }
        }
    }
    return v;
}

double rook_xray_attack (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, rook_xray_attack, NULL);
    }
    Square* s2 = (Square*) param;
    Square temp;
    double v = 0;
    for (int i = 0; i < 4; i++) {
        int ix = (i == 0 ? -1 : i == 1 ? 1 : 0);
        int iy = (i == 2 ? -1 : i == 3 ? 1 : 0);
        for (int d = 1; d < 8; d++) {
            char b = board(pos, square->x + d * ix, square->y + d * iy);
            if (b == 'R' && (s2 == NULL || s2->x == square->x + d * ix && s2->y == square->y + d * iy)) {
                temp.x = square->x+d*ix;
                temp.y = square->y+d*iy;
                double dir = pinned_direction(pos, &temp, NULL);
                if (dir == 0 || fabs(ix+iy*3) == dir) {
                    v++;
                }
            }
            if (b != '-' && b != 'R' && b != 'Q' && b != 'q') {
                break;
            }
        }
    }
    return v;
}

double queen_attack (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, queen_attack, NULL);
    }
    Square* s2 = (Square*) param;
    Square temp;
    double v = 0;
    for (int i = 0; i < 8; i++) {
        int ix = (i + (i > 3)) % 3 - 1;
        int iy = (((i + (i > 3)) / 3) << 0) - 1;
        for (int d = 1; d < 8; d++) {
            char b = board(pos, square->x + d * ix, square->y + d * iy);
            if (b == 'Q' && (s2 == NULL || s2->x == square->x + d * ix && s2->y == square->y + d * iy)) {
                temp.x = square->x+d*ix;
                temp.y = square->y+d*iy;
                double dir = pinned_direction(pos, &temp, NULL);
                if (dir == 0 || fabs(ix+iy*3) == dir) {
                    v++;
                }
            }
            if (b != '-') {
                break;
            }
        }
    }
    return v;
}

double pawn_attack (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, pawn_attack, NULL);
    }
    if (board(pos, square->x - 1, square->y + 1) == 'P') {
        return 1;
    }
    if (board(pos, square->x + 1, square->y + 1) == 'P') {
        return 1;
    }
    return 0;
}

double king_attack (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, king_attack, NULL);
    }
    for (int i = 0; i < 8; i++) {
        int ix = (i + (i > 3)) % 3 - 1;
        int iy = (((i + (i > 3)) / 3) << 0) - 1;
        if (board(pos, square->x + ix, square->y + iy) == 'K') {
            return 1;
        }
    }
    return 0;
}

double attack (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, attack, NULL);
    }
    double v = 0;
    v += pawn_attack(pos, square, NULL);
    v += king_attack(pos, square, NULL);
    v += knight_attack(pos, square, NULL);
    v += bishop_xray_attack(pos, square, NULL);
    v += rook_xray_attack(pos, square, NULL);
    v += queen_attack(pos, square, NULL);
    return v;
}

double queen_attack_diagonal (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, queen_attack_diagonal, NULL);
    }
    Square* s2 = (Square*) param;
    Square temp;
    double v = 0;
    for (int i = 0; i < 8; i++) {
        int ix = (i + (i > 3)) % 3 - 1;
        int iy = (((i + (i > 3)) / 3) << 0) - 1;
        if (ix == 0 || iy == 0) continue;
        for (int d = 1; d < 8; d++) {
            char b = board(pos, square->x + d * ix, square->y + d * iy);
            if (b == 'Q' && (s2 == NULL || s2->x == square->x + d * ix && s2->y == square->y + d * iy)) {
                temp.x = square->x+d*ix;
                temp.x = square->y+d*iy;
                double dir = pinned_direction(pos, &temp, NULL);
                if (dir == 0 || fabs(ix+iy*3) == dir) {
                    v++;
                }
            }
            if (b != '-') {
                break;
            }
        }
    }
    return v;
}

double pinned (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, pinned, NULL);
    }
    if (indexOf("PNBRQK", 6, board(pos, square->x, square->y)) < 0) {
        return 0;
    }
    return pinned_direction(pos, square, NULL) > 0 ? 1 : 0;
}
