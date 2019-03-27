# Stockfish C
## Overview
Stockfish main evaluation in C.
## Files
This distribution of Stockfish consists of the following files:
- README.md, the file you are currently reading.
- LICENCE.md, a text file containing the GNU General Public License.
- src, a subdirectory containing the full source code.
## Usage
1. Add src files into your C project.
2. Include "global" and "eval" headers in your target C file:
```
#include <global.h>
#include <eval.h>
```
3. Use "global.h" to define the position (Pos structure) that you want to evaluate. Below you will find an example of the initial position (pay attention to placement, it looks like you are looking from under the table). The whites are in capital letters.
```
Pos pos;
char initial[8][8] = {
    {'r','p','-','-','-','-','P','R'},
    {'n','p','-','-','-','-','P','N'},
    {'b','p','-','-','-','-','P','B'},
    {'q','p','-','-','-','-','P','Q'},
    {'k','p','-','-','-','-','P','K'},
    {'b','p','-','-','-','-','P','B'},
    {'n','p','-','-','-','-','P','N'},
    {'r','p','-','-','-','-','P','R'}
};
for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
        pos.b[i][j] = initial[i][j];
    }
}
for (int i = 0; i < 4; i++) {
    pos.c[i] = TRUE;
}
for (int i = 0; i < 2; i++) {
    pos.e[i] = NULL;
}
pos.w = TRUE;
pos.m[0] = 0;
pos.m[1] = 1;
```
4. Use "eval.h" to evaluate the position by calling main_evaluation function. The continuation of the previous exemple:
```
double me = main_evaluation(&pos);
```
5. Use the result to play the best move !
## Contributing
Contributions are always welcome!
## TODO
- Add tests.
## Licence
GNU General Public License v3.0
## Credits
- The Stockfish engine was developed by Tord Romstad, Marco Costalba, and Joona Kiiski using C++. It is now being developed and maintained by the Stockfish community. Find the project repository here : https://github.com/official-stockfish/Stockfish
- We used Stockfish evaluation function implemented in javascript and we reimplemented it in C. Find it here : https://hxim.github.io/Stockfish-Evaluation-Guide/
