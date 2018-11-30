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

double mobility (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, mobility, NULL);
    }
    Square s2;
    double v = 0;
    char b = board(pos, square->x, square->y);
    if (indexOf("NBRQ", 4, b) < 0) {
        return 0;
    }
    for (int x = 0; x < 8; x++) {
        for(int y = 0; y < 8; y++) {
            s2.x = x;
            s2.y = y;
            if (!mobility_area(pos, &s2, NULL)) {
                continue;
            }
            if (b == 'N' && knight_attack(pos, &s2, square) && board(pos, x, y) != 'Q') {
                v++;
            }
            if (b == 'B' && bishop_xray_attack(pos, &s2, square) && board(pos, x, y) != 'Q') {
                v++;
            }
            if (b == 'R' && rook_xray_attack(pos, &s2, square)) {
                v++;
            }
            if (b == 'Q' && queen_attack(pos, &s2, square)) {
                v++;
            }
        }
    }
    return v;
}

double mobility_area (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, mobility_area, NULL);
    }
    if (board(pos, square->x, square->y) == 'K') {
        return 0;
    }
    if (board(pos, square->x, square->y) == 'Q') {
        return 0;
    }
    if (board(pos, square->x - 1, square->y - 1) == 'p') {
        return 0;
    }
    if (board(pos, square->x + 1, square->y - 1) == 'p') {
        return 0;
    }
    if (board(pos, square->x, square->y) == 'P' && (rank(pos, square, NULL) < 4 || board(pos, square->x, square->y - 1) != '-')) {
        return 0;
    }
    return 1;
}

double mobility_bonus (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        int* mg = (int*) param;
        return sum(pos, mobility_bonus, mg);
    }
    int* mg = (int*) param;
    int bonus1[4][28] = {
        {-75,-57,-9,-2,6,14,22,29,36},
        {-48,-20,16,26,38,51,55,63,63,68,81,81,91,98},
        {-58,-27,-15,-10,-5,-2,9,16,30,29,32,38,46,48,58},
        {-39,-21,3,3,14,22,28,41,43,48,56,60,60,66,67,70,71,73,79,88,88,99,102,102,106,109,113,116}
    };
    int bonus2[4][28] = {
        {-76,-54,-28,-10,5,12,26,29,29},
        {-59,-23,-3,13,24,42,54,57,65,73,78,86,88,97},
        {-76,-18,28,55,69,82,112,118,132,142,155,165,166,169,171},
        {-36,-15,8,18,34,54,61,73,79,92,94,104,113,120,123,126,133,136,140,143,148,166,170,175,184,191,206,212}
    };
    int i = indexOf("NBRQ", 4, board(pos, square->x, square->y));
    if (i < 0) {
        return 0;
    }
    if (*mg) {
        return bonus1[i][(int) mobility(pos, square, NULL)];
    }
    else {
        return bonus2[i][(int) mobility(pos, square, NULL)];
    }
}

double mobility_mg (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, mobility_mg, NULL);
    }
    int temp = TRUE;
    return mobility_bonus(pos, square, (void*) &temp);
}

double mobility_eg (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, mobility_eg, NULL);
    }
    int temp = FALSE;
    return mobility_bonus(pos, square, (void*) &temp);
}
