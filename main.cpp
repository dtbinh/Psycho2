#include "game.h"


int main()
{
   Game *game = new Game();
   //game->runMinimax();
   for(int i = 0 ; i < 16 ; i+=5){
       for(int j = 0 ; j < 16 ; j+=5){
            cout << "morts = " << i << " bord = " << j << endl;
            game->generateGames(1000, i, j);
       }
   }

   return 0;
}
