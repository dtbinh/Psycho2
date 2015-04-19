#include "game.h"


int main()
{
   Game *game = new Game();
   cout << game->runMinimaxAlphaBeta(NULL, 10, INT_MIN, INT_MAX, true) << endl;
   //game->runMinimax();
   /*
   for(int i = 0 ; i < 15 ; i++){
    game->generateGames(1000, i, 10);
   }
   */
   
   return 0;
}
