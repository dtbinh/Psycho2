#ifndef CONSTANTS_H
#define CONSTANTS_H

using namespace std;

extern const int NBNODES;           // number of nodes in board
extern const int NBPATHS;           // number of paths in board
extern const int NBMARBLES;         // number of marbles PER PLAYER
extern const int BOARDSIZE;         // total number of nodes in board

extern const string DATAPATH;       // file path to data folder
extern const string PATHSFILE;      // file path to paths file
extern const string OPENINGFILE;    // file path to initial positions of marbles file
extern const string CURPOSFILE;     // file path to current positions of marbles file

extern const bool VERBOSE;          // true -> display more informations
extern const bool GUI;              // true -> update GUI throught positions file
extern const int TIMEOUT;           // timeout before IA must play

extern const int ALPHABETA;         // represents alphabeta
extern const int HUMAN;             // represents human
extern const int RANDOM;            // represents random
extern const int BETTERRDM;         // represents betterrdm

static void warningMsg (string s) { cout << s << endl; }                        // displays warningMsg
static void exitError (string s) { cout << s << endl; exit (EXIT_FAILURE); }    // displays error and exit

enum MarbleType { PKP, PSY, INF, DEL, X };                                      // represents type of marbles

#endif
