#ifndef MINIMAX_H
#define MINIMAX_H

#include "game.h"
#include "const.h"

using namespace std;


class Game;

class Tree
{

private:

    vector<Tree*> sons;                 // reference to sons
    
	Game *game;							// reference to Game is needed to access the board and marbles
    
    int bestScore;                      //  not used
    Tree *bestSon;                      // not used
        
public:
    Tree *father;                       // father. NULL if root

    int score;                          // score returned by eval()
    int depth;                          // current depth
    int marblesPositions[2][13];		// saving in each node the current positions of the marbles

    void addNewSon (Tree *son);         // add a son to tree

    Tree (Game *_game, Tree *_father);  // constructor
    ~Tree();

    void displayConsole ();             // display informations on console

};

#endif
