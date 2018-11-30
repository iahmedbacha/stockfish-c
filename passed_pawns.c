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

double passed_square (Pos* pos, Square* square, void* param) {
    return 0;
}

double candidate_passed (Pos* pos, Square* square, void* param) {
    return 0;
}

double king_proximity (Pos* pos, Square* square, void* param) {
    return 0;
}

double passed_block (Pos* pos, Square* square, void* param) {
    return 0;
}

double passed_file (Pos* pos, Square* square, void* param) {
    return 0;
}

double passed_rank (Pos* pos, Square* square, void* param) {
    return 0;
}

double passed_mg (Pos* pos, Square* square, void* param) {
    return 0;
}

double passed_eg (Pos* pos, Square* square, void* param) {
    return 0;
}
