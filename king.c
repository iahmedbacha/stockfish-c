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
    return 0;
}

double strength_square (Pos* pos, Square* square, void* param) {
    return 0;
}

double storm_square (Pos* pos, Square* square, void* param) {
    return 0;
}

double shelter_strength (Pos* pos) {
    return 0;
}

double shelter_storm (Pos* pos) {
    return 0;
}

double king_danger (Pos* pos) {
    return 0;
}

double king_pawn_distance (Pos* pos) {
    return 0;
}

double close_enemies (Pos* pos, Square* square, void* param) {
    return 0;
}

double check (Pos* pos, Square* square, void* param) {
    return 0;
}

double safe_check (Pos* pos, Square* square, void* param) {
    return 0;
}

double king_attackers_count (Pos* pos, Square* square, void* param) {
    return 0;
}

double king_attackers_weight (Pos* pos, Square* square, void* param) {
    return 0;
}

double king_attacks (Pos* pos, Square* square, void* param) {
    return 0;
}

double weak_bonus (Pos* pos, Square* square, void* param) {
    return 0;
}

double weak_squares (Pos* pos, Square* square, void* param) {
    return 0;
}

double unsafe_checks (Pos* pos, Square* square, void* param) {
    return 0;
}

double king_mg (Pos* pos) {
    return 0;
}

double king_eg (Pos* pos) {
    return 0;
}
