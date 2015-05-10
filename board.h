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

    Game *game;                         // pointer to Game
    int index;                          // number of the node
    bool isSpecial;                     // True if special node
    bool isExtraNode;                   // nodes for dead Marbles
    Marble *marble;                     // Marble positioned on this Node, NULL if none
    Path *paths[3];                     // the Paths that intersect at this Node (there are 2 or 3 paths)
    vector<Node*> dirPaths[6];			// list of directional paths to explore (there are 3, 4 or 6 such paths)

    Node (Game *_game, int _index, Marble* _marble); // constructor
    
    void addPath (Path *p); // add a path to node
    void updateDirPath ();  // order node paths in several directions
    void display (bool full);   // Display informations in console

};


/* PATH */

struct Path{

    Game *game;                         // pointer to Game
    int index;                          // number of the path
    bool isBorder;                      // true
    Node **nodes;						// the list of nodes in the path (possible sizes are 9, 11 and 48)
    vector<Marble*> marbles;			// the list of marbles on the path
    int size;                           // Number of nodes

    Path (Game *_game, int _index, vector<int> idxList); // constructor
    
    void updateMarbles (); // update marbles on path
    void display ();    // display informations on console

    bool wellOrdered (Node *n1, Node *n2, Node *n3); // n1 n2 n3 alined = true

private:
    int countSpecialNodesInBetween (Node *n1, Node *n2, Node *n3); // count how many special nodes are between n1 n2 n3
};


/* MARBLE */

struct Marble {

    Game *game;                     // Pointer to game
    MarbleType type;                // type of marble
    int owner;                      // player that owns marble
    Node *node;                     // node where is the marble
    vector<Node*> accessibleNodes;  // nodes where can go the marble

    Marble (Game *_game, Node *_node, int _type, int _player); // constructor
    
    bool isCaptured();              // true if captured
    bool isAlive();                 // true if alive
    bool isOnBorder();              // true if on border
    void updateAccessibleNodes ();  // update nodes where can go the marble
    void move (Node *dst);          // move the marble to the node dst
    void kill ();                   // kill and move marble to dead nodes
    void display (bool full, bool cr);//display informations on console

    string type2str (MarbleType type); // return type in string for the console
    int type2int (MarbleType type);    // convert constant type to an int
    MarbleType int2type (int type);    // convert int to constant type

};


#endif
