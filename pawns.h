#ifndef PAWNS_H_INCLUDED
#define PAWNS_H_INCLUDED

double isolated (Pos* pos, Square* square, void* param);
double opposed (Pos* pos, Square* square, void* param);
double phalanx (Pos* pos, Square* square, void* param);
double supported (Pos* pos, Square* square, void* param);
double backward (Pos* pos, Square* square, void* param);
double doubled (Pos* pos, Square* square, void* param);
double connected (Pos* pos, Square* square, void* param);
double connected_bonus (Pos* pos, Square* square, void* param);
double pawns_mg (Pos* pos, Square* square, void* param);
double pawns_eg (Pos* pos, Square* square, void* param);

#endif // PAWNS_H_INCLUDED
