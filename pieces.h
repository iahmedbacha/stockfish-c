#ifndef PIECES_H_INCLUDED
#define PIECES_H_INCLUDED

double outpost (Pos* pos, Square* square, void* param);
double outpost_square (Pos* pos, Square* square, void* param);
double reachable_outpost (Pos* pos, Square* square, void* param);
double minor_behind_pawn (Pos* pos, Square* square, void* param);
double bishop_pawns (Pos* pos, Square* square, void* param);
double rook_on_pawn (Pos* pos, Square* square, void* param);
double rook_on_file (Pos* pos, Square* square, void* param);
double trapped_rook (Pos* pos, Square* square, void* param);
double weak_queen (Pos* pos, Square* square, void* param);
double king_protector (Pos* pos, Square* square, void* param);
double long_diagonal_bishop (Pos* pos, Square* square, void* param);
double knight_outpost (Pos* pos, Square* square, void* param);
double bishop_outpost (Pos* pos, Square* square, void* param);
double pieces_mg (Pos* pos, Square* square, void* param);
double pieces_eg (Pos* pos, Square* square, void* param);

#endif // PIECES_H_INCLUDED
