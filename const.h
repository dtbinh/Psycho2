#ifndef CONSTANTS_H
#define CONSTANTS_H

using namespace std;

extern const int NBNODES;
extern const int NBPATHS;
extern const int NBMARBLES;
extern const int BOARDSIZE;

extern const string DATAPATH;
extern const string PATHSFILE;
extern const string OPENINGFILE;
extern const string CURPOSFILE;

extern const bool VERBOSE;
extern const bool GUI;
extern const int TIMEOUT;

static void warningMsg (string s) { cout << s << endl; }
static void exitError (string s) { cout << s << endl; exit (EXIT_FAILURE); }

enum MarbleType { PKP, PSY, INF, DEL, X };

#endif
