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
    return 0;
}

double threat_safe_pawn (Pos* pos, Square* square, void* param) {
    return 0;
}

double weak_enemies (Pos* pos, Square* square, void* param) {
    return 0;
}

double minor_threat (Pos* pos, Square* square, void* param) {
    return 0;
}

double rook_threat (Pos* pos, Square* square, void* param) {
    return 0;
}

double hanging (Pos* pos, Square* square, void* param) {
    return 0;
}

double king_threat (Pos* pos, Square* square, void* param) {
    return 0;
}

double pawn_push_threat (Pos* pos, Square* square, void* param) {
    return 0;
}

double rank_threat (Pos* pos, Square* square, void* param) {
    return 0;
}

double weak_unopposed_pawn (Pos* pos, Square* square, void* param) {
    return 0;
}

double overload (Pos* pos, Square* square, void* param) {
    return 0;
}

double slider_on_queen (Pos* pos, Square* square, void* param) {
    return 0;
}

double knight_on_queen (Pos* pos, Square* square, void* param) {
    return 0;
}

double threats_mg (Pos* pos, Square* square, void* param) {
    return 0;
}

double threats_eg (Pos* pos, Square* square, void* param) {
    return 0;
}
