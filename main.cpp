#include "game.h"


int main()
{
   Game *game = new Game();
   //cout << game->runMinimaxAlphaBeta(NULL, 10, INT_MIN, INT_MAX, true) << endl;
   //game->runMinimax();
   /*
   for(int i = 0 ; i < 15 ; i++){
    game->generateGames(1000, i, 10);
   }
   */

    Marble * psycho0;
    Marble * psycho1;
    for(int i = 0 ; i < 2 ; i++){
        for(int j = 0 ; j < NBMARBLES ; j++){
            if(game->marbles[i][j]->type == PKP){
                if(i)
                    psycho1 = game->marbles[i][j];
                else
                    psycho0 = game->marbles[i][j];
            }
        }
    }

    int coups=0;
    while(psycho1->isAlive() && psycho0->isAlive()){
        cout << game->whosTurn << " joue. (" << coups << ")" << endl;
        coups++;
        game->nextTurn();
    }

    cout << " fin." << endl;
   
   return 0;
}
