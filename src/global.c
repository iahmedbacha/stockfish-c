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

void colorflip (Pos* pos, Pos* colorflippos) {
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            char temp = pos->b[x][7-y];
            colorflippos->b[x][y] = temp;
            int color = toupper(colorflippos->b[x][y]) == colorflippos->b[x][y];
            colorflippos->b[x][y] = color ? tolower(colorflippos->b[x][y]) : toupper(colorflippos->b[x][y]);
        }
    }
    colorflippos->c[0] = pos->c[2];
    colorflippos->c[1] = pos->c[3];
    colorflippos->c[2] = pos->c[0];
    colorflippos->c[3] = pos->c[1];
    if (pos->e==NULL) {
        colorflippos->e==NULL;
    }
    else {
        colorflippos->e[0] = pos->e[0];
        colorflippos->e[1] = 7-pos->e[1];
    }
    colorflippos->w= !pos->w;
    colorflippos->m[0] = pos->m[0];
    colorflippos->m[1] = pos->m[1];
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
