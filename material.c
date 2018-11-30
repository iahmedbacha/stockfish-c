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
    if (square == NULL) {
        return sum(pos, non_pawn_material, NULL);
    }
    int i = indexOf("NBRQ", 4, board(pos, square->x, square->y));
    if (i >= 0) {
        int temp = TRUE;
        return piece_value_bonus(pos, square, (void*) &temp);
    }
    return 0;
}

double piece_value_bonus (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, piece_value_bonus, param);
    }
    int* mg = (int*) param;
    int a1[5] = {142, 784, 828, 1286, 2528};
    int a2[5] = {207, 868, 916, 1378, 2698};
    int i = indexOf("PNBRQ", 5, board(pos, square->x, square->y));
    if (i >= 0) {
        if (*mg) {
            return a1[i];
        }
        else {
            return a2[i];
        }
    }
    return 0;
}

double psqt_bonus (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        int* mg = (int*) param;
        return sum(pos, psqt_bonus, mg);
    }
    int* mg = (int*) param;
    int bonus1[6][8][4] = {
        {{0,0,0,0},{-11,7,7,17},{-16,-3,23,23},{-14,-7,20,24},{-5,-2,-1,12},{-11,-12,-2,4},{-2,20,-10,-2},{0,0,0,0}},
        {{-161,-96,-80,-73},{-83,-43,-21,-10},{-71,-22,0,9},{-25,18,43,47},{-26,16,38,50},{-11,37,56,65},{-63,-19,5,14},{-195,-67,-42,-29}},
        {{-49,-7,-10,-34},{-24,9,15,1},{-9,22,-3,12},{4,9,18,40},{-8,27,13,30},{-17,14,-6,6},{-19,-13,7,-11},{-47,-7,-17,-29}},
        {{-25,-16,-16,-9},{-21,-8,-3,0},{-21,-9,-4,2},{-22,-6,-1,2},{-22,-7,0,1},{-21,-7,0,2},{-12,4,8,12},{-23,-15,-11,-5}},
        {{0,-4,-3,-1},{-4,6,9,8},{-2,6,9,9},{-1,8,10,7},{-3,9,8,7},{-2,6,8,10},{-2,7,7,6},{-1,-4,-1,0}},
        {{272,325,273,190},{277,305,241,183},{198,253,168,120},{169,191,136,108},{145,176,112,69},{122,159,85,36},{87,120,64,25},{64,87,49,0}}
    };
    int bonus2[6][8][4] = {
        {{0,0,0,0},{-3,-1,7,2},{-2,2,6,-1},{7,-4,-8,2},{13,10,-1,-8},{16,6,1,16},{1,-12,6,25},{0,0,0,0}},
        {{-105,-82,-46,-14},{-69,-54,-17,9},{-50,-39,-7,28},{-41,-25,6,38},{-46,-25,3,40},{-54,-38,-7,27},{-65,-50,-24,13},{-109,-89,-50,-13}},
        {{-58,-31,-37,-19},{-34,-9,-14,4},{-23,0,-3,16},{-26,-3,-5,16},{-26,-4,-7,14},{-24,-2,0,13},{-34,-10,-12,6},{-55,-32,-36,-17}},
        {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}},
        {{-71,-56,-42,-29},{-56,-30,-21,-5},{-39,-17,-8,5},{-29,-5,9,19},{-27,-5,10,21},{-40,-16,-10,3},{-55,-30,-21,-6},{-74,-55,-43,-30}},
        {{0,41,80,93},{57,98,138,131},{86,138,165,173},{103,152,168,169},{98,166,197,194},{87,164,174,189},{40,99,128,141},{5,60,75,75}}
    };
    int i = indexOf("PNBRQK", 6, board(pos, square->x, square->y));
    if (i < 0) {
        return 0;
    }
    if (*mg) {
        return bonus1[i][7 - square->y][(int) min(square->x, 7 - square->x)];
    }
    else {
        return bonus2[i][7 - square->y][(int) min(square->x, 7 - square->x)];
    }
}

double piece_value_mg (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, piece_value_mg, NULL);
    }
    int temp = TRUE;
    return piece_value_bonus(pos, square, (void*) &temp);
}

double piece_value_eg (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, piece_value_eg, NULL);
    }
    int temp = FALSE;
    return piece_value_bonus(pos, square, (void*) &temp);
}

double psqt_mg (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, psqt_mg, NULL);
    }
    int temp = TRUE;
    return psqt_bonus(pos, square, (void*) &temp);
}

double psqt_eg (Pos* pos, Square* square, void* param) {
    if (square == NULL) {
        return sum(pos, psqt_eg, NULL);
    }
    int temp = FALSE;
    return psqt_bonus(pos, square, (void*) &temp);
}

