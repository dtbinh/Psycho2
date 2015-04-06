#ifndef MINIMAX_H
#define MINIMAX_H

#include "game.h"
#include "const.h"

using namespace std;


class Game;

class Tree
{

private:

    Tree *father;
    vector<Tree*> sons;
    
	Game *game;							// reference to Game is needed to access the board and marbles
    
    int bestScore;
    Tree *bestSon;
        
public:
    
    int score;
    int depth;
    int marblesPositions[2][13];		// saving in each node the current positions of the marbles

    void addNewSon (Tree *son);

    Tree (Game *_game, Tree *_father);
    ~Tree();

	void displayConsole ();

};

#endif
