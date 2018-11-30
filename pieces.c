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

double outpost (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, outpost, NULL);
    }
    if (board(pos, square->x, square->y) != 'N' && board(pos, square->x, square->y) != 'B') {
        return 0;
    }
    if (!outpost_square(pos, square, NULL)) {
        return 0;
    }
    return 1;
}

double outpost_square (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, outpost_square, NULL);
    }
    if (rank(pos, square, NULL) < 4 || rank(pos, square, NULL) > 6) {
        return 0;
    }
    for (int y = 0; y < square->y; y++) {
        if (board(pos, square->x - 1, y) == 'p') {
            return 0;
        }
        if (board(pos, square->x + 1, y) == 'p') {
            return 0;
        }
    }
    return 1;
}



double reachable_outpost (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, reachable_outpost, NULL);
    }
    if (board(pos, square->x, square->y) != 'B' && board(pos, square->x, square->y) != 'N') {
        return 0;
    }
    Square temp;
    double v = 0;
    for (int x = 0; x < 8; x++) {
        for (int y = 2; y < 5; y++) {
            temp.x = x;
            temp.y = y;
            if ((board(pos, square->x, square->y) == 'N' && indexOf("PNBRQK", 6, board(pos, x, y)) < 0 && knight_attack(pos, &temp, square) && outpost_square(pos, &temp, NULL)) || (board(pos, square->x, square->y) == 'B' && indexOf("PNBRQK", 6, board(pos, x, y)) < 0 && bishop_xray_attack(pos, &temp, square) && outpost_square(pos, &temp, NULL))) {
                double support = board(pos, x - 1, y + 1) == 'P' || board(pos, x + 1, y + 1) == 'P' ? 2 : 1;
                v = max(v, support);
            }
        }
    }
    return v;
}

double minor_behind_pawn (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, minor_behind_pawn, NULL);
    }
    if (board(pos, square->x, square->y) != 'B' && board(pos, square->x, square->y) != 'N') {
        return 0;
    }
    if (toupper(board(pos, square->x, square->y - 1)) != 'P') {
        return 0;
    }
    return 1;
}

double bishop_pawns (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, bishop_pawns, NULL);
    }
    if (board(pos, square->x, square->y) != 'B') {
        return 0;
    }
    int c = (square->x + square->y) % 2;
    double v = 0;
    double blocked = 0;
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            if (board(pos, x, y) == 'P' && c == (x + y) % 2) {
                v++;
            }
            if (board(pos, x, y) == 'P' && x > 1 && x < 6 && board(pos, x, y - 1) != '-') {
                blocked++;
            }
        }
    }
    return v * (blocked + 1);
}

double rook_on_pawn (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, rook_on_pawn, NULL);
    }
    if (board(pos, square->x, square->y) != 'R') {
        return 0;
    }
    if (rank(pos, square, NULL) < 5) {
        return 0;
    }
    double v = 0;
    for (int x = 0; x < 8; x++) {
        if (board(pos, x, square->y) == 'p') {
            v++;
        }
    }
    for (int y = 0; y < 8; y++) {
        if (board(pos, square->x, y) == 'p') {
            v++;
        }
    }
    return v;
}

double rook_on_file (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, rook_on_file, NULL);
    }
    if (board(pos, square->x, square->y) != 'R') {
        return 0;
    }
    double open = 1;
    for (int y = 0; y < 8; y++) {
        if (board(pos, square->x, y) == 'P') {
            return 0;
        }
        if (board(pos, square->x, y) == 'p') {
            open = 0;
        }
    }
    return open + 1;
}

double trapped_rook (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, trapped_rook, NULL);
    }
    if (board(pos, square->x, square->y) != 'R') {
        return 0;
    }
    if (rook_on_file(pos, square, NULL)) {
        return 0;
    }
    double mob = mobility(pos, square, NULL);
    if (mob > 3) {
        return 0;
    }
    int kx = 0;
    int ky = 0;
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            if (board(pos, x, y) == 'K') {
                kx = x;
                ky = y;
            }
        }
    }
    if ((kx < 4) != (square->x < kx)) {
        return 0;
    }
    return (92 - mob * 22) * (pos->c[0] || pos->c[1] ? 1 : 2);
}

double weak_queen (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, weak_queen, NULL);
    }
    if (board(pos, square->x, square->y) != 'Q') {
        return 0;
    }
    for (int i = 0; i < 8; i++) {
        int ix = (i + (i > 3)) % 3 - 1;
        int iy = (((i + (i > 3)) / 3) << 0) - 1;
        int count = 0;
        for (int d = 1; d < 8; d++) {
            char b = board(pos, square->x + d * ix, square->y + d * iy);
            if (b == 'r' && (ix == 0 || iy == 0) && count == 1) {
                return 1;
            }
            if (b == 'b' && (ix != 0 && iy != 0) && count == 1) {
                return 1;
            }
            if (b != '-') {
                count++;
            }
        }
    }
    return 0;
}

double king_protector (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, king_protector, NULL);
    }
    if (board(pos, square->x, square->y) != 'N' && board(pos, square->x, square->y) != 'B') {
        return 0;
    }
    return -6 * king_distance(pos, square, NULL);
}

double long_diagonal_bishop (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, long_diagonal_bishop, NULL);
    }
    if (board(pos, square->x, square->y) != 'B') {
        return 0;
    }
    if (square->x - square->y != 0 && square->x - (7 - square->y) != 0) {
        return 0;
    }
    int x1 = square->x;
    int y1 = square->y;
    if (min(x1,7-x1) > 2) {
        return 0;
    }
    for (int i = min(x1,7-x1); i < 4; i++) {
        if (board(pos, x1, y1) == 'p') {
            return 0;
        }
        if (board(pos, x1, y1) == 'P') {
            return 0;
        }
        if (x1 < 4) {
            x1++;
        }
        else {
            x1--;
        }
        if (y1 < 4) {
            y1++;
        }
        else {
            y1--;
        }
    }
    return 1;
}

double knight_outpost (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, knight_outpost, NULL);
    }
    if (board(pos, square->x, square->y) != 'N') {
        return 0;
    }
    double reachable = 0;
    double support = 0;
    if (!outpost(pos, square, NULL)) {
        reachable = reachable_outpost(pos, square, NULL);
        if (!reachable) {
            return 0;
        }
        support = reachable > 1;
    }
    else {
        support = (board(pos, square->x - 1, square->y + 1) == 'P' || board(pos, square->x + 1, square->y + 1) == 'P');
    }
    return (support ? 2 : 1) + 2 * (reachable ? 0 : 1);
}

double bishop_outpost (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, bishop_outpost, NULL);
    }
    if (board(pos, square->x, square->y) != 'B') {
        return 0;
    }
    double reachable = 0;
    double support = 0;
    if (!outpost(pos, square, NULL)) {
        reachable = reachable_outpost(pos, square, NULL);
        if (!reachable) {
            return 0;
        }
        support = reachable > 1;
    }
    else {
        support = (board(pos, square->x - 1, square->y + 1) == 'P' || board(pos, square->x + 1, square->y + 1) == 'P');
    }
    return (support ? 2 : 1) + 2 * (reachable ? 0 : 1);
}

double pieces_mg (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, pieces_mg, NULL);
    }
    if (indexOf("NBRQ", 4, board(pos, square->x, square->y)) < 0) {
        return 0;
    }
    int a1[5] = {0,22,36,44,72};
    int a2[5] = {0,9,15,18,30};
    int a3[3] = {0,20,45};
    double v = 0;
    v += a1[(int) knight_outpost(pos, square, NULL)];
    v += a2[(int) bishop_outpost(pos, square, NULL)];
    v += 16 * minor_behind_pawn(pos, square, NULL);
    v -= 3 * bishop_pawns(pos, square, NULL);
    v += 10 * rook_on_pawn(pos, square, NULL);
    v += a3[(int) rook_on_file(pos, square, NULL)];
    v -= trapped_rook(pos, square, NULL);
    v -= 50 * weak_queen(pos, square, NULL);
    v += king_protector(pos, square, NULL);
    v += 46 * long_diagonal_bishop(pos, square, NULL);
    return v;
}

double pieces_eg (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, pieces_eg, NULL);
    }
    if (indexOf("NBRQ", 4, board(pos, square->x, square->y)) < 0) {
        return 0;
    }
    int a1[5] = {0,6,12,12,24};
    int a2[5] = {0,2,5,4,10};
    int a3[3] = {0,7,20};
    double v = 0;
    v += a1[(int) knight_outpost(pos, square, NULL)];
    v += a2[(int) bishop_outpost(pos, square, NULL)];
    v -= 7 * bishop_pawns(pos, square, NULL);
    v += 30 * rook_on_pawn(pos, square, NULL);
    v += a3[(int) rook_on_file(pos, square, NULL)];
    v -= 10 * weak_queen(pos, square, NULL);
    v += king_protector(pos, square, NULL);
    return v;
}
