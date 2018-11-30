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

int main() {
    Pos pos;
    char initial[8][8] = {
        {'r','-','P','-','-','-','-','R'},
        {'n','-','-','K','-','-','P','N'},
        {'b','p','-','-','-','-','P','B'},
        {'q','p','-','-','-','-','P','Q'},
        {'k','p','-','-','-','-','P','-'},
        {'b','p','-','-','-','-','P','B'},
        {'n','p','-','-','-','-','P','N'},
        {'r','p','-','-','-','-','P','R'}
    };
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            pos.b[i][j] = initial[i][j];
        }
    }
    for (int i = 0; i < 4; i++) {
        pos.c[i] = TRUE;
    }
    for (int i = 0; i < 2; i++) {
        pos.e[i] = NULL;
    }
    pos.w = FALSE;
    pos.m[0] = 1;
    pos.m[1] = 2;
    //printf("main_evaluation = %.2f\n", main_evaluation(&pos));
    printf("king_proximity(pos, TRUE) = %.2f\n", king_proximity(&pos, NULL, NULL));
    return 0;
}
