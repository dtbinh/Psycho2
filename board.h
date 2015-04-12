#ifndef BOARD_H
#define BOARD_H


#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <vector>
#include <ctime>

#include "const.h"
#include "game.h"

class Game;

struct Path;
struct Marble;


/* NODE */

struct Node {

	Game *game;
    int index;
    bool isSpecial;
    bool isExtraNode;                   // nodes for dead Marbles
    Marble *marble;                     // Marble positioned on this Node, NULL if none
    Path *paths[3];                     // the Paths that intersect at this Node (there are 2 or 3 paths)
    vector<Node*> dirPaths[6];			// list of directional paths to explore (there are 3, 4 or 6 such paths)

    Node (Game *_game, int _index, Marble* _marble);
    
	void addPath (Path *p);
	void updateDirPath ();
	void display (bool full);

};


/* PATH */

struct Path{

	Game *game;
    int index;
    bool isBorder;
    Node **nodes;						// the list of nodes in the path (possible sizes are 9, 11 and 48)
    vector<Marble*> marbles;			// the list of marbles on the path
    int size;

    Path (Game *_game, int _index, vector<int> idxList);
    
    void updateMarbles ();
	void display ();

    bool wellOrdered (Node *n1, Node *n2, Node *n3);

private:
    int countSpecialNodesInBetween (Node *n1, Node *n2, Node *n3);
};


/* MARBLE */

struct Marble {

	Game *game;
	MarbleType type;
	int owner;
	Node *node;
	vector<Node*> accessibleNodes;

    Marble (Game *_game, Node *_node, int _type, int _player);
    
    bool isCaptured();
    bool isAlive();
    void updateAccessibleNodes ();
    void move (Node *dst);
    void kill ();
	void display (bool full, bool cr);

	string type2str (MarbleType type);
	int type2int (MarbleType type);
	MarbleType int2type (int type);

};


#endif
