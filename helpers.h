#ifndef HELPERS_H_INCLUDED
#define HELPERS_H_INCLUDED

double rank (Pos* pos, Square* square, void* param);
double file (Pos* pos, Square* square, void* param);
double bishop_count (Pos* pos, Square* square, void* param);
double queen_count (Pos* pos, Square* square, void* param);
double pawn_count (Pos* pos, Square* square, void* param);
double knight_count (Pos* pos, Square* square, void* param);
double rook_count (Pos* pos, Square* square, void* param);
double opposite_bishops (Pos* pos, Square* square);
double king_distance (Pos* pos, Square* square, void* param);
double king_ring (Pos* pos, Square* square, void* param);

#endif // HELPERS_H_INCLUDED
