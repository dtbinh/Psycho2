
#include "game.h"
#include <ctime>
#include <cmath>
#include <cstdlib>
#include <unistd.h>

extern const int NBNODES (163);
extern const int NBPATHS (28);
extern const int NBMARBLES (13);
extern const int BOARDSIZE (190);

extern const string DATAPATH = "./data/";
extern const string PATHSFILE = DATAPATH + "paths.txt";
extern const string OPENINGFILE = DATAPATH + "opening.txt";
extern const string CURPOSFILE = DATAPATH + "positions.txt";

extern const bool VERBOSE (true);
extern const bool GUI (false);
extern const int TIMEOUT (60);

extern const int ALPHABETA (01);
extern const int HUMAN (02);
extern const int RANDOM (03);
extern const int BETTERRDM (04);


/**
 * @brief Game::Game Constructor
 */
Game::Game()
{
    initBoard (PATHSFILE);			// creating nodes and paths
    initMatch (OPENINGFILE, 0);		// creating marbles

    // TESTS (uncomment to check)

    //for (int i=0; i<NBPATHS; i++) paths[i]->display ();
    //for (int i=0; i<BOARDSIZE; i++) nodes[i]->display (false);
    //for (int i=0; i<BOARDSIZE; i++) nodes[i]->display (true);
    //for (int j=0; j<2; j++) for (int i=0; i<NBMARBLES; i++) marbles[j][i]->display (false, true);
    //updateGUI (CURPOSFILE); marbles[0][7]->kill(); marbles[1][3]->kill();
    //for (int j=0; j<2; j++) for (int i=0; i<NBMARBLES; i++) marbles[j][i]->display (false, true);
    //updateGUI (CURPOSFILE);
    //for (int j=0; j<2; j++) for (int i=0; i<NBMARBLES; i++) { marbles[j][i]->updateAccessibleNodes (); marbles[j][i]->display (true, true);}
}


/**
 * @brief Game::initBoard initialize the board with nodes and paths
 * @param pathsFile file path to data about the paths
 */
void Game::initBoard (string pathsFile) {
    // create nodes (no marble, just empty board)
    for (int i=0; i<BOARDSIZE; i++) nodes[i] = new Node (this, i, NULL);

    // create paths
    ifstream file;
    file.open (pathsFile.c_str(), ios::in);
    vector<int> nodeIdxList;
    int currentPathidx = 0;
    int tmpIdx;

    if (file) {
        string line;
        while (getline(file, line)) {
            istringstream iss(line);
            nodeIdxList.clear ();
            while (iss >> tmpIdx) nodeIdxList.push_back(tmpIdx);
            if (currentPathidx < NBPATHS) {
                paths[currentPathidx] = new Path (this, currentPathidx, nodeIdxList);
                currentPathidx++;
            }
        }
        file.close();
    }
    
    else exitError ("couldn't open file " + pathsFile);
    
    // update directional paths in all nodes
    for (int i=0; i<BOARDSIZE; i++) nodes[i]->updateDirPath();
}

/**
 * @brief Game::initMatch initialize a match with board paths and marbles
 * @param openingFile file with the marbles
 * @param turn who's turn to play
 */
void Game::initMatch (string openingFile, int turn) {
    // create marbles
    ifstream file;
    file.open (openingFile.c_str(), ios::in);
    int player, nodeIdx, type;
    int currentMarbleIdx[2] = {0, 0};

    if (file) {
        while (file >> player >> nodeIdx >> type)
            if (currentMarbleIdx[player] < NBMARBLES)
                marbles[player][currentMarbleIdx[player]++] = new Marble (this, nodes[nodeIdx], type, player);
        file.close();
    }
    
    else exitError ("couldn't open file " + openingFile);
    
    for(int i = 0; i < NBPATHS; i++){
        paths[i]->updateMarbles();
    }
    // who's turn to play
    whosTurn = turn;
}

/**
 * @brief Game::updateGUI update positionsFile for the GUI
 * @param positionsFile file path to positions File with current positions for the marbles
 */
void Game::updateGUI (string positionsFile) {

    ofstream file;
    file.open(positionsFile.c_str(), ios::out);

    if (file.is_open()) {
        for (int i=0; i<2; i++)
            for (int j=0; j<NBMARBLES; j++)
                file << i << " " << marbles[i][j]->node->index << " " << marbles[i][j]->type2int(marbles[i][j]->type) << endl;
        file.close();
    }
    
    else warningMsg ("couldn't open file " + positionsFile);
}

/**
 * @brief Game::chooseRespawn (not used currently) player can choose a dead marble to revive. You can only revive a doctor or a delater
 * @param player player that can revive a marble
 * @param dst node to put the marble to
 */
void Game::chooseRespawn(int player, Node* dst){
    if(player==0){
        float nbINFMort = 0;
        float nbDELMort = 0;
        Marble* anINF = NULL;
        Marble* aDEL = NULL;
        for(int i = 0; i < NBMARBLES; i++){
            if(!marbles[player][i]->isAlive()){
                switch(marbles[player][i]->type){
                case INF:
                    nbINFMort++;
                    anINF = marbles[player][i];
                    break;
                case DEL:
                    nbDELMort++;
                    aDEL = marbles[player][i];
                    break;
                default:
                    break;
                }
            }
        }
        if(nbINFMort/6 > nbDELMort/4){
            anINF->move(dst);
        }else if(aDEL != NULL){
            aDEL->move(dst);
        }
    }else{
        // TODO player's choice
    }
}

/**
 * @brief Game::setBoard replace all the board with a Tree containing board state
 * @param t Tree containing the board state
 */
void Game::setBoard(Tree * t){
    for(int i = 0; i < BOARDSIZE; i++) this->nodes[i]->marble = NULL;
    for(int i = 0; i < 2; i++){
        for(int j = 0; j < NBMARBLES; j++){
            this->nodes[t->marblesPositions[i][j]]->marble = this->marbles[i][j];
            this->marbles[i][j]->node = this->nodes[t->marblesPositions[i][j]];
        }
    }
    for(int i = 0; i < NBPATHS; i++) this->paths[i]->updateMarbles();
}

/**
 * @brief Game::eval Evaluation function for minimax / alphabeta algorithms. Check the board and compute a value positive or negative for the player
 * @param player the player concerned by the eval function.
 * @return a positive value if this board state is a good one for the player, negative if not.
 */
int Game::eval (int player) {
    int evaluation = 0;
    for(int i = 0; i < 2; i++){
        int possibleMoves = 0;
        int nbDocOnBorder = 0;
        int valuePsycho = 1000000;
        bool pkpAlive = true;
        for(int j = 0; j < NBMARBLES; j++){
            int value = (marbles[i][j]->type) == INF ? 20 : (marbles[i][j]->type == DEL) ? 30 : (marbles[i][j]->type == PSY) ? 50 : 0;
            // Kill
            if(!marbles[i][j]->isAlive()){
                if(marbles[i][j]->type == PKP) pkpAlive = false;
                evaluation = (i == player) ? evaluation-value : evaluation+value;
            }
            // Border
            else if (marbles[i][j]->isOnBorder() && marbles[i][j]->type == INF){ // handle border
                nbDocOnBorder++;
                if(nbDocOnBorder > 3){
                    evaluation = (i == player) ? evaluation - 2 : evaluation + 2;
                }
                else{
                    evaluation = (i == player) ? evaluation + 2 : evaluation - 2;
                }
            }
            possibleMoves += marbles[i][j]->accessibleNodes.size();
        }
        // standard deviation to include movement possibilites number
        int sd = 15;
        int mean = 190;
        int value = (int)floor((possibleMoves - mean) / sd);
        evaluation = (i == player) ? evaluation + value : evaluation - value;
        // pkp
        if(!pkpAlive){
            evaluation = (i==player) ? INT_MIN : INT_MAX;
        }
    }
    return evaluation;
}

/**
 * @brief Game::runMinimax Run minimax algorithm and returns the best Tree. See Wikipedia "minimax" for the algorithm details
 * @param currentNode Tree to expand
 * @param depth current depth. Recursively decrease to 0 to stop recursivity.
 * @param maximizingPlayer true if we are maximizin
 * @return the tree with the best value returned by eval()
 */
Tree* Game::runMinimax (Tree* currentNode, int depth, bool maximizingPlayer) {
    if(currentNode == NULL){
        currentNode = new Tree(this, NULL);
    }
    if(depth == 0){
        currentNode->score = this->eval(0);
        return currentNode;
    }
    if(maximizingPlayer){
        currentNode->score = INT_MIN;
        Tree* keptSon;
        this->setBoard(currentNode);
        // Sons of currentNode
        for(int j = 0; j < NBMARBLES; j++){
            if(j == 1 && depth == 3){
                cout << "pb" << endl;
            }
            Marble* m = marbles[0][j];
            m->updateAccessibleNodes();
            for(int k = 0; k < m->accessibleNodes.size(); k++){
                m->move(m->accessibleNodes[k]);
                Tree* son = new Tree(this, currentNode);
                currentNode->addNewSon(son);
                runMinimax(son, depth-1, false);
                if(currentNode->score < son->score){
                    currentNode->score = son->score;
                    keptSon = son;
                }
                this->setBoard(currentNode);
                m->updateAccessibleNodes();
            }
        }

        return keptSon;
    }else{
        currentNode->score = INT_MAX;
        Tree* keptSon;
        this->setBoard(currentNode);
        for(int j = 0; j < NBMARBLES; j++){
            Marble* m = marbles[1][j];
            m->updateAccessibleNodes();
            for(int k = 0; k < m->accessibleNodes.size(); k++){
                m->move(m->accessibleNodes[k]);
                Tree* son = new Tree(this, currentNode);
                currentNode->addNewSon(son);
                runMinimax(son, depth-1, true);
                if(currentNode->score > son->score){
                    currentNode->score = son->score;
                    keptSon = son;
                }
                this->setBoard(currentNode);
                m->updateAccessibleNodes();
            }
        }
        return keptSon;
    }
}

/**
 * @brief Game::runMinimaxAlphaBeta run alpha beta algorithm and returns the best Tree. See Wikipedia "alpha beta" for the algorithm details.
 * @param currentNode Tree to expand
 * @param depth current depth. Recursively decrease to 0 to stop recursivity.
 * @param alpha représente le meilleur noeud MAX
 * @param beta représente le meilleur noeud MIN
 * @param maximizingPlayer true if we are maximizing
 * @param whoIsMaximizing the player maximizing
 * @return the tree with the best value returned by eval()
 */
Tree* Game::runMinimaxAlphaBeta (Tree* currentNode, int depth, int alpha, int beta, bool maximizingPlayer, int whoIsMaximizing) {
    if(currentNode == NULL){
        currentNode = new Tree(this, NULL);
    }
    if(depth == 0){
        currentNode->score = this->eval(whoIsMaximizing);
        return currentNode;
    }
    if(maximizingPlayer){
        currentNode->score = INT_MIN;
        Tree* keptSon;
        this->setBoard(currentNode);
        for(int j = 0; j < NBMARBLES; j++){
            Marble* m = marbles[whoIsMaximizing][j];
            m->updateAccessibleNodes();
            for(int k = 0; k < m->accessibleNodes.size(); k++){
                m->move(m->accessibleNodes[k]);
                Tree* son = new Tree(this, currentNode);
                currentNode->addNewSon(son);
                runMinimaxAlphaBeta(son, depth-1, alpha, beta, false, whoIsMaximizing);
                if(currentNode->score < son->score){
                    currentNode->score = son->score;
                    keptSon = son;
                }
                alpha = max(alpha, currentNode->score);
                if(beta <= alpha){
                    return keptSon;
                }
                this->setBoard(currentNode);
                m->updateAccessibleNodes();
            }
        }
        return keptSon;
    }else{
        currentNode->score = INT_MAX;
        Tree* keptSon;
        this->setBoard(currentNode);
        for(int j = 0; j < NBMARBLES; j++){
            Marble* m = marbles[(whoIsMaximizing+1)%2][j];
            m->updateAccessibleNodes();
            for(int k = 0; k < m->accessibleNodes.size(); k++){
                m->move(m->accessibleNodes[k]);
                Tree* son = new Tree(this, currentNode);
                currentNode->addNewSon(son);
                runMinimaxAlphaBeta(son, depth-1, alpha, beta, true, whoIsMaximizing);
                if(currentNode->score > son->score){
                    currentNode->score = son->score;
                    keptSon = son;
                }
                beta = min(beta, currentNode->score);
                if(beta <= alpha){
                    return keptSon;
                }
                this->setBoard(currentNode);
                m->updateAccessibleNodes();
            }
        }
        return keptSon;
    }
}

/**
 * @brief Game::pat This function check if a player cannot play anymore (marbles cannot move)
 * @return true if a player cannot play
 */
bool Game::pat(){
    for(int i = 0 ; i < 2 ; i++){
        int possMoves = 0;
        for(int j = 0 ; j < NBMARBLES ; j++){
            marbles[i][j]->updateAccessibleNodes();
            possMoves += marbles[i][j]->accessibleNodes.size();
        }
        if(possMoves == 0){
            //cout << "pat" << endl;
            return true;
        }
    }
    return false;
}

/**
 * @brief Game::letsplay method to do a match between two players. Parameters are constants to use between HUMAN, RANDOM, and ALPHABETA
 * @param player0 constant to use between HUMAN, RANDOM, and ALPHABETA
 * @param player1 constant to use between HUMAN, RANDOM, and ALPHABETA
 * @return
 */
int Game::letsplay(int player0, int player1){
    Marble * psycho0;
    Marble * psycho1;
    for(int i = 0 ; i < 2 ; i++){
        for(int j = 0 ; j < NBMARBLES ; j++){
            if(marbles[i][j]->type == PKP){
                if(i)
                    psycho1 = marbles[i][j];
                else
                    psycho0 = marbles[i][j];
            }
        }
    }
    updateGUI(CURPOSFILE);
    int coups=0;
    srand(time(NULL));

    while(psycho1->isAlive() && psycho0->isAlive() && !pat()){
        coups++;
        nextTurn(player0, player1);
        updateGUI(CURPOSFILE);
    }
    if(psycho0->isAlive()){
        return coups;
    }
    else return -coups;
}

/**
 * @brief Game::computePossibilities update accessible nodes for all marbles of a player
 * @param player we update marble for only one player
 */
void Game::computePossibilities(int player){
    for(int i = 0 ; i < NBMARBLES ; i++){
        marbles[player][i]->updateAccessibleNodes();
    }
}

/**
 * @brief Game::playerDoAMove switch case between HUMAIN, RANDOM, and ALPHABETA on player, and do a move.
 * @param player player that plays
 */
void Game::playerDoAMove(int player){
    switch(player){
    case HUMAN:
    {
        ifstream file;
        file.open (CURPOSFILE.c_str(), ios::in);

        int player[NBMARBLES*2];
        int idNode[NBMARBLES*2];
        int type[NBMARBLES*2];

        int currentP;
        int currentI;
        int currentT;

        int cpt=0;
        while (file >> player[cpt] >> idNode[cpt] >>type[cpt])cpt++;

        file.close();

        bool guiChanged = false;
        while(!guiChanged){
            file.open (CURPOSFILE.c_str(), ios::in);
            cpt=0;
            while(file >> currentP >> currentI >> currentT){
              if((player[cpt] != currentP || idNode[cpt] != currentI || type[cpt] != currentT) && currentI != 183){
                  guiChanged = true;
                  cout << "Move from " << idNode[cpt] << " to " << currentI << endl;
                  marbles[whosTurn][cpt]->move(nodes[currentI]);
              }
              cpt++;
            }
            file.close();
            sleep(1); // We wait a bit before refreshing
        }
        break;
    }
    case RANDOM:
    {
        computePossibilities(1);
        int marble = rand() % NBMARBLES;
        while(marbles[whosTurn][marble]->accessibleNodes.empty()) marble = (marble + 1) % NBMARBLES;
        int move = rand() % marbles[whosTurn][marble]->accessibleNodes.size();
        marbles[whosTurn][marble]->move(marbles[whosTurn][marble]->accessibleNodes[move]);
        cout << "Random plays" << endl;
        break;
    }
    case ALPHABETA:
    {
        Tree* bestMove = this->runMinimaxAlphaBeta(NULL, 4, INT_MIN, INT_MAX, true, whosTurn);
        this->setBoard(bestMove);
        delete bestMove->father;
        cout << "Alpha beta plays" << endl;
        break;
    }
    case BETTERRDM: //not implemented yet
        break;
    }


}

/**
 * @brief Game::nextTurn Change the player to play and make him plays
 * @param player0 white
 * @param player1 black
 */
void Game::nextTurn(int player0, int player1){
    if(whosTurn == 0){
        playerDoAMove(player0);
    }else{
        playerDoAMove(player1);
    }
    whosTurn = (whosTurn+1) % 2;
}

/* TESTS */

/**
 * @brief Game::generateGames statistics tests, average and standard deviation. Displays statistics in console
 * @param nbGames amount of games to generate. More games -> more accurate statistics
 * @param nbDead Number of marbles to kill
 * @param nbBorder Number of marbles to place on border
 */
void Game::generateGames(int nbGames, int nbDead, int nbBorder){
    bool alreadyMarble;
    int type;
    int rdm;
    int currDead, currBorder;
    currDead = currBorder = 0;
    double average = 0.0;
    double variance = 0.0;
    double possibilities[nbGames];

    initBoard (PATHSFILE);    // creating nodes and paths
    srand (time(NULL)); //Setting the seed for random number generator



    // Placing randomly marbles
    for(int n = 0 ; n < nbGames ; n++){
        //cout << "Generation game " << n << "                  \r";
        currDead = currBorder = 0;
        // Empty the board
        for(int nId = 0 ; nId < BOARDSIZE ; nId++) nodes[nId]->marble = NULL;

        // create marbles
        for(int p = 0 ; p < 2 ; p++){
            for(int i = 0 ; i < NBMARBLES ; i++){
                alreadyMarble = true;
                while(alreadyMarble){
                    rdm = rand() % NBNODES;
                    alreadyMarble = (nodes[rdm]->marble != NULL);
                    //if(alreadyMarble) cout << "                          Aie " << n << "    " << rdm << "          \r";
                }
                if(i < 6) type = INF;
                else if(i < 10) type = DEL;
                else if(i < 12) type = PSY;
                else type = PKP;

                marbles[p][i] = new Marble (this, nodes[rdm], type, 0);
            }
        }
        //Update marbles on path
        for(int i = 0; i < NBPATHS; i++){
            paths[i]->updateMarbles();
        }

        // Check for kills
        for(int p = 0 ; p < 2 ; p++){
            for(int i = 0 ; i < NBMARBLES ; i++){
                if(marbles[p][i]->isCaptured()){
                    marbles[p][i]->kill();
                    currDead++;
                }
            }
        }

        // Kill some marbles
        for(int p = 0 ; p < 2 ; p++){
            for(int i = 0 ; i < NBMARBLES ; i++){
                if(marbles[p][i]->isAlive()){
                    if(currDead < nbDead){
                        marbles[p][i]->kill();
                        currDead++;
                    }
                }
            }
        }

        // Move some marbles to border
        for(int p = 0 ; p < 2 ; p++){
            for(int i = 0 ; i < NBMARBLES ; i++){
                for(int path = 0 ; path < 3 ; path++){
                    alreadyMarble = true;
                    while(alreadyMarble){
                        rdm = rand() % 48;
                        alreadyMarble = (paths[0]->nodes[rdm]->marble != NULL);

                        if(marbles[p][i]->node->paths[path] != NULL){
                            if(marbles[p][i]->node->paths[path]->isBorder){
                                currBorder++;
                            }
                            else{
                                if(currBorder < nbBorder && !alreadyMarble){
                                    marbles[p][i]->move(paths[0]->nodes[rdm]);
                                    currBorder++;
                                }
                            }
                        }
                    }
                }
            }
        }

        // Check for kills
        for(int p = 0 ; p < 2 ; p++){
            for(int i = 0 ; i < NBMARBLES ; i++){
                if(marbles[p][i]->isCaptured()){
                    marbles[p][i]->kill();
                    currDead++;
                }
            }
        }

        //Update marbles on path
        for(int i = 0; i < NBPATHS; i++){
            paths[i]->updateMarbles();
        }

        double totalpossibilities = 0.0;
        // Compute possibilities
        for(int p = 0 ; p < 2 ; p++){
            for(int i = 0 ; i < NBMARBLES ; i++){
                marbles[p][i]->updateAccessibleNodes();
                totalpossibilities+=marbles[p][i]->accessibleNodes.size();
            }
        }
        possibilities[n] = totalpossibilities / 2.0;
        average += totalpossibilities / 2.0;
    }

    average /= (double)nbGames;
    cout << endl << endl << "Average : " << average << endl;

    // calcul ecart type
    for(int n = 0 ; n < nbGames ; n++){
        variance += pow(possibilities[n] - average, 2.0);
    }
    variance = 1.0/(double)nbGames * variance;

    cout << endl << "Standard deviation : " << sqrt(variance) << endl;

}

/**
 * @brief Game::~Game Destructor
 */
Game::~Game(){
    delete nodes;
    delete paths;
    delete marbles;
}

