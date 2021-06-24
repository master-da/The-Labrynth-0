#include "init.h"
#include "game.h"
#include "tile.h"
#include "player.h"
#include "enemy.h"
#include "screen.h"

int main(int argc, char* argv[]) {

    Game* game = new Game();
    game->init("a", 0);
    game->text_loader(46);

    while (game->game_running) {
        if(game->current_screen == game->UI_SCREEN) UI(game);
        else if(game->current_screen == game->OPTIONS_SCREEN) options(game);
        else if(game->current_screen == game->LEVEL_CHOICE) levels(game);
        else if(game->current_screen == game->CREDITS_SCREEN) credits(game);
        else if(game->current_screen == game->LEVEL_1) level_one(game);
        else if(game->current_screen == game->LEVEL_2) level_two(game);
        else if(game->current_screen == game->LEVEL_3) level_three(game);
    }

    game->close();
    return 0;
}