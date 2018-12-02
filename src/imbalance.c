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

double imbalance (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, imbalance, NULL);
    }
    int qo[6][6] = {{0},{40,38},{32,255,-62},{0,104,4,0},{-26,-2,47,105,-208},{-189,24,117,133,-134,-6}};
    int qt[6][6] = {{0},{36,0},{9,63,0},{59,65,42,0},{46,39,24,-24,0},{97,100,-42,137,268,0}};
    int j = indexOf("XPNBRQxpnbrq", 12, board(pos, square->x, square->y));
    if (j < 0 || j > 5) {
        return 0;
    }
    int bishop[2] = {0, 0};
    double v = 0;
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            int i = indexOf("XPNBRQxpnbrq", 12, board(pos, x, y));
            if (i < 0) {
                continue;
            }
            if (i == 9) {
                bishop[0]++;
            }
            if (i == 3) {
                bishop[1]++;
            }
            if (i % 6 > j) {
                continue;
            }
            if (i > 5) {
                v += qt[j][i-6];
            }
            else {
                v += qo[j][i];
            }
        }
    }
    if (bishop[0] > 1) {
        v += qt[j][0];
    }
    if (bishop[1] > 1) {
        v += qo[j][0];
    }
    return v;
}

double bishop_pair (Pos* pos) {
    if (bishop_count(pos, NULL, NULL) > 1) {
        return 1438;
    }
    return 0;
}

double imbalance_total (Pos* pos, Square* square) {
    double v = 0;
    v += imbalance(pos, NULL, NULL) - imbalance(colorflip(pos), NULL, NULL);
    v += bishop_pair(pos) - bishop_pair(colorflip(pos));
    return v / 16;
}
