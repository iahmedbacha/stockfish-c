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

double space_area (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, space_area, NULL);
    }
    double v = 0;
    double r = rank(pos, square, NULL);
    double f = file(pos, square, NULL);
    if ((r >= 2 && r <= 4 && f >= 3 && f <= 6) && (board(pos, square->x ,square->y) != 'P') && (board(pos, square->x - 1 ,square->y - 1) != 'p') && (board(pos, square->x + 1 ,square->y - 1) != 'p')) {
        v++;
        if (board(pos, square->x, square->y - 1) == 'P' || board(pos, square->x, square->y - 2) == 'P' || board(pos, square->x, square->y - 3) == 'P') {
            v++;
        }
    }
    return v;
}

double space (Pos* pos, Square* square) {
    Pos colorflippos;
    colorflip(pos, &colorflippos);
    if (non_pawn_material(pos, NULL, NULL) + non_pawn_material(&colorflippos, NULL, NULL) < 12222) {
        return 0;
    }
    double weight = 0;
    for (int x = 0; x < 8; x++) {
        int open = TRUE;
        for (int y = 0; y < 8; y++) {
            if (indexOf("PNBRQK", 6, board(pos, x, y)) >= 0) {
                weight++;
            }
            if (toupper(board(pos, x, y)) == 'P' ) {
                open = FALSE;
            }
        }
        if (open) {
            weight -= 2;
        }
    }
    return space_area(pos, NULL, NULL) * weight * weight / 16;
}
