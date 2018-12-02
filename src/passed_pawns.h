#ifndef PASSED_PAWNS_H_INCLUDED
#define PASSED_PAWNS_H_INCLUDED

double passed_square (Pos* pos, Square* square, void* param);
double candidate_passed (Pos* pos, Square* square, void* param);
double king_proximity (Pos* pos, Square* square, void* param);
double passed_block (Pos* pos, Square* square, void* param);
double passed_file (Pos* pos, Square* square, void* param);
double passed_rank (Pos* pos, Square* square, void* param);
double passed_mg (Pos* pos, Square* square, void* param);
double passed_eg (Pos* pos, Square* square, void* param);

#endif // PASSED_PAWNS_H_INCLUDED
