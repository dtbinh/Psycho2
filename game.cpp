
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
            if(j == 0){
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
            }
        }
        return keptSon;
    }
}

Tree* Game::runMinimaxAlphaBeta (Tree* currentNode, int depth, int alpha, int beta, bool maximizingPlayer) {
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
        for(int j = 0; j < NBMARBLES; j++){
            Marble* m = marbles[0][j];
            m->updateAccessibleNodes();
            for(int k = 0; k < m->accessibleNodes.size(); k++){
                m->move(m->accessibleNodes[k]);
                Tree* son = new Tree(this, currentNode);
                currentNode->addNewSon(son);
                runMinimaxAlphaBeta(son, depth-1, alpha, beta, false);
                if(currentNode->score < son->score){
                    currentNode->score = son->score;
                    keptSon = son;
                }
                alpha = max(alpha, currentNode->score);
                if(beta <= alpha){
                    return keptSon;
                }
                this->setBoard(currentNode);
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
                runMinimaxAlphaBeta(son, depth-1, alpha, beta, true);
                if(currentNode->score > son->score){
                    currentNode->score = son->score;
                    keptSon = son;
                }
                beta = min(beta, currentNode->score);
                if(beta <= alpha){
                    return keptSon;
                }
                this->setBoard(currentNode);
            }
        }
        return keptSon;
    }
}


/**
 * @brief Game::randomMove affect randomly a source marble and a random valid destination node.
 * @param src source marble
 * @param dst destination node
 * @param player player concerned
 */
void Game::randomMove(Marble *src, Node *dst, int player){
    int rdm;

    srand(time(NULL));
    rdm = rand() % NBMARBLES;

    src = marbles[player][rdm];

    vector<Node *> v = src->accessibleNodes;
    rdm = rand() % v.size();
    dst = v.at(rdm);
}

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

int Game::letTheBotFightBegin(){
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
        //cout << whosTurn << " joue. (" << coups << ")" << endl;
        coups++;
        sleep(1);
        nextTurn();
        updateGUI(CURPOSFILE);
    }
    return coups;
}

void Game::computePossibilities(int player){
    for(int i = 0 ; i < NBMARBLES ; i++){
        marbles[player][i]->updateAccessibleNodes();
    }
}

void Game::nextTurn(){
    if(whosTurn == 0){
        Tree* bestMove = this->runMinimax(NULL, 3, true);
        bestMove->displayConsole();
        sleep(1);
        this->setBoard(bestMove);
    }else{
        computePossibilities(1);
        int marble = rand() % NBMARBLES;
        while(marbles[whosTurn][marble]->accessibleNodes.empty()) marble = (marble + 1) % NBMARBLES;
        int move = rand() % marbles[whosTurn][marble]->accessibleNodes.size();
        marbles[whosTurn][marble]->move(marbles[whosTurn][marble]->accessibleNodes[move]);
    }
    whosTurn = (whosTurn+1) % 2;
}

/* TESTS */

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
    cout << endl << endl << "Moyenne : " << average << endl;

    // calcul ecart type
    for(int n = 0 ; n < nbGames ; n++){
        variance += pow(possibilities[n] - average, 2.0);
    }
    variance = 1.0/(double)nbGames * variance;

    cout << endl << "Ecart type : " << sqrt(variance) << endl;

}

Game::~Game(){
    //delete nodes;
    //delete paths;
    //delete marbles;
    free(minimax);
}

