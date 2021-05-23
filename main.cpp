#include "game.h"

void loadmedia(){
    gPlayer.loadFromFile("png/idle_player.png", "png/walk_player.png");
    gEnemy_one_lw.loadFromFile("png/walk_enemy.png");
    gTile.loadImageFromFile( "png/level-1-tiles.png" );
    gTile.loadInfoFromFile( "map/mapped.txt ");
}

int main(int argc, char* argv[])
{
    gGame->init( "a", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0 );
    SDL_Event e;
    bool quit = false;
    loadmedia();

    while(!quit){
        SDL_PollEvent( &e );
        if(e.type == SDL_QUIT) quit = 1;

        SDL_RenderClear( gGame->renderer );
        SDL_SetRenderDrawColor( gGame->renderer, 0, 0, 0, 255);

        gPlayer.event_handler( e );
        gEnemy_one_lw.move();
        gGame->camera_set( &gPlayer.dest );

        gTile.render( gGame->camera );
        gEnemy_one_lw.render( gGame->camera );
        gPlayer.render();

        SDL_RenderPresent( gGame->renderer );
    }
    
    gGame->close();
    return 0;
}