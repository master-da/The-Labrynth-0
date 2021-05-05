#include "game.h"

void loadmedia();
void close();

LTexture logo;
LTexture backup;

Button button_start(gGame->BUTTON_START);
Button button_load(gGame->BUTTON_LOAD);
Button button_options(gGame->BUTTON_OPTIONS);
Button button_hiscore(gGame->BUTTON_HISCORE);
Button button_quit(gGame->BUTTON_QUIT);

int main(int argc, char* argv[])
{
    gGame->init( "game.exe", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0);
    loadmedia();
    SDL_Event e;
    bool quit = 0;

    while (!quit){
        SDL_PollEvent( &e );
        if( e.type == SDL_QUIT ) quit = true;

        SDL_RenderClear( gGame->renderer );
        SDL_SetRenderDrawColor(gGame->renderer, 0, 0, 0, 255);
        
        if(gGame->current_screen == gGame->QUIT_SCREEN ) quit = true;

        else if( gGame->current_screen == gGame->UI_SCREEN ) {
            logo.render();
            button_start.render( e );
            button_load.render( e );
            button_options.render( e );
            button_hiscore.render( e );
            button_quit.render( e );
        }
        else{
            backup.render();
        }

        SDL_RenderPresent( gGame->renderer );
    }

    return 0;
}

void loadmedia(){
    logo.loadFromFile( "png/logo.png" );
    logo.setDest( ( gGame->SCREEN_WIDTH-logo.width)/2, 120 , logo.width, logo.height );

    backup.loadFromFile( "png/under_construction.png ");
    backup.setDest( (gGame->SCREEN_WIDTH-backup.width)/2, (gGame->SCREEN_HEIGHT-backup.height)/2, backup.width, backup.height );

    button_start.loadFromFile( "png/buttons/start_default.png", "png/buttons/start_hover.png", "png/buttons/start_pressed.png");
    button_start.setDest( (gGame->SCREEN_WIDTH-250)/2 , 170, 250, 180);
    printf("\n...\nx %d y %d w %d h %d\n...\n", button_start.button_pos.x, button_start.button_pos.y, button_start.button_pos.w, button_start.button_pos.h);
    printf("\n...\nx %d y %d w %d h %d\n...\n", button_start.button[button_start.BUTTON_DEFAULT].pos.x, button_start.button[button_start.BUTTON_DEFAULT].pos.y, button_start.button[button_start.BUTTON_DEFAULT].pos.w, button_start.button[button_start.BUTTON_DEFAULT].pos.h);

    button_load.loadFromFile( "png/buttons/load_default.png", "png/buttons/load_hover.png", "png/buttons/load_pressed.png");
    button_load.setDest( (gGame->SCREEN_WIDTH-250)/2 , 240, 250, 180);

    button_options.loadFromFile( "png/buttons/options_default.png", "png/buttons/options_hover.png", "png/buttons/options_pressed.png");
    button_options.setDest( (gGame->SCREEN_WIDTH-250)/2 , 310, 250, 180);

    button_hiscore.loadFromFile( "png/buttons/highscore_default.png", "png/buttons/highscore_hover.png", "png/buttons/highscore_pressed.png");
    button_hiscore.setDest( (gGame->SCREEN_WIDTH-250)/2 , 380, 250, 180);

    button_quit.loadFromFile( "png/buttons/quit_default.png", "png/buttons/quit_hover.png", "png/buttons/quit_pressed.png");
    button_quit.setDest( (gGame->SCREEN_WIDTH-250)/2 , 450, 250, 180);
}