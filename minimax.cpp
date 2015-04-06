#include "minimax.h"


Tree::Tree (Game *_game, Tree *_father){
	game = _game;
    father = _father;
	if (father == NULL) depth = 0;
	else depth = father->depth+1;
    score = -1;
    for (int i=0; i<2; i++)
		for (int j=0; j<NBMARBLES; j++)
	        marblesPositions[i][j] = game->marbles[i][j]->node->index;
}


void Tree::addNewSon (Tree *son) {
    sons.push_back(son);
}


void Tree::displayConsole () {

	// !! not implemented !! //
	
    cout << "depth : " << depth << " score : " << score << "             \r";
}
