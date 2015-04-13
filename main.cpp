#include "game.h"


int main()
{
   Game *game = new Game();
   //game->runMinimax();
   game->generateGames(1000, false);
   
   return 0;
}
