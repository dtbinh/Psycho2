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
    for(int i = 0; i < 2; i++){
        for(int j = 0; j < NBMARBLES; j++){
            cout << marblesPositions[i][j] << endl;
        }
        cout << "_____________" << endl;
    }

    //cout << "depth : " << depth << " score : " << score << "             \r";
}

Tree::~Tree(){
    for(int i = 0 ; i < sons.size() ; i++){
        delete sons[i];
    }
}
