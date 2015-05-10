#ifndef GAME_H
#define GAME_H

#include "board.h"
#include "minimax.h"
#include "const.h"

class Tree;

struct Node;
struct Path;
struct Marble;


class Game
{

private:

    void initBoard (string pathsFile);                  // initialize the board with nodes and paths
    void initMatch (string openingFile, int turn);      // initialize a match with board paths and marbles
    void updateGUI (string positionsFile);              // update positionsFile for the GUI
    void setBoard(Tree*);                               // replace all the board with a Tree containing board state
    
    Tree *minimax;                                      // tree with minimax
    int eval (int player);                              // evaluation function used in minimax
    void computePossibilities(int player);              // update accessible nodes for all marbles of a player
    bool pat();                                         // true if a player cannot play



public:

    Node *nodes[190];                                   // all the nodes in an array
    Path *paths[28];                                    // all paths in an array
    Marble *marbles[2][13];                             // all marbles in an array, in an array of 2 players
    
	int whosTurn;								// 0: computer / 1: human
    void chooseRespawn(int player, Node* dst);

    void nextTurn(int player0, int player1);
	
    Game();
    ~Game();

    Tree* runMinimax(Tree *currentNode, int depth, bool maximizingPlayer, int whoIsMaximizing);
    Tree* runMinimaxAlphaBeta(Tree *currentNode, int depth, int alpha, int beta, bool maximizingPlayer, int whoIsMaximizing);
    void randomMove(Marble *src, Node *dst, int player);
    int letsplay(int player0, int player1);
    void playerDoAMove(int player);

    /* Tests */
    void generateGames(int nbGames, int nbDead, int nbBorder);
};

#endif
