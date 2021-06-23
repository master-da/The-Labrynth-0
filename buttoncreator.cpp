#include "init.h"
#include "game.h"

Game* game = new Game(); 

struct Texture{
    SDL_Texture* t;
    int w;
    int h;
    SDL_Rect dest;
    void create(SDL_Surface* surface){
        t = SDL_CreateTextureFromSurface(game->renderer, surface);
        w = surface->w;
        h = surface->h;
    }
    void set_dest(std::string place){
        if(place == "start") dest = {(300-w)/2, (100-h)/2, w, h};
        else if(place == "mid") dest = {300 + (300-w)/2, (100-h)/2, w, h};
        else if(place == "end") dest = {600 + (300-w)/2, (100-h)/2, w, h};
    }
    void render(){
        SDL_RenderCopy(game->renderer, t, NULL, &dest);
    }
};

int main(int argc, char* argv[]){
    game->init("buttoncreator", 0);
    game->resize_window(900, 100);

    game->font = TTF_OpenFont("fonts/quicksand/Quicksand-Regular.ttf", 48);//48 == FONT SIZE
    SDL_Surface* tmpSurface = TTF_RenderText_Solid(game->font, "HOME", {0, 0, 0});//YHIS HOME
    Texture text[3];
    text[0].create(tmpSurface);
    text[0].set_dest("start");
    tmpSurface = TTF_RenderText_Solid(game->font, "HOME", {255, 255, 255});//THIS HOMR
    text[1].create(tmpSurface);
    text[1].set_dest("mid");
    text[2].create(tmpSurface);
    text[2].set_dest("end");

    tmpSurface = IMG_Load("png/buttons/button_default.png");
    Texture button;
    button.create(tmpSurface);

    SDL_FreeSurface(tmpSurface);
    tmpSurface = NULL;

    SDL_Event e;
    bool quit = true;
    while(quit){
        while(SDL_PollEvent(&e));
        if(e.type == SDL_QUIT) quit = false;
        SDL_RenderClear(game->renderer);
        SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);

        SDL_RenderCopy(game->renderer, button.t, NULL, NULL);
        for(int i=0; i<3; i++) text[i].render();
        
        SDL_RenderPresent(game->renderer);
        
    }
}