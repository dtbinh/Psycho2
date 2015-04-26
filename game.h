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

	void initBoard (string pathsFile);
    void initMatch (string openingFile, int turn);
    void updateGUI (string positionsFile);
    void setBoard(Tree*);
    
    Tree *minimax;
    int eval (int player);						// evaluation function used in minimax




public:

    Node *nodes[190];
    Path *paths[28];
    Marble *marbles[2][13];
    
	int whosTurn;								// 0: computer / 1: human
    void chooseRespawn(int player, Node* dst);

    bool nextTurn();
	
    Game();
    ~Game();

    void runMinimax();
    int runMinimaxAlphaBeta(Tree *currentNode, int depth, int alpha, int beta, bool maximizingPlayer);

    /* Tests */
    void generateGames(int nbGames, int nbDead, int nbBorder);
};

#endif
