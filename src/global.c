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

char board (Pos* pos, int x, int y) {
    if (x >= 0 && x <= 7 && y >= 0 && y <= 7) {
        if (pos==NULL) {
        }
        return pos->b[x][y];
    }
}

Pos* colorflip (Pos* pos) {
    Pos* result = (Pos*)(malloc(sizeof(Pos)));
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            result->b[x][y] = pos->b[x][7-y];
            int color = toupper(result->b[x][y]) == result->b[x][y];
            result->b[x][y] = color ? tolower(result->b[x][y]) : toupper(result->b[x][y]);
        }
    }
    result->c[0] = pos->c[2];
    result->c[1] = pos->c[3];
    result->c[2] = pos->c[0];
    result->c[3] = pos->c[1];
    if (pos->e==NULL) {
        result->e==NULL;
    }
    else {
        result->e[0] = pos->e[0];
        result->e[1] = 7-pos->e[1];
    }
    result->w= !pos->w;
    result->m[0] = pos->m[0];
    result->m[1] = pos->m[1];
    return result;
}

double sum (Pos* pos, double (*func) (Pos* pos, Square* square, void* param), void* param) {
    double sum = 0;
    Square square;
    for (int x = 0; x < 8; x++) {
        for(int y = 0; y < 8; y++) {
            square.x=x;
            square.y=y;
            sum += func(pos, &square, param);
        }
    }
    return sum;
}

int indexOf (char* str, int length, char ch) {
    for (int i = 0; i < length; i++) {
        if (str[i]==ch) {
            return i;
        }
    }
    return -1;
}

double max (double a, double b) {
    return (a>b)?a:b;
}

double min (double a, double b) {
    return (a<b)?a:b;
}