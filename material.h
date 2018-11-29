#ifndef MATERIAL_H_INCLUDED
#define MATERIAL_H_INCLUDED

double non_pawn_material (Pos* pos, Square* square, void* param);
double piece_value_bonus (Pos* pos, Square* square, void* param);
double psqt_bonus (Pos* pos, Square* square, void* param);
double piece_value_mg (Pos* pos, Square* square, void* param);
double piece_value_eg (Pos* pos, Square* square, void* param);
double psqt_mg (Pos* pos, Square* square, void* param);
double psqt_eg (Pos* pos, Square* square, void* param);

#endif // MATERIAL_H_INCLUDED
