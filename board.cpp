#include "board.h"


/* NODE */

/**
 * @brief Node::Node Constructor
 * @param _game the game to attach to
 * @param _index    id of node
 * @param _marble   marble on node. NULL if none
 */
Node::Node (Game *_game, int _index, Marble* _marble) {
    game = _game;
    index = _index;
    isSpecial = false;
    isExtraNode = (index >= 163);
    marble = _marble;
    for(int i=0; i<3; i++) paths[i] = NULL;			// NULL for now, will be updated when Path are created
    for(int i=0; i<6; i++) dirPaths[i].clear();		// empty for now, will be updated after Path are created
}

/**
 * @brief Node::addPath add a reference to a path for node
 * @param p path to add
 */
void Node::addPath (Path *p) {
    int i;
    for(i=0; (i<3) && (paths[i]!=NULL) && (paths[i]!=p); i++);
    paths[i] = p;
    isSpecial = (paths[2] != NULL);
}
/**
 * @brief Node::updateDirPath order node paths in several directions
 */
void Node::updateDirPath () {
    int ii = 0;
    for(int i=0; (i<3) && (paths[i]!=NULL); i++) {
        int seekIdx = 0;
        while ((seekIdx<paths[i]->size) && (paths[i]->nodes[seekIdx]->index != index)) seekIdx++;
        if (paths[i]->isBorder) {					// border is a spacial case
            for (int j=0; j<paths[i]->size-1; j++) dirPaths[ii].push_back(paths[i]->nodes[(seekIdx+1+j)%(paths[i]->size)]);
            ii++;
            for (int j=0; j<paths[i]->size-1; j++) dirPaths[ii].push_back(paths[i]->nodes[(seekIdx-1-j+paths[i]->size)%(paths[i]->size)]);
            ii++;
        }
        else {
            int sizeFwd = paths[i]->size - seekIdx - 1;
            int sizeBwd = seekIdx;
            for (int j=0; j<sizeFwd; j++) dirPaths[ii].push_back(paths[i]->nodes[seekIdx+1+j]);
            if (sizeFwd > 0) ii++;
            for (int j=0; j<sizeBwd; j++) dirPaths[ii].push_back(paths[i]->nodes[seekIdx-1-j]);
            if (sizeBwd > 0) ii++;
        }
    }
}

/**
 * @brief Node::display Display informations in console
 * @param full true if all informations needed
 */
void Node::display (bool full) {
    cout << index << " -> " << (isSpecial?"x ":"  ") << (isExtraNode?"x ":"  ");
    for (int i=0; i<3; i++) if (paths[i]!=NULL) cout << paths[i]->index << " ";
    cout << "\t";
    if (marble != NULL) marble->display(false, false);
    cout << "\t";
    if (full) for (int i=0; i<6; i++)
        if (!dirPaths[i].empty()) {
            cout << endl << "\t[ ";
            for (int j=0; j<dirPaths[i].size(); j++) cout << dirPaths[i][j]->index << " ";
            cout << "]";
        }
    cout << endl;
}


/* PATH */


/**
 * @brief Path::Path
 * @param _game
 * @param _index
 * @param idxList
 */
Path::Path (Game *_game, int _index, vector<int> idxList) {
    game = _game;
    index = _index;
    size = idxList.size();
    isBorder = (index == 0);					// path of index 0 is the border
    nodes = new Node*[size];
    for(int i=0; i<size; i++) {
        nodes[i] = game->nodes[idxList[i]];		// create list of nodes
        nodes[i]->addPath(this);				// update related field in node
    }
    updateMarbles();							// update Marbles
}

/**
 * @brief Path::updateMarbles
 */
void Path::updateMarbles () {
    this->marbles.clear();
    for(int i = 0; i < size; i++){
        if(nodes[i]->marble != NULL){
            this->marbles.push_back(nodes[i]->marble);
        }
    }
}

/**
 * @brief Path::countSpecialNodesInBetween
 * @param n1
 * @param n2
 * @param n3
 * @return
 */
int Path::countSpecialNodesInBetween (Node *n1, Node *n2, Node *n3) {
    int nodeId = 0;
    int specialNodes = 0;
    bool rightWay = false;
    while(nodeId < this->size && this->nodes[nodeId] != n1 && this->nodes[nodeId] != n3)
        nodeId++;
    for(int i = (nodeId+1) % this->size; i != nodeId; i = (i+1)%this->size){
        if(this->nodes[i] == n2) rightWay = true;
        if(this->nodes[i] == n1 || this->nodes[i] == n3) break;
        if(this->nodes[i]->isSpecial) specialNodes++;
    }
    if(rightWay) return specialNodes;
    specialNodes = 0;
    for(int i = (nodeId==0) ? this->size-1 : nodeId-1; i != nodeId; i = (i==0) ? this->size - 1 : i-1){
        if(this->nodes[i] == n1 || this->nodes[i] == n3) break;
        if(this->nodes[i]->isSpecial) specialNodes++;
    }
    return specialNodes;
}

/**
 * @brief Path::wellOrdered
 * @param n1
 * @param n2
 * @param n3
 * @return
 */
bool Path::wellOrdered (Node *n1, Node *n2, Node *n3) {
    bool extFound = false;
    if(isBorder){
        return this->countSpecialNodesInBetween(n1,n2,n3) < 2;
    }else{
        for(int i = 0; i < this->size; i++){
            if(this->nodes[i] == n2 && !extFound) return false;
            if(this->nodes[i] == n2 && extFound) return true;
            if((this->nodes[i] == n1 || this->nodes[i] == n3) && extFound) return false;
            if(this->nodes[i] == n1 || this->nodes[i] == n3) extFound = true;
        }
    }
    return true;
}

/**
 * @brief Path::display
 */
void Path::display () {
    cout << index << " -> (" << size << ") ";
    for (int i=0; i<size; i++) cout << nodes[i]->index << " ";
    cout << endl;
}


/* MARBLE */

/**
 * @brief Marble::Marble
 * @param _game
 * @param _node
 * @param _type
 * @param _player
 */
Marble::Marble (Game *_game, Node *_node, int _type, int _player) {
    game = _game;
    node = _node;
    type = int2type(_type);
    owner = _player;
    node->marble = this;
}

/**
 * @brief Marble::display
 * @param full
 * @param cr
 */
void Marble::display (bool full, bool cr) {
    cout << owner << (isAlive()?" x ":"   ") << node->index << "\t" << type2str(type) << "  ";
    if (full) for (int j=0; j<accessibleNodes.size(); j++) cout << accessibleNodes[j]->index << " ";
    if (cr) cout << endl;
}

/**
 * @brief Marble::move
 * @param dst
 */
void Marble::move (Node *dst) {
    // PB
    Node* src = this->node;
    this->node->marble = NULL;
    this->node = dst;
    this->node->marble = this;
    for (int i=0; i<3; i++)
        if (node->paths[i]!= NULL)
            node->paths[i]->updateMarbles();
    for (int i=0; i<3; i++)
        if (src->paths[i] != NULL)
            src->paths[i]->updateMarbles();
    if(this->isCaptured()){
        kill();
    }
    else if(this->type==INF || this->type==DEL){
        for(int j = 0; j < NBMARBLES; j++){
            if(game->marbles[(owner+1)%2][j]->isCaptured())
                game->marbles[(owner+1)%2][j]->kill();
        }
    }
}

/**
 * @brief Marble::kill
 */
void Marble::kill () {
    Node* src = this->node;
    int firstIndexFree = NBNODES;
    while (game->nodes[firstIndexFree]->marble != NULL) firstIndexFree++;
    move (game->nodes[firstIndexFree]);

    if(type==PSY){
        game->chooseRespawn((this->owner+1)%2, src);
    }
}

/**
 * @brief Marble::updateAccessibleNodes
 */
void Marble::updateAccessibleNodes () {
    accessibleNodes.clear ();
    bool semi_paralysis = false;
    for (int i=0; i<6; i+=2){
        vector<Node*> thisPathAccessibleNodes;
        bool causeParalysis = false;
        bool cureParalysis = false;
        if (!node->dirPaths[i].empty()) {
            bool marbleEncountered  = false;
            for (int j=0; j<node->dirPaths[i].size(); j++) {
                Marble *m = node->dirPaths[i][j]->marble ;
                if (m == NULL){
                    if(!marbleEncountered && !semi_paralysis){
                        accessibleNodes.push_back(node->dirPaths[i][j]);
                        thisPathAccessibleNodes.push_back(node->dirPaths[i][j]);
                    }
                }else if (m->type != PSY) marbleEncountered = true;
                else if(m->owner != this->owner)
                    causeParalysis = true;
                else
                    cureParalysis = true;
            }
        }
        if(!node->dirPaths[i+1].empty()){
            bool marbleEncountered  = false;
            for (int j=0; j<node->dirPaths[i+1].size(); j++) {
                Marble *m = node->dirPaths[i+1][j]->marble ;
                if (m == NULL){
                    if(!marbleEncountered && !semi_paralysis){
                        thisPathAccessibleNodes.push_back(node->dirPaths[i+1][j]);
                        accessibleNodes.push_back(node->dirPaths[i+1][j]);
                    }
                }else if (m->type != PSY) marbleEncountered = true;
                else if(m->owner != this->owner)
                    causeParalysis = true;
                else
                    cureParalysis = true;
            }
        }
        if(semi_paralysis && (causeParalysis && !cureParalysis)){
            accessibleNodes.clear();
            break;
        }if(causeParalysis && !cureParalysis){
            semi_paralysis = true;
            accessibleNodes = thisPathAccessibleNodes;
            continue;
        }
    }
    // check for paralysis / semi_paralysis //
    // no check for: capture (suicide) / respawn //
}

/**
 * @brief Marble::isCaptured
 * @return
 */
bool Marble::isCaptured(){
    vector<int> surrounded;
    vector<int> watched;
    surrounded.clear();
    watched.clear();
    for(int i = 0; i < 3; i++){
        bool watchedOnPath = false;
        bool surroundedOnPath = false;
        Path* p = this->node->paths[i];
        if(p == NULL) continue;
        vector<Marble*> ms = p->marbles;

        for(int j = 0; j < ms.size(); j++){
            if(ms[j]->owner == this->owner) continue;
            if(ms[j]->type == DEL && !watchedOnPath){
                watchedOnPath = true;
                watched.push_back(i);
                continue;
            }if(ms[j]->type == INF && !surroundedOnPath){
                for(int k = j+1; k < ms.size(); k++){
                    if(ms[k]->owner == this->owner) continue;
                    if(ms[k]->type == INF && p->wellOrdered(ms[j]->node, this->node, ms[k]->node)){
                        surroundedOnPath = true;
                        surrounded.push_back(i);
                    }
                }
                continue;
            }
        }
    }
    for(int i = 0; i < surrounded.size(); i++){
        for(int j = 0; j < watched.size(); j++){
            if(surrounded[i]!=watched[j]){
                return true;
            }
        }
    }
    return false;
}

/**
 * @brief Marble::isAlive
 * @return
 */
bool Marble::isAlive(){ return this->node->index < NBNODES;}

/**
 * @brief Marble::isOnBorder
 * @return
 */
bool Marble::isOnBorder(){
    for(int i = 0; i < 3; i++){
        if(this->node->paths[i] != NULL && this->node->paths[i]->isBorder)
            return true;
    }
    return false;
}

/**
 * @brief Marble::type2str
 * @param type
 * @return
 */
string Marble::type2str (MarbleType type) {
    return (type==PKP)?"psychopath":((type==PSY)?"psychologist":((type==INF)?"doctor":((type==DEL)?"informer":"unknown")));
}

/**
 * @brief Marble::type2int
 * @param type
 * @return
 */
int Marble::type2int (MarbleType type) {
    return (type==PKP)?0:((type==PSY)?3:((type==INF)?2:((type==DEL)?1:-1)));
}

/**
 * @brief Marble::int2type
 * @param type
 * @return
 */
MarbleType Marble::int2type (int type) {
    return (type==0)?PKP:((type==3)?PSY:((type==2)?INF:((type==1)?DEL:X)));
}
