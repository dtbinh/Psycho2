#include "game.h"



int main()
{
   Game *game;
   //cout << game->runMinimaxAlphaBeta(NULL, 10, INT_MIN, INT_MAX, true) << endl;
   //game->runMinimax();
   /*
   for(int i = 0 ; i < 15 ; i++){
    game->generateGames(1000, i, 10);
   }
   */
    double coupsTot, moyenne;
    moyenne = coupsTot = 0.0;
    for(int i = 0 ; i < 1 ; i++){
        cout << i << endl;
        game = new Game();
        coupsTot += game->letTheBotFightBegin();
        //delete game;
    }
    moyenne = coupsTot / 10.0;

    cout << "moyenne = " << moyenne << endl;
   
   return 0;
}
