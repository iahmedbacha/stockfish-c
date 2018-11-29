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

double initiative (Pos* pos, Square* square) {
    double pawns = 0;
    double asymmetry = 0;
    int kx[2] = {0, 0};
    int ky[2] = {0, 0};
    int flanks[2] = {0, 0};
    for (int x = 0; x < 8; x++) {
        int open[2] = {0, 0};
        for (int y = 0; y < 8; y++) {
            if (toupper(board(pos, x, y)) == 'P' ) {
                open[board(pos, x, y) == 'P' ? 0 : 1] = 1;
                pawns++;
            }
            if (toupper(board(pos, x, y)) == 'K' ) {
                kx[board(pos, x, y) == 'K' ? 0 : 1] = x;
                ky[board(pos, x, y) == 'K' ? 0 : 1] = y;
            }
        }
        if (open[0] + open[1] == 1) asymmetry++;
        if (open[0] + open[1] > 0) flanks[x < 4 ? 0 : 1] = 1;
    }
    Pos* pos2 = colorflip(pos);
    asymmetry += candidate_passed(pos,NULL,NULL) + candidate_passed(pos2,NULL,NULL);
    double bothFlanks = flanks[0] && flanks[1] ? 1 : 0;
    double kingDistance = fabs(kx[0] - kx[1]) - fabs(ky[0] - ky[1]);
    double purePawn = (non_pawn_material(pos,NULL,NULL) + non_pawn_material(pos2,NULL,NULL)) == 0 ? 1 : 0;
    return 8 * asymmetry + 12 * pawns + 12 * kingDistance + 16 * bothFlanks + 48 * purePawn - 118;
}

double initiative_total (Pos* pos, double* v) {
    if (v == NULL) {
        v = (double*) (malloc(sizeof(double)));
        int noinitiative = TRUE;
        *v = end_game_evaluation(pos, &noinitiative);
    }
    return (*v > 0 ? 1 : *v < 0 ? -1 : 0) * max(initiative(pos,NULL), -fabs(*v));
}
