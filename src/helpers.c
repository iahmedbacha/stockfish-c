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

double rank (Pos* pos, Square* square, void* param) {
    if (square==NULL) {
        return sum(pos, rank, NULL);
    }
    return 8-square->y;
}

double file (Pos* pos, Square* square, void* param) {
    if (square==NULL) {
        return sum(pos, file, NULL);
    }
    return 1+square->x;
}

double bishop_count (Pos* pos, Square* square, void* param) {
    if (square==NULL) {
        return sum(pos, bishop_count, NULL);
    }
    if (board(pos,square->x,square->y)=='B') {
        return 1;
    }
    return 0;
}

double queen_count (Pos* pos, Square* square, void* param) {
    if (square==NULL) {
        return sum(pos, queen_count, NULL);
    }
    if (board(pos,square->x,square->y)=='Q') {
        return 1;
    }
    return 0;
}

double pawn_count (Pos* pos, Square* square, void* param) {
    if (square==NULL) {
        return sum(pos, pawn_count, NULL);
    }
    if (board(pos,square->x,square->y)=='P') {
        return 1;
    }
    return 0;
}

double knight_count (Pos* pos, Square* square, void* param) {
    if (square==NULL) {
        return sum(pos, knight_count, NULL);
    }
    if (board(pos,square->x,square->y)=='N') {
        return 1;
    }
    return 0;
}

double rook_count (Pos* pos, Square* square, void* param) {
    if (square==NULL) {
        return sum(pos, rook_count, NULL);
    }
    if (board(pos,square->x,square->y)=='R') {
        return 1;
    }
    return 0;
}

double opposite_bishops (Pos* pos, Square* square) {
    if (bishop_count(pos,NULL,NULL)!=1) {
        return 0;
    }
    Pos* colorflippos = colorflip(pos);
    if (bishop_count(colorflippos,NULL,NULL)!=1) {
        return 0;
    }
    int color[2] = {0, 0};
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            if (board(pos, x, y) == 'B') {
                color[0] = (x + y) % 2;
            }
            if (board(pos, x, y) == 'b') {
                color[1] = (x + y) % 2;
            }
        }
    }
    free(colorflippos);
    return color[0] == color[1] ? 0 : 1;
}

double king_distance (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, king_distance,NULL);
    }
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            if (board(pos, x, y) == 'K') {
                return max(fabs(x - square->x),fabs(y - square->y));
            }
        }
    }
    return 0;
}

double king_ring (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, king_ring,NULL);
    }
    for (int ix = -2; ix <= 2; ix++) {
        for (int iy = -2; iy <= 1; iy++) {
            if (board(pos, square->x + ix, square->y + iy) == 'k' && (iy >= -1 || square->y + iy == 0) && (ix >= -1 && ix <= 1 || square->x + ix == 0 || square->x + ix == 7)) {
                return 1;
            }
        }
    }
    return 0;
}

