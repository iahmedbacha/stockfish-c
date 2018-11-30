#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED

#define TRUE 1
#define FALSE 0

typedef struct {
    char b[8][8]; // chessboard
    int c[4]; // castling rights
    int e[2]; // enpassant
    int w; // side to move
    int m[2]; // move counts
}Pos;

typedef struct {
    int x;
    int y;
}Square;

char board (Pos* pos, int x, int y);
Pos* colorflip (Pos* pos);
double sum (Pos* pos, double (*func) (Pos* pos, Square* square, void* param), void* param);

int indexOf (char* str, int length, char ch);
double max (double a, double b);
double min (double a, double b);

#endif // GLOBAL_H_INCLUDED
