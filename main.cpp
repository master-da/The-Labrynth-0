#include "game.h"

void loadmedia();
void close();

LTexture one;
int main(int argc, char* argv[])
{
    gGame->init( "game.exe", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0);
    loadmedia();
    SDL_Event e;
    bool quit = 0;

    while (!quit){

    }

    return 0;
}

void loadmedia(){
    one.loadFromFile( "png/logo.png" );
}