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

double pawnless_flank (Pos* pos) {
    int pawns[8] = {0,0,0,0,0,0,0,0};
    int kx = 0;
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            if (toupper(board(pos, x, y)) == 'P') {
                pawns[x]++;
            }
            if (board(pos, x, y) == 'k') {
                kx = x;
            }
        }
    }
    int sum;
    if (kx == 0) {
        sum = pawns[0] + pawns[1] + pawns[2];
    }
    else if (kx < 3) {
        sum = pawns[0] + pawns[1] + pawns[2] + pawns[3];
    }
    else if (kx < 5) {
        sum = pawns[2] + pawns[3] + pawns[4] + pawns[5];
    }
    else if (kx < 7) {
        sum = pawns[4] + pawns[5] + pawns[6] + pawns[7];
    }
    else  {
        sum = pawns[5] + pawns[6] + pawns[7];
    }
    return sum == 0 ? 1 : 0;
}

double strength_square (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, strength_square, NULL);
    }
    double v = 5;
    int kx = min(6, max(1, square->x));
    int weakness[4][7] = {
        {-6,81,93,58,39,18,25},
        {-43,61,35,-49,-29,-11,-63},
        {-10,75,23,-2,32,3,-45},
        {-39,-13,-29,-52,-48,-67,-166}
    };
    for (int x = kx - 1; x <= kx +1; x++) {
        int us = 0;
        for (int y = 7; y >= square->y; y--) {
            if (board(pos, x, y) == 'p') {
                us = y;
            }
        }
        int f = min(x, 7 - x);
        v += weakness[f][us]? weakness[f][us] : 0;
    }
    return v;
}

double storm_square (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, storm_square, NULL);
    }
    double v = 0;
    int kx = min(6, max(1, square->x));
    int unblockedstorm[4][7] = {
        {89,107,123,93,57,45,51},
        {44,-18,123,46,39,-7,23},
        {4,52,162,37,7,-14,-2},
        {-10,-14,90,15,2,-7,-16}
    };
    for (int x = kx - 1; x <= kx +1; x++) {
        int us = 0;
        int them = 0;
        for (int y = 7; y >= square->y; y--) {
            if (board(pos, x, y) == 'p') {
                us = y;
            }
            if (board(pos, x, y) == 'P') {
                them = y;
            }
        }
        int f = min(x, 7 - x);
        if (us > 0 && them == us + 1) {
            v += 66 * (them == 2);
        }
        else {
            v += unblockedstorm[f][them];
        }
    }
    if ((square->x == 0 || square->x == 7) && (square->y == 0 || square->y == 1) && board(pos, square->x, square->y + 1) == 'P') {
        v -= 369;
    }
    return v;
}

double shelter_strength (Pos* pos, Square* square) {
    double w = 0;
    double s = 1024;
    double txtemp = 0;
    double* tx = NULL;
    Square temp;
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            if (board(pos, x, y) == 'k' || pos->c[2] && x == 6 && y == 0 || pos->c[3] && x == 2 && y == 0) {
                temp.x = x;
                temp.y = y;
                int w1 = strength_square(pos, &temp, NULL);
                int s1 = storm_square(pos, &temp, NULL);
                if (s1 - w1 < s - w) {
                    w = w1;
                    s = s1;
                    if (tx == NULL) {
                        tx = &txtemp;
                    }
                    *tx=max(1,min(6,x));
                }
            }
        }
    }

    if (square == NULL) {
        return w;
    }
    if (tx != NULL && board(pos, square->x, square->y) == 'p' && square->x >= tx-1 && square->x <= tx+1) {
        for (int y = square->y-1; y >= 0; y--) {
            if (board(pos, square->x, y) == 'p') {
                return 0;
            }
        }
        return 1;
    }
    return 0;
}

double shelter_storm (Pos* pos, Square* square) {
    double w = 0;
    double s = 1024;
    double txtemp = 0;
    double* tx = NULL;
    Square temp;
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            if (board(pos, x, y) == 'k' || pos->c[2] && x == 6 && y == 0 || pos->c[3] && x == 2 && y == 0) {
                temp.x = x;
                temp.y = y;
                int w1 = strength_square(pos, &temp, NULL);
                int s1 = storm_square(pos, &temp, NULL);
                if (s1 - w1 < s - w) {
                    w = w1;
                    s = s1;
                    if (tx == NULL) {
                        tx = &txtemp;
                    }
                    *tx=max(1,min(6,x));
                }
            }
        }
    }

    if (square == NULL) {
        return s;
    }
    if (tx != NULL && toupper(board(pos, square->x, square->y)) == 'P' && square->x >= tx-1 && square->x <= tx+1) {
        for (int y = square->y-1; y >= 0; y--) {
            if (board(pos, square->x, y) == board(pos, square->x, square->y)) {
                return 0;
            }
        }
        return 1;
    }
    return 0;
}

double king_danger (Pos* pos) {
    if (non_pawn_material(pos, NULL, NULL) < 782+1289) {
        return 0;
    }
    double count = king_attackers_count(pos, NULL, NULL);
    if (count <= 1 - queen_count(pos, NULL, NULL)) {
        return 0;
    }
    double weight = king_attackers_weight(pos, NULL, NULL);
    double kingattacks = king_attacks(pos, NULL, NULL);
    double weak = weak_bonus(pos, NULL, NULL);
    double pins_uchcks = unsafe_checks(pos, NULL, NULL);
    double tropism = close_enemies(pos, NULL, NULL);
    double noqueen = (queen_count(pos, NULL, NULL) > 0 ? 0 : 1);
    Pos* colorflippos = colorflip(pos);
    double v = count * weight + 69 * kingattacks + 185 * weak + 150 * pins_uchcks + (tropism * tropism / 4) - 873 * noqueen - (6 * (shelter_strength(pos, NULL) - shelter_storm(pos, NULL)) / 8) + mobility_mg(pos, NULL, NULL) - mobility_mg(colorflippos, NULL, NULL) - 30 + 780 * (safe_check(pos, NULL, 3) > 0 ? 1 : 0) + 880 * (safe_check(pos, NULL, 2) > 0 ? 1 : 0) + 435 * (safe_check(pos, NULL, 1) > 0 ? 1 : 0) + 790 * (safe_check(pos, NULL, 0) > 0 ? 1 : 0);
    free(colorflippos);
    if (v > 0) {
        return v;
    }
    return 0;
}

double king_pawn_distance (Pos* pos, Square* square) {
    double v = 8;
    int kx = 0;
    int ky = 0;
    int px = 0;
    int py = 0;
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            if (board(pos, x, y) == 'K') {
                kx = x;
                ky = y;
            }
        }
    }
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            double dist = max(fabs(x-kx),fabs(y-ky));
            if (board(pos, x, y) == 'P' && dist < v) {
                px = x; py = y; v = dist;
            }
        }
    }
    if (v < 8 && (square == NULL || square->x == px && square->y == py)) {
        return v;
    }
    return 0;
}

double close_enemies (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, close_enemies, NULL);
    }
    if (square->y > 4) {
        return 0;
    }
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            if (board(pos, x, y) == 'k') {
                if (x == 0 && square->x > 2) {
                    return 0;
                }
                if (x < 3 && square->x > 3) {
                    return 0;
                }
                if (x >= 3 && x < 5 && (square->x < 2 || square->x > 5)) {
                    return 0;
                }
                if (x >= 5 && square->x < 4) {
                    return 0;
                }
                if (x == 7 && square->x < 5) {
                    return 0;
                }
            }
        }
    }
    double a = attack(pos, square, NULL);
    if (!a) {
        return 0;
    }
    return a > 1 ? 2 : 1;
}

double check (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, check, NULL);
    }
    int* type = (int*) param;
    if (rook_xray_attack(pos, square, NULL) && (type == NULL || type == 2 || type == 4) || queen_attack(pos, square, NULL) && (type == NULL || type == 3)) {
        for (int i = 0; i < 4; i++) {
            int ix = (i == 0 ? -1 : i == 1 ? 1 : 0);
            int iy = (i == 2 ? -1 : i == 3 ? 1 : 0);
            for (int d = 1; d < 8; d++) {
                char b = board(pos, square->x + d * ix, square->y + d * iy);
                if (b == 'k') {
                    return 1;
                }
                if (b != '-' && b != 'q') {
                    break;
                }
            }
        }
    }
    if (bishop_xray_attack(pos, square, NULL) && (type == NULL || type == 1 || type == 4) || queen_attack(pos, square, NULL) && (type == NULL || type == 3)) {
    for (int i = 0; i < 4; i++) {
        int ix = ((i > 1) * 2 - 1);
        int iy = ((i % 2 == 0) * 2 - 1);
        for (int d = 1; d < 8; d++) {
            char b = board(pos, square->x + d * ix, square->y + d * iy);
            if (b == 'k') {
                return 1;
            }
            if (b != '-' && b != 'q') {
                break;
            }
        }
    }
    }
    if (knight_attack(pos, square, NULL) && (type == NULL || type == 0 || type == 4)) {
        if (board(pos, square->x + 2, square->y + 1) == 'k' || board(pos, square->x + 2, square->y - 1) == 'k' || board(pos, square->x + 1, square->y + 2) == 'k' || board(pos, square->x + 1, square->y - 2) == 'k' || board(pos, square->x - 2, square->y + 1) == 'k' || board(pos, square->x - 2, square->y - 1) == 'k' || board(pos, square->x - 1, square->y + 2) == 'k' || board(pos, square->x - 1, square->y - 2) == 'k') {
            return 1;
        }
    }
    return 0;
}

double safe_check (Pos* pos, Square* square, void* param) {
    int* type = (int*) param;
    if (square == NULL) {
        return sum(pos, safe_check, type);
    }
    if (indexOf("PNBRQK", 6, board(pos, square->x, square->y)) >= 0) {
        return 0;
    }
    if (!check(pos, square, type)) {
        return 0;
    }
    Pos* pos2 = colorflip(pos);
    Square temp;
    temp.x = square->x;
    temp.y = 7-square->y;
    if ((!attack(pos2, &temp, NULL) || (weak_squares(pos, square, NULL) && attack(pos, square, NULL) > 1)) && (type != 3 || !queen_attack(pos2, &temp, NULL))) {
        return 1;
    }
    free(pos2);
    return 0;
}

double king_attackers_count (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, king_attackers_count, NULL);
    }
    if (indexOf("PNBRQ", 5, board(pos, square->x, square->y)) < 0) {
        return 0;
    }
    if (board(pos, square->x, square->y) == 'P') {
        Square temp;
        double v = 0;
        for (int dir = -1; dir <= 1; dir += 2) {
            int fr = board(pos, square->x + dir * 2, square->y) == 'P';
            temp.x = square->x+dir;
            temp.y = square->y-1;
            if (square->x + dir >= 0 && square->x + dir <= 7 && king_ring(pos, &temp, NULL)) {
                v = v + (fr ? 0.5 : 1);
            }
        }
        return v;
    }
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            Square s2;
            s2.x = x;
            s2.y = y;
            if (king_ring(pos, &s2, NULL)) {
                if (knight_attack(pos, &s2, square) || bishop_xray_attack(pos, &s2, square) || rook_xray_attack(pos, &s2, square) || queen_attack(pos, &s2, square)) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

double king_attackers_weight (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, king_attackers_weight, NULL);
    }
    if (king_attackers_count(pos, square, NULL)) {
        int a[5] = {0,77,55,44,10};
        return a[indexOf("PNBRQ", 5, board(pos, square->x, square->y))];
    }
    return 0;
}

double king_attacks (Pos* pos, Square* square, void* param) {
    if (non_pawn_material(pos, NULL, NULL) < 782+1289) {
        return 0;
    }
    if (square == NULL) {
        return sum(pos, king_attacks, NULL);
    }
    double v = 0;
    for (int ix = -1; ix <= 1; ix++) {
        for (int iy = -1; iy <= 1; iy++) {
            if (board(pos, square->x + ix, square->y + iy) == 'k') {
                v += knight_attack(pos, square, NULL);
                v += bishop_xray_attack(pos, square, NULL);
                v += rook_xray_attack(pos, square, NULL);
                v += queen_attack(pos, square, NULL);
            }
        }
    }
    return v;
}

double weak_bonus (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, weak_bonus, NULL);
    }
    if (!weak_squares(pos, square, NULL)) {
        return 0;
    }
    if (!king_ring(pos, square, NULL)) {
        return 0;
    }
    return 1;
}

double weak_squares (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, weak_squares, NULL);
    }
    if (attack(pos, square, NULL)) {
        Pos* pos2 = colorflip(pos);
        Square temp;
        temp.x = square->x;
        temp.y = 7-square->y;
        double a = attack(pos2, &temp, NULL);
        if (attack >= 2) {
            return 0;
        }
        if (attack == 0) {
            return 1;
        }
        if (king_attack(pos2, &temp, NULL) || queen_attack(pos2, &temp, NULL)) {
            return 1;
        }
        free(pos2);
    }
    return 0;
}

double unsafe_checks (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, unsafe_checks, NULL);
    }
    int type;

    Square temp;
    temp.x = square->x;
    temp.y = 7-square->y;
    Pos* colorflippos = colorflip(pos);
    if (pinned_direction(colorflippos, &temp, NULL)) {
        return 1;
    }
    if (!mobility_area(pos, square, NULL)) {
        return 0;
    }
    type = 0;
    if (check(pos, square, &type) && safe_check(pos, NULL, &type) == 0) {
        return 1;
    }
    type = 1;
    if (check(pos, square, &type) && safe_check(pos, NULL, &type) == 0) {
        return 1;
    }
    type = 2;
    if (check(pos, square, &type) && safe_check(pos, NULL, &type) == 0) {
        return 1;
    }
    free(colorflippos);
    return 0;
}

double king_mg (Pos* pos) {
    double v = 0;
    double kd = king_danger(pos);
    v -= shelter_strength(pos, NULL);
    v += shelter_storm(pos, NULL);
    v += kd * kd / 4096;
    v += 7 * close_enemies(pos, NULL, NULL);
    v += 18 * pawnless_flank(pos);
    return v;
}

double king_eg (Pos* pos) {
    double v = 0;
    v -= 16 * king_pawn_distance(pos, NULL);
    v += 94 * pawnless_flank(pos);
    v += king_danger(pos) / 16;
    return v;
}
