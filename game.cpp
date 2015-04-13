#include "game.h"
#include <ctime>

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

void Game::setBoard(Tree * t){
    for(int i = 0; i < NBNODES; i++) this->nodes[i]->marble = NULL;
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
    for(int i = 0; i < 2; i++)
        for(int j = 0; j < NBMARBLES; j++)
            if(!marbles[i][j]->isAlive()) evaluation = (i == player) ? evaluation-1 : evaluation+1;

    return evaluation;
}


void Game::runMinimax () {
    vector<Tree*> list;
    Tree* start = new Tree(this, NULL);
    start->score = this->eval(0);
    list.push_back(start);
    int listCpt = 0;

    Tree* bestTree = start;
    int bestScore = INT_MIN;
    time_t beginning;
    beginning = time(0);

    while(!list.empty() && time(0) < beginning + TIMEOUT){
        Tree* currentTree = list[listCpt];
        this->setBoard(currentTree);
        for(int j = 0; j < NBMARBLES; j++){
            Marble* m = marbles[currentTree->depth % 2][j];
            m->updateAccessibleNodes();
            for(int k = 0; k < m->accessibleNodes.size(); k++){
                m->move(m->accessibleNodes[k]);
                Tree* son = new Tree(this, currentTree);
                son->score = this->eval(0);
                if(son->score > bestScore){
                    bestScore = son->score;
                    bestTree = son;
                }
                list.push_back(son);
                currentTree->addNewSon(son);
                son->displayConsole();
                this->setBoard(currentTree);
            }
        }
        listCpt++;
    }
    cout << "Meilleur score : " << bestScore << endl;
}

/* TESTS */

void Game::generateGames(int nbGames, bool alive){
    initBoard (PATHSFILE);    // creating nodes and paths
    bool alreadyMarble;
    int type;
    int rdm;
    double average = 0.0;
    srand (time(NULL)); //Setting the seed for random number generator

    // Placing randomly marbles
    for(int n = 0 ; n < nbGames ; n++){
        //cout << "Generation game " << n << "                  \r";

        // Empty the board
        for(int nId = 0 ; nId < BOARDSIZE ; nId++) nodes[nId]->marble = NULL;

        // create marbles
        for(int p = 0 ; p < 2 ; p++){
            for(int i = 0 ; i < NBMARBLES ; i++){
                alreadyMarble = true;
                while(alreadyMarble){
                    if(!alive){
                        rdm = rand() % BOARDSIZE;
                    }
                    else{
                        rdm = rand() % NBNODES;
                    }
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
                if(marbles[p][i]->isCaptured())marbles[p][i]->kill();
            }
        }

        int totalpossibilities = 0;
        // Compute possibilities
        for(int p = 0 ; p < 2 ; p++){
            for(int i = 0 ; i < NBMARBLES ; i++){
                marbles[p][i]->updateAccessibleNodes();
                totalpossibilities+=marbles[p][i]->accessibleNodes.size();
            }
        }
        average += totalpossibilities / 2;
    }

    average /= nbGames;
    cout << endl << endl << "Moyenne : " << average << endl;
}


