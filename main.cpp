#include <stdio.h>
#include <iostream>
#include <math.h>
#include <string>
#include <stdlib.h>
#include <cstring>
#include <algorithm>
#include <math.h>
#include <cstdio>
#include <vector>
#include <map>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "UI.cpp"

#define error {printf("sdl error in line %d. Error: %s\n", __LINE__, SDL_GetError()); close();}
#define error_i {printf("image error in line %d. Error: %s\n", __LINE__, IMG_GetError()); close();}
#define error_t {printf("ttf error in line %d. Error: %s\n", __LINE__, TTF_GetError()); close();}

const int WIDTH = 1280;
const int HEIGHT = 720;

void close();

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
SDL_Rect VIEWPORT_GLOBAL = { 0, 0, WIDTH, HEIGHT };
enum buttonStatus{
    BUTTON_UNPRESSED,
    BUTTON_HOVER,
    BUTTON_PRESSED
};

void init(){
    if(SDL_Init( SDL_INIT_VIDEO ) < 0) error;

    if( !(IMG_Init( IMG_INIT_PNG ) & IMG_INIT_PNG) ) error_i;

    gWindow = SDL_CreateWindow( "UI", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN );
    if( gWindow == NULL ) error;

    gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
    if( gRenderer == NULL ) error;
}

class LTexture{
    public:
    LTexture(){
        texture = NULL;
        width = 0;
        height = 0;
    }

    ~LTexture(){
        ltclose();
    }

    void ltclose(){
        SDL_DestroyTexture( texture );
        texture = NULL;
        width = 0;
        height = 0;
    }

    void loadFromImage( std::string path , Uint8 r, Uint8 g ,Uint8 b ){
        SDL_Surface* tempImage = IMG_Load( path.c_str() );
        if(tempImage == NULL) error;

        SDL_SetColorKey ( tempImage, SDL_TRUE, SDL_MapRGB( tempImage->format, r, g, b ) );

        texture = SDL_CreateTextureFromSurface( gRenderer, tempImage );
        if(texture == NULL) error;

        width = tempImage->w;
        height = tempImage->h;
        SDL_FreeSurface( tempImage );
    }

    private:
    SDL_Texture* texture = NULL;
    int width = 0;
    int height = 0;
};

class Button{
    public:
    Button(){
        int x = y = width = height = 0;
        button = NULL;
        buttonStatus = BUTTON_UNPRESSED;
        dest = {0, 0, width, height};
    }

    void loadFromImage( std::string path , Uint8 r, Uint8 g ,Uint8 b ){ //loads all button states as a texture
        SDL_Surface* tempImage = IMG_Load( path.c_str() );
        if(tempImage == NULL) error;

        SDL_SetColorKey ( tempImage, SDL_TRUE, SDL_MapRGB( tempImage->format, r, g, b ) );

        button = SDL_CreateTextureFromSurface( gRenderer, tempImage );
        if(button == NULL) error; 

        SDL_FreeSurface( tempImage );
    }

    void eventProcessor( SDL_Event e){
        if(e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP){
            int xc, yc;
            SDL_GetMouseState(&xc, &yc);
            if(xc < dest.x) buttonStatus = BUTTON_UNPRESSED;
            else if(xc > dest.x+width) buttonStatus = BUTTON_UNPRESSED;
            else if(yc < dest.y) buttonStatus = BUTTON_UNPRESSED;
            else if(yc > dest.y+height) buttonStatus = BUTTON_UNPRESSED;
            else {
                if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) buttonStatus = BUTTON_PRESSED;
                else buttonStatus = BUTTON_HOVER;
            }
        }
    }

    void setSize( int w, int h ){ //must be called before setDest. Sets the width and height of button itself
        width = w;
        height = h;
    }

    void setDest(int x, int y){ //sets the position the button is rendered. Must have width and height defined
        if(!width || !height) printtf("Warning! Button dimensions not set.\n";)
        dest = {x, y, width, height};
    }

    void render(){      //checks if the button is pressed or not before rendering
        SDL_Rect source = {0, 0, width, height} ;
        if(buttonStatus == BUTTON_PRESSED) source.x = 2*width;
        else if( buttonStatus == BUTTON_HOVER) source.x = width;

        SDL_RenderCopy( gRenderer, button, &source, &dest );
    }

    private:
    SDL_Texture* button = NULL;
    SDL_Rect dest;
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
    int buttonStatus = BUTTON_UNPRESSED;
};

class Sprite{
    public:
    Sprite(){
        texture = NULL;
        width = 0;
        height = 0;
        slow = 0;
    }

    void loadFromImage( std::string path , Uint8 r, Uint8 g ,Uint8 b ){ //laods entire sprite-sheet
        SDL_Surface* tempImage = IMG_Load( path.c_str() );
        if(tempImage == NULL) error;

        SDL_SetColorKey ( tempImage, SDL_TRUE, SDL_MapRGB( tempImage->format, r, g, b ) );

        texture = SDL_CreateTextureFromSurface( gRenderer, tempImage );
        if(texture == NULL) error;

        SDL_FreeSurface( tempImage );
    }

    //sets the width and heiht of each sprite state. Must be called before setDest
    void setinfo(int w, int h, int states , int per_row, int per_col ){ 
        width = w;
        height = h;
        sprite_states = states;
        sprites_per_row = per_row;
        sprites_per_col = per_col;
    }

    void setDest( int x1, int y1, int x2, int y2 ){//sets the rectangle to render sprite in. setInfo muts've been called
        dest1 = {x1, y1, width, height};
        dest2 = {x2, y2, width, height}; //im being lazy and rendering same sprite in 2 positions. I should fix.
    }

    void render(){
        int frame = slow/4; //frame holds which sprite state to render. Slowing speed by a factor of 4
        if(frame >= sprite_states) frame = slow = 0;    //resetting sprite to cycle around
        source = { width * (frame%sprites_per_row),  height * (frame/sprites_per_row), width , height };//setting which frame to be rendered
        SDL_RenderCopy( gRenderer, texture, &source, &dest1 );
        SDL_RenderCopy( gRenderer, texture, &source, &dest2 );
        slow++;
    }

    private:
    SDL_Texture* texture;
    SDL_Rect source;
    SDL_Rect dest1;
    SDL_Rect dest2;
    int width = 0;
    int height = 0;
    int sprite_states = 0;
    int sprites_per_row = 0;
    int sprites_per_col = =0;
    int slow = 0;
};

Button butarr[4];
Sprite fire_sprite;

void loadmedia(){
    for(int i=0; i<4; i++) {
        butarr[i].loadFromImage( "png/sprite_sheet/button_pressed_sprite_dark.png", 255, 255, 255);
        butarr[i].setSize(240, 120);
        butarr[i].setDest( 520, 100 + i*120 );
    }
    fire_sprite.loadFromImage( "png/sprite_sheet/fire_sprite.png", 0, 0, 0 );
    fire_sprite.setSize( 128, 128 );
    fire_sprite.setDest( 240, 296, 912, 296);
}

int main(int argc, char* argv[])
{
    init();
    loadmedia();
    bool quit = false;
    SDL_Event e;

    while(!quit){
        while( SDL_PollEvent( &e ) ){
            if(e.type == SDL_QUIT) quit = 1;
            else for(int i=0; i<4; i++) butarr[i].eventProcessor( e );
        }

        SDL_SetRenderDrawColor( gRenderer, 0, 0, 0, 255);
        SDL_RenderClear( gRenderer );
        for(int i=0; i<4; i++) butarr[i].render();
        fire_sprite.render();
        SDL_RenderPresent( gRenderer );
    }
    
    return 0;
}

void close(){
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;

    SDL_DestroyRenderer( gRenderer );
    gRenderer = NULL;

    SDL_Quit();
    IMG_Quit();
}