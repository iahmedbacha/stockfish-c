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

double main_evaluation (Pos* pos) {
    double mg = middle_game_evaluation(pos);
    double eg = end_game_evaluation(pos, NULL);
    double p = phase(pos);
    double t = tempo(pos, NULL);
    eg = eg * scale_factor(pos, &eg) / 64;
    return (mg * p + eg * (128 - p)) / 128 + t;
}

double middle_game_evaluation (Pos* pos) {
    double v = 0;
    v += piece_value_mg(pos,NULL,NULL) - piece_value_mg(colorflip(pos),NULL,NULL);
    v += psqt_mg(pos,NULL,NULL) - psqt_mg(colorflip(pos),NULL,NULL);
    v += imbalance_total(pos,NULL);
    v += pawns_mg(pos,NULL,NULL) - pawns_mg(colorflip(pos),NULL,NULL);
    v += pieces_mg(pos,NULL,NULL) - pieces_mg(colorflip(pos),NULL,NULL);
    v += mobility_mg(pos,NULL,NULL) - mobility_mg(colorflip(pos),NULL,NULL);
    v += threats_mg(pos,NULL,NULL) - threats_mg(colorflip(pos),NULL,NULL);
    v += passed_mg(pos,NULL,NULL) - passed_mg(colorflip(pos),NULL,NULL);
    v += space(pos,NULL) - space(colorflip(pos),NULL);
    v += king_mg(pos) - king_mg(colorflip(pos));
    return v;
}

double end_game_evaluation (Pos* pos, int* noinitiative) {
    double v = 0;
    v += piece_value_eg(pos,NULL,NULL) - piece_value_eg(colorflip(pos),NULL,NULL);
    v += psqt_eg(pos,NULL,NULL) - psqt_eg(colorflip(pos),NULL,NULL);
    v += imbalance_total(pos,NULL);
    v += pawns_eg(pos,NULL,NULL) - pawns_eg(colorflip(pos),NULL,NULL);
    v += pieces_eg(pos,NULL,NULL) - pieces_eg(colorflip(pos),NULL,NULL);
    v += mobility_eg(pos,NULL,NULL) - mobility_eg(colorflip(pos),NULL,NULL);
    v += threats_eg(pos,NULL,NULL) - threats_eg(colorflip(pos),NULL,NULL);
    v += passed_eg(pos,NULL,NULL) - passed_eg(colorflip(pos),NULL,NULL);
    v += king_eg(pos) - king_eg(colorflip(pos));
    if (noinitiative==NULL || !*noinitiative) {
        v += initiative_total(pos, &v);
    }
    return v;
}

double scale_factor (Pos* pos, double* eg) {
    if (eg == NULL) {
        *eg = end_game_evaluation(pos, NULL);
    }
    Pos* pos_w = *eg > 0 ? pos : colorflip(pos);
    Pos* pos_b = *eg > 0 ? colorflip(pos) : pos;
    double sf = 64;
    double pc_w = pawn_count(pos_w,NULL,NULL);
    double pc_b = pawn_count(pos_b,NULL,NULL);
    double npm_w = non_pawn_material(pos_w,NULL,NULL);
    double npm_b = non_pawn_material(pos_b,NULL,NULL);
    double bishopValueMg = 828;
    double bishopValueEg = 916;
    double rookValueMg = 1286;

    if (pc_w == 0 && npm_w - npm_b <= bishopValueMg) sf = npm_w < rookValueMg ? 0 : npm_b <= bishopValueMg ? 4 : 14;
    if (sf == 64) {
        double ob = opposite_bishops(pos,NULL);
        if (ob && npm_w == bishopValueMg && npm_b == bishopValueMg) {
            double asymmetry = 0;
            for (int x = 0; x < 8; x++) {
                int open[2] = {0, 0};
                for (int y = 0; y < 8; y++) {
                    if (board(pos, x, y) == 'p' || board(pos, x, y) == 'P' ) {
                        open[board(pos, x, y) == "P" ? 0 : 1] = 1;
                    }
                }
                if (open[0] + open[1] == 1) {
                    asymmetry++;
                }
            }
            asymmetry += candidate_passed(pos,NULL,NULL) + candidate_passed(colorflip(pos),NULL,NULL);
            sf = 8 + 4 * asymmetry;
        }
        else {
            double temp = 40 + (ob ? 2 : 7) * pc_w;
            sf = (sf<temp)?sf:temp;
        }
    }
    return sf;
}

double phase (Pos* pos) {
    double midgameLimit = 15258;
    double endgameLimit = 3915;
    double npm = non_pawn_material(pos,NULL,NULL) + non_pawn_material(colorflip(pos),NULL,NULL);
    npm = (npm<midgameLimit)?npm:midgameLimit;
    npm = (endgameLimit>npm)?endgameLimit:npm;
    return ((npm - endgameLimit) * 128) / (midgameLimit - endgameLimit);
}

double tempo (Pos* pos, Square* square) {
    return 20 * (pos->w ? 1 : -1);
}
