#ifndef KING_H_INCLUDED
#define KING_H_INCLUDED

double pawnless_flank (Pos* pos);
double strength_square (Pos* pos, Square* square, void* param);
double storm_square (Pos* pos, Square* square, void* param);
double shelter_strength (Pos* pos);
double shelter_storm (Pos* pos);
double king_danger (Pos* pos);
double king_pawn_distance (Pos* pos);
double close_enemies (Pos* pos, Square* square, void* param);
double check (Pos* pos, Square* square, void* param);
double safe_check (Pos* pos, Square* square, void* param);
double king_attackers_count (Pos* pos, Square* square, void* param);
double king_attackers_weight (Pos* pos, Square* square, void* param);
double king_attacks (Pos* pos, Square* square, void* param);
double weak_bonus (Pos* pos, Square* square, void* param);
double weak_squares (Pos* pos, Square* square, void* param);
double unsafe_checks (Pos* pos, Square* square, void* param);
double king_mg (Pos* pos);
double king_eg (Pos* pos);

#endif // KING_H_INCLUDED
