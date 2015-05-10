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
    
    int whosTurn;                                       // 0: computer / 1: human
    void chooseRespawn(int player, Node* dst);          // (not used currently) player can choose a dead marble to revive

    void nextTurn(int player0, int player1);            // Change the player to play and make him plays
	
    Game();
    ~Game();

    Tree* runMinimax(Tree *currentNode, int depth, bool maximizingPlayer, int whoIsMaximizing); // Run minimax algorithm and returns the best Tree
    Tree* runMinimaxAlphaBeta(Tree *currentNode, int depth, int alpha, int beta, bool maximizingPlayer, int whoIsMaximizing); // run alpha beta algorithm and returns the best Tree
    int letsplay(int player0, int player1); // method to do a match between two players. Parameters are constants to use between HUMAN, RANDOM, and ALPHABETA
    void playerDoAMove(int player); // switch case between HUMAIN, RANDOM, and ALPHABETA on player, and do a move.

    /* Tests */
    void generateGames(int nbGames, int nbDead, int nbBorder); // statistics tests, average and standard deviation
};

#endif
