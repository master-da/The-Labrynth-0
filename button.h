#pragma once

#include "init.h"
#include "screen.h"
#include "game.h"

struct Button{

    

    SDL_Texture* look;
    int buttonID;
    SDL_Rect src;
    SDL_Rect dest;

    Game* game;

    Button(int buttonID_, int button_type, Game* game_){
        if(button_type == game->BUTTON_SMALL) src = {0, 0, 70, 70};
        else src = {0, 0, 300, 100};

        buttonID = buttonID_;
        game = game_;
    }

    ~Button(){
        SDL_DestroyTexture(look);
    }

    void loadFromFile(std::string path){
        SDL_Surface* imgTemp = IMG_Load(path.c_str());
        if(imgTemp == NULL) error_i
        
        look = SDL_CreateTextureFromSurface(game->renderer, imgTemp);
        if(look == NULL) error

        SDL_FreeSurface(imgTemp);
        imgTemp = NULL;
    }

    void set_dest(int x, int y){
        dest = {x, y, 300, 75};
    }

    bool mouse_in_button(int x, int y){
        return !((x < dest.x) ||
                (x > dest.x + dest.w) ||
                (y < dest.y) ||
                (y > dest.y + dest.h));
    }

    void handle_event(SDL_Event& e){
        int x, y;
        SDL_GetMouseState(&x, &y);
        if(mouse_in_button(x, y)) {
            if(e.type == SDL_MOUSEMOTION) src.x = src.w;
            else if(e.type == SDL_MOUSEBUTTONDOWN) src.x = 2*src.w;
            else if(src.x == 2*src.w && e.button.state == SDL_RELEASED) game->button_action(buttonID), src.x = 0;
        }
        else src.x = 0;
    }

    void render(){
        SDL_RenderCopy(game->renderer, look, &src, &dest);
        // printf("dest %d %d %d %d\n", dest.x, dest.y, dest.w, dest.h);
    }
};