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

double pinned_direction (Pos* pos, Square* square, void* param) {
    return 0;
}

double knight_attack (Pos* pos, Square* square, void* param) {
    return 0;
}

double bishop_xray_attack (Pos* pos, Square* square, void* param) {
    return 0;
}

double rook_xray_attack (Pos* pos, Square* square, void* param) {
    return 0;
}

double queen_attack (Pos* pos, Square* square, void* param) {
    return 0;
}

double pawn_attack (Pos* pos, Square* square, void* param) {
    return 0;
}

double king_attack (Pos* pos, Square* square, void* param) {
    return 0;
}

double attack (Pos* pos, Square* square, void* param) {
    return 0;
}

double queen_attack_diagonal (Pos* pos, Square* square, void* param) {
    return 0;
}

double pinned (Pos* pos, Square* square, void* param) {
    return 0;
}
