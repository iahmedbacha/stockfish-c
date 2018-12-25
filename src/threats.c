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

double safe_pawn (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, safe_pawn, NULL);
    }
    if (board(pos, square->x, square->y) != 'P') {
        return 0;
    }
    if (attack(pos, square, NULL)) {
        return 1;
    }
    Square temp;
    temp.x = square->x;
    temp.y = 7-square->y;
    Pos* colorflippos = colorflip(pos);
    if (!attack(colorflippos, &temp, NULL)) {
        free(colorflippos);
        return 1;
    }
    free (colorflippos);
    return 0;
}

double threat_safe_pawn (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, threat_safe_pawn, NULL);
    }
    if (indexOf("nbrq", 4, board(pos, square->x, square->y)) < 0) {
        return 0;
    }
    if (!pawn_attack(pos, square, NULL)) {
        return 0;
    }
    Square temp1;
    temp1.x = square->x - 1;
    temp1.y = square->y + 1;
    Square temp2;
    temp2.x = square->x + 1;
    temp2.y = square->y + 1;
    if (safe_pawn(pos, &temp1, NULL) || safe_pawn(pos, &temp2, NULL)) {
        return 1;
    }
    return 0;
}

double weak_enemies (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, weak_enemies, NULL);
    }
    if (indexOf("pnbrqk", 6, board(pos, square->x, square->y)) < 0) {
        return 0;
    }
    if (board(pos, square->x - 1, square->y - 1) == 'p') {
        return 0;
    }
    if (board(pos, square->x + 1, square->y - 1) == 'p') {
        return 0;
    }
    if (!attack(pos, square, NULL)) {
        return 0;
    }
    Square temp;
    temp.x = square->x;
    temp.y = 7-square->y;
    Pos* colorflippos = colorflip(pos);
    if (attack(pos, square, NULL) <= 1 && attack(colorflippos, &temp, NULL) > 1) {
        free(colorflippos);
        return 0;
    }
    free(colorflippos);
    return 1;
}

double minor_threat (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, minor_threat, NULL);
    }
    int type = indexOf("pnbrqk", 6, board(pos, square->x, square->y));
    if (type < 0) {
        return 0;
    }
    if (!knight_attack(pos, square, NULL) && !bishop_xray_attack(pos, square, NULL)) {
        return 0;
    }
    Square temp;
    temp.x = square->x;
    temp.y = 7-square->y;
    Pos* colorflippos = colorflip(pos);
    if ((board(pos, square->x, square->y) == 'p' || !(board(pos, square->x - 1, square->y - 1) == 'p' || board(pos, square->x + 1, square->y - 1) == 'p' || (attack(pos, square, NULL) <= 1 && attack(colorflippos, &temp, NULL) > 1))) && !weak_enemies(pos, square, NULL)) {
        free(colorflippos);
        return 0;
    }
    free(colorflippos);
    return type + 1;
}

double rook_threat (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, rook_threat, NULL);
    }
    int type = indexOf("pnbrqk", 6, board(pos, square->x, square->y));
    if (type < 0) {
        return 0;
    }
    if (!weak_enemies(pos, square, NULL)) {
        return 0;
    }
    if (!rook_xray_attack(pos, square, NULL)) {
        return 0;
    }
    return type + 1;
}

double hanging (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, hanging, NULL);
    }
    if (!weak_enemies(pos, square, NULL)) {
        return 0;
    }
    Square temp;
    temp.x = square->x;
    temp.y = 7-square->y;
    Pos* colorflippos = colorflip(pos);
    if (attack(colorflippos, &temp, NULL)) {
        free(colorflippos);
        return 0;
    }
    free(colorflippos);
    return 1;
}

double king_threat (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, king_threat, NULL);
    }
    if (indexOf("pnbrq", 5, board(pos, square->x, square->y)) < 0) {
        return 0;
    }
    if (!weak_enemies(pos, square, NULL)) {
        return 0;
    }
    if (!king_attack(pos, square, NULL)) {
        return 0;
    }
    return 1;
}

double pawn_push_threat (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, pawn_push_threat, NULL);
    }
    if (indexOf("pnbrqk", 6, board(pos, square->x, square->y)) < 0) {
        return 0;
    }
    Square temp1;
    Square temp2;
    for (int ix = -1; ix <= 1; ix += 2) {
        temp1.x = square->x+ix;
        temp1.y = square->y+1;
        temp2.x = square->x+ix;
        temp2.y = 6-square->y;
        Pos* colorflippos = colorflip(pos);
        if (board(pos, square->x + ix, square->y + 2) == 'P' && board(pos, square->x + ix, square->y + 1) == '-' && board(pos, square->x + ix - 1, square->y) != 'p' && board(pos, square->x + ix + 1, square->y) != 'p' && (attack(pos, &temp1, NULL) || !attack(colorflippos, &temp2, NULL))) {
            free(colorflippos);
            return 1;
        }
        temp1.x = square->x+ix;
        temp1.y = square->y+1;
        temp2.x = square->x+ix;
        temp2.y = 6-square->y;
        if (square->y == 3 && board(pos, square->x + ix, square->y + 3) == 'P' && board(pos, square->x + ix, square->y + 2) == '-' && board(pos, square->x + ix, square->y + 1) == '-' && board(pos, square->x + ix - 1, square->y) != 'p' && board(pos, square->x + ix + 1, square->y) != 'p' && (attack(pos, &temp1, NULL) || !attack(colorflippos, &temp2, NULL))) {
            free(colorflippos);
            return 1;
        }
    }
    return 0;
}

double rank_threat (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, rank_threat, NULL);
    }
    return ((minor_threat(pos, square, NULL) > 1) + (rook_threat(pos, square, NULL) > 1)) * square->y;
}

double weak_unopposed_pawn (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, weak_unopposed_pawn, NULL);
    }
    Pos* colorflippos = colorflip(pos);
    if (rook_count(colorflippos, NULL, NULL) + queen_count(colorflippos, NULL, NULL) == 0) {
        free(colorflippos);
        return 0;
    }
    free(colorflippos);
    if (opposed(pos, square, NULL)) {
        return 0;
    }
    double v = 0;
    if (isolated(pos, square, NULL)) {
        v++;
    }
    if (backward(pos, square, NULL)) {
        v++;
    }
    return v;
}

double overload (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, overload, NULL);
    }
    if (indexOf("nbrq", 4, board(pos, square->x, square->y)) < 0) {
        return 0;
    }
    if (!weak_enemies(pos, square, NULL)) {
        return 0;
    }
    Square temp;
    temp.x = square->x;
    temp.y = 7-square->y;
    Pos* colorflippos = colorflip(pos);
    if (!attack(colorflippos, &temp, NULL)) {
        free(colorflippos);
        return 0;
    }
    free(colorflippos);
    return 1;
}

double slider_on_queen (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, slider_on_queen, NULL);
    }
    Pos* pos2 = colorflip(pos);
    if (queen_count(pos2, NULL, NULL) != 1) {
        free(pos2);
        return 0;
    }
    free(pos2);
    if (board(pos, square->x - 1, square->y - 1) == 'p') {
        return 0;
    }
    if (board(pos, square->x + 1, square->y - 1) == 'p') {
        return 0;
    }
    if (attack(pos, square, NULL) <= 1) {
        return 0;
    }
    if (!mobility_area(pos, square, NULL)) {
        return 0;
    }
    Square temp;
    temp.x = square->x;
    temp.y = 7-square->y;
    double diagonal = queen_attack_diagonal(pos2, &temp, NULL);
    if (diagonal && bishop_xray_attack(pos, square, NULL)) {
        return 1;
    }
    if (!diagonal && rook_xray_attack(pos, square, NULL) && queen_attack(pos2, &temp, NULL)) {
        return 1;
    }
    return 0;
}

double knight_on_queen (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, knight_on_queen, NULL);
    }
    Pos* pos2 = colorflip(pos);
    int qx = -1;
    int qy = -1;
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            if (board(pos, x, y) == 'q') {
                if (qx >= 0 || qy >= 0) {
                    free(pos2);
                    return 0;
                }
                qx = x;
                qy = y;
            }
        }
    }
    if (queen_count(pos2, NULL, NULL) != 1) {
        free(pos2);
        return 0;
    }
    if (board(pos, square->x - 1, square->y - 1) == 'p') {
        free(pos2);
        return 0;
    }
    if (board(pos, square->x + 1, square->y - 1) == 'p') {
        free(pos2);
        return 0;
    }
    Square temp;
    temp.x = square->x;
    temp.y = 7-square->y;
    if (attack(pos, square, NULL) <= 1 && attack(pos2, &temp, NULL) > 1) {
        free(pos2);
        return 0;
    }
    free(pos2);
    if (!mobility_area(pos, square, NULL)) {
        return 0;
    }
    if (!knight_attack(pos, square, NULL)) {
        return 0;
    }
    if (fabs(qx-square->x) == 2 && fabs(qy-square->y) == 1) {
        return 1;
    }
    if (fabs(qx-square->x) == 1 && fabs(qy-square->y) == 2) {
        return 1;
    }
    return 0;
}

double threats_mg (Pos* pos) {
    double v = 0;
    v += 57 * hanging(pos, NULL, NULL);
    v += 13 * overload(pos, NULL, NULL);
    v += king_threat(pos, NULL, NULL) > 0 ? 23 : 0;
    v += 45 * pawn_push_threat(pos, NULL, NULL);
    v += 16 * rank_threat(pos, NULL, NULL);
    v += 173 * threat_safe_pawn(pos, NULL, NULL);
    v -= 5 * weak_unopposed_pawn(pos, NULL, NULL);
    v += 42 * slider_on_queen(pos, NULL, NULL);
    v += 21 * knight_on_queen(pos, NULL, NULL);
    Square s;
    int a1[7] = {0,0,39,57,68,62,0};
    int a2[7] = {0,0,38,38,0,51,0};
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            s.x = x;
            s.y = y;
            v += a1[(int) minor_threat(pos, &s, NULL)];
            v += a2[(int) rook_threat(pos, &s, NULL)];
        }
    }
    return v;
}

double threats_eg (Pos* pos) {
    double v = 0;
    v += 32 * hanging(pos, NULL, NULL);
    v += 6 * overload(pos, NULL, NULL);
    v += king_threat(pos, NULL, NULL) > 0 ? 76 : 0;
    v += 40 * pawn_push_threat(pos, NULL, NULL);
    v += 3 * rank_threat(pos, NULL, NULL);
    v += 102 * threat_safe_pawn(pos, NULL, NULL);
    v -= 29 * weak_unopposed_pawn(pos, NULL, NULL);
    v += 21 * slider_on_queen(pos, NULL, NULL);
    v += 11 * knight_on_queen(pos, NULL, NULL);
    Square s;
    int a1[7] = {0,31,42,44,112,120,0};
    int a2[7] = {0,24,71,61,38,38,0};
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            s.x = x;
            s.y = y;
            v += a1[(int) minor_threat(pos, &s, NULL)];
            v += a2[(int) rook_threat(pos, &s, NULL)];
        }
    }
    return v;
}
