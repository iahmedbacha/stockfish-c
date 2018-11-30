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

double isolated (Pos* pos, Square* square, void* param) {
    return 0;
}

double opposed (Pos* pos, Square* square, void* param) {
    return 0;
}

double phalanx (Pos* pos, Square* square, void* param) {
    return 0;
}

double supported (Pos* pos, Square* square, void* param) {
    return 0;
}

double backward (Pos* pos, Square* square, void* param) {
    return 0;
}

double doubled (Pos* pos, Square* square, void* param) {
    return 0;
}

double connected (Pos* pos, Square* square, void* param) {
    return 0;
}

double connected_bonus (Pos* pos, Square* square, void* param) {
    return 0;
}

double pawns_mg (Pos* pos, Square* square, void* param) {
    return 0;
}

double pawns_eg (Pos* pos, Square* square, void* param) {
    return 0;
}
