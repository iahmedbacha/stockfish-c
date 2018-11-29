#include <stdio.h>
#include <stdlib.h>

#include "global.h"
#include "helpers.h"

double rank (Pos* pos, Square* square, void* param) {
    if (square==NULL) {
        return sum(pos,&rank,NULL);
    }
    return 8-square->y;
}

double file (Pos* pos, Square* square, void* param) {
    if (square==NULL) {
        return sum(pos,&file,NULL);
    }
    return 1+square->x;
}

double bishop_count (Pos* pos, Square* square, void* param) {
    if (square==NULL) {
        return sum(pos,&bishop_count,NULL);
    }
    if (board(pos,square->x,square->y)=='B') {
        return 1;
    }
    return 0;
}

double queen_count (Pos* pos, Square* square, void* param) {
    if (square==NULL) {
        return sum(pos,&queen_count,NULL);
    }
    if (board(pos,square->x,square->y)=='Q') {
        return 1;
    }
    return 0;
}

double pawn_count (Pos* pos, Square* square, void* param) {
    if (square==NULL) {
        return sum(pos,&pawn_count,NULL);
    }
    if (board(pos,square->x,square->y)=='P') {
        return 1;
    }
    return 0;
}

double knight_count (Pos* pos, Square* square, void* param) {
    if (square==NULL) {
        return sum(pos,&knight_count,NULL);
    }
    if (board(pos,square->x,square->y)=='N') {
        return 1;
    }
    return 0;
}

double rook_count (Pos* pos, Square* square, void* param) {
    if (square==NULL) {
        return sum(pos,&rook_count,NULL);
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
    if (bishop_count(colorflip(pos),NULL,NULL)!=1) {
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
    return color[0] == color[1] ? 0 : 1;
}

double king_distance (Pos* pos, Square* square, void* param) {
    return 0;
}

double king_ring (Pos* pos, Square* square, void* param) {
    return 0;
}

