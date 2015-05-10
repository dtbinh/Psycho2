#include "game.h"

#include <unistd.h>


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
    int victoiresNoir;
    int victoiresBlanc;
    victoiresNoir = victoiresBlanc = 0;
    moyenne = coupsTot = 0.0;
    double parties = 1.0;
    int cpt=0;
    for(int i = 0 ; i < parties ; i++){
        cout << i << endl;
        game = new Game();
        cpt = game->letsplay(HUMAN, ALPHABETA); // CHOOSE THE PLAYERS HERE. HUMAN CAN ONLY BE PLAYER WHITE
        coupsTot+=abs(cpt);
        if(cpt > 0) victoiresBlanc++;
        else victoiresNoir++;
        //delete game;
    }
    moyenne = coupsTot / parties;

    cout << "moyenne = " << moyenne << endl;
    cout << "noir wins " << victoiresNoir << endl;
    cout << "blanc wins " << victoiresBlanc << endl;

   return 0;
}
