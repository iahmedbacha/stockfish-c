#ifndef EVAL_H_INCLUDED
#define EVAL_H_INCLUDED

#include "global.h"

double main_evaluation (Pos* pos);
double middle_game_evaluation (Pos* pos);
double end_game_evaluation (Pos* pos, int* noinitiative);
double scale_factor (Pos* pos, double* eg);
double phase (Pos* pos);
double tempo (Pos* pos, Square* square);


#endif // EVAL_H_INCLUDED
