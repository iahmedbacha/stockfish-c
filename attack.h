#ifndef ATTACK_H_INCLUDED
#define ATTACK_H_INCLUDED

double pinned_direction (Pos* pos, Square* square, void* param);
double knight_attack (Pos* pos, Square* square, void* param);
double bishop_xray_attack (Pos* pos, Square* square, void* param);
double rook_xray_attack (Pos* pos, Square* square, void* param);
double queen_attack (Pos* pos, Square* square, void* param);
double pawn_attack (Pos* pos, Square* square, void* param);
double king_attack (Pos* pos, Square* square, void* param);
double attack (Pos* pos, Square* square, void* param);
double queen_attack_diagonal (Pos* pos, Square* square, void* param);
double pinned (Pos* pos, Square* square, void* param);

#endif // ATTACK_H_INCLUDED
