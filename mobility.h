#ifndef MOBILITY_H_INCLUDED
#define MOBILITY_H_INCLUDED

double mobility (Pos* pos, Square* square, void* param);
double mobility_area (Pos* pos, Square* square, void* param);
double mobility_bonus (Pos* pos, Square* square, void* param);
double mobility_mg (Pos* pos, Square* square, void* param);
double mobility_eg (Pos* pos, Square* square, void* param);

#endif // MOBILITY_H_INCLUDED
