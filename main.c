#include <stdio.h>
#include <stdlib.h>

#include "global.h"
#include "eval.h"
#include "helpers.h"

int main() {
    Pos pos;
    char initial[8][8] = {
        {'r','p','-','-','-','-','P','R'},
        {'n','p','-','-','-','-','P','N'},
        {'b','p','-','-','-','-','P','B'},
        {'q','p','-','-','-','-','P','Q'},
        {'k','p','-','-','-','-','P','K'},
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
    pos.w = TRUE;
    pos.m[0] = 0;
    pos.m[1] = 1;
    printf("main_evaluation = %.2f\n", main_evaluation(&pos));
    printf("pawn_count = %.2f\n", pawn_count(&pos,NULL,NULL));
    return 0;
}
