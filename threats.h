#ifndef THREATS_H_INCLUDED
#define THREATS_H_INCLUDED

double safe_pawn (Pos* pos, Square* square, void* param);
double threat_safe_pawn (Pos* pos, Square* square, void* param);
double weak_enemies (Pos* pos, Square* square, void* param);
double minor_threat (Pos* pos, Square* square, void* param);
double rook_threat (Pos* pos, Square* square, void* param);
double hanging (Pos* pos, Square* square, void* param);
double king_threat (Pos* pos, Square* square, void* param);
double pawn_push_threat (Pos* pos, Square* square, void* param);
double rank_threat (Pos* pos, Square* square, void* param);
double weak_unopposed_pawn (Pos* pos, Square* square, void* param);
double overload (Pos* pos, Square* square, void* param);
double slider_on_queen (Pos* pos, Square* square, void* param);
double knight_on_queen (Pos* pos, Square* square, void* param);
double threats_mg (Pos* pos, Square* square, void* param);
double threats_eg (Pos* pos, Square* square, void* param);

#endif // THREATS_H_INCLUDED
