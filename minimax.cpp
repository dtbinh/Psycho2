#include "minimax.h"

/**
 * @brief Tree::Tree Constructor
 * @param _game reference to game object
 * @param _father reference to father
 */
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

/**
 * @brief Tree::addNewSon add a son to tree
 * @param son reference to son to add
 */
void Tree::addNewSon (Tree *son) {
    sons.push_back(son);
}

/**
 * @brief Tree::displayConsole  display informations on console
 */
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

/**
 * @brief Tree::~Tree Destructor
 */
Tree::~Tree(){
    for(int i = 0 ; i < sons.size() ; i++){
        delete sons[i];
    }
}
