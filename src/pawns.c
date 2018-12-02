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

double isolated (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, isolated, NULL);
    }
    if (board(pos, square->x, square->y) != 'P') {
        return 0;
    }
    for (int y = 0 ; y < 8; y++) {
        if (board(pos, square->x - 1, y) == 'P') {
            return 0;
        }
        if (board(pos, square->x + 1, y) == 'P') {
            return 0;
        }
    }
    return 1;
}

double opposed (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, opposed, NULL);
    }
    if (board(pos, square->x, square->y) != 'P') {
        return 0;
    }
    for (int y = 0; y < square->y; y++) {
        if (board(pos, square->x, y) == 'p') {
            return 1;
        }
    }
    return 0;
}

double phalanx (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, phalanx, NULL);
    }
    if (board(pos, square->x, square->y) != 'P') {
        return 0;
    }
    if (board(pos, square->x - 1, square->y) == 'P') {
        return 1;
    }
    if (board(pos, square->x + 1, square->y) == 'P') {
        return 1;
    }
    return 0;
}

double supported (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, supported, NULL);
    }
    if (board(pos, square->x, square->y) != 'P') {
        return 0;
    }
    return (board(pos, square->x - 1, square->y + 1) == 'P' ? 1 : 0) + (board(pos, square->x + 1, square->y + 1) == 'P' ? 1 : 0);
}

double backward (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, backward, NULL);
    }
    if (board(pos, square->x, square->y) != 'P') {
        return 0;
    }
    for (int y = square->y; y < 8; y++) {
        if (board(pos, square->x - 1, y) == 'P'
        || board(pos, square->x + 1, y) == 'P') return 0;
    }
    if (isolated(pos, square, NULL)) return 0;
    if (board(pos, square->x - 1, square->y - 2) == 'p' || board(pos, square->x + 1, square->y - 2) == 'p' || board(pos, square->x, square->y - 1) == 'p') {
        return 1;
    }
    return 0;
}

double doubled (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, doubled, NULL);
    }
    if (board(pos, square->x, square->y) != 'P') {
        return 0;
    }
    if (board(pos, square->x, square->y + 1) != 'P') {
        return 0;
    }
    if (board(pos, square->x - 1, square->y + 1) == 'P') {
        return 0;
    }
    if (board(pos, square->x + 1, square->y + 1) == 'P') {
        return 0;
    }
    return 1;
}

double connected (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, connected, NULL);
    }
    if (supported(pos, square, NULL) || phalanx(pos, square, NULL)) {
        return 1;
    }
    return 0;
}

double connected_bonus (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, connected_bonus, NULL);
    }
    if (!connected(pos, square, NULL)) {
        return 0;
    }
    int seed[8] = {0, 13, 24, 18, 65, 100, 175, 330};
    int op = opposed(pos, square, NULL);
    double ph = phalanx(pos, square, NULL);
    double su = supported(pos, square, NULL);
    double r = rank(pos, square, NULL);
    if (r < 2 || r > 7) return 0;
    int v = seed[(int) r - 1];
    v += (ph ? (seed[(int) r] - seed[(int) r - 1]) >> 1 : 0);
    v >>= op;
    v += 17 * su;
    return v;
}

double pawns_mg (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, pawns_mg, NULL);
    }
    double v = 0;
    v -= isolated(pos, square, NULL) ? 5 : 0;
    v -= backward(pos, square, NULL) ? 9 : 0;
    v -= doubled(pos, square, NULL) ? 11 : 0;
    v += connected(pos, square, NULL) ?  connected_bonus(pos, square, NULL) : 0;
    return v;
}

double pawns_eg (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, pawns_eg, NULL);
    }
    double v = 0;
    v -= isolated(pos, square, NULL) ? 15 : 0;
    v -= backward(pos, square, NULL) ? 24 : 0;
    v -= doubled(pos, square, NULL) ? 56 : 0;
    v += connected(pos, square, NULL) ?  (int) (connected_bonus(pos, square, NULL) * (rank(pos, square, NULL) - 3) / 4) : 0;
    return v;
}
