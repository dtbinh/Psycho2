#include "game.h"


int main()
{
   Game *game = new Game();
   //game->runMinimax();
   for(int i = 0 ; i < 15 ; i++){
    game->generateGames(1000, i, 10);
   }
   
   return 0;
}
