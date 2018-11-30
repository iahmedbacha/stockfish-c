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

double non_pawn_material (Pos* pos, Square* square, void* param) {
    return 0;
}

double piece_value_bonus (Pos* pos, Square* square, void* param) {
    return 0;
}

double psqt_bonus (Pos* pos, Square* square, void* param) {
    return 0;
}

double piece_value_mg (Pos* pos, Square* square, void* param) {
    return 0;
}

double piece_value_eg (Pos* pos, Square* square, void* param) {
    return 0;
}

double psqt_mg (Pos* pos, Square* square, void* param) {
    return 0;
}

double psqt_eg (Pos* pos, Square* square, void* param) {
    return 0;
}

