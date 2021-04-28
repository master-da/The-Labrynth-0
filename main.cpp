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


#define error {printf("sdl error in line %d. Error: %s\n", __LINE__, SDL_GetError()); close();}
#define error_i {printf("image error in line %d. Error: %s\n", __LINE__, IMG_GetError()); close();}
#define error_t {printf("ttf error in line %d. Error: %s\n", __LINE__, TTF_GetError()); close();}

// #include "loadUI.h"
// #include "under_construction.h"

const int WIDTH = 1280;
const int HEIGHT = 720;

void close();

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
SDL_Rect VIEWPORT_GLOBAL = { 0, 0, WIDTH, HEIGHT };
bool quit = false;
SDL_Event e;
bool screenUI;
bool screenUInt;
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

    void loadFromImage( std::string path ){
        SDL_Surface* tempImage = IMG_Load( path.c_str() );
        if(tempImage == NULL) error;

        // SDL_SetColorKey ( tempImage, SDL_TRUE, SDL_MapRGB( tempImage->format, r, g, b ) );

        texture = SDL_CreateTextureFromSurface( gRenderer, tempImage );
        if(texture == NULL) error;

        width = tempImage->w;
        height = tempImage->h;
        SDL_FreeSurface( tempImage );
    }

    void render( int x, int y ){
        SDL_Rect dest = {x, y, NULL, NULL};
        SDL_RenderCopy( gRenderer, texture, NULL, NULL);
    }

    private:
    SDL_Texture* texture = NULL;
    int width = 0;
    int height = 0;
};

class Button{
    public:
    Button(){
        int x = y = given_width= my_width = given_height= my_height = 0;
        button = NULL;
        buttonStatus = BUTTON_UNPRESSED;
        dest = {0, 0, given_width, given_height};
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
            else if(xc > dest.x+my_width) buttonStatus = BUTTON_UNPRESSED;
            else if(yc < dest.y) buttonStatus = BUTTON_UNPRESSED;
            else if(yc > dest.y+my_height) buttonStatus = BUTTON_UNPRESSED;
            else {
                if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) buttonStatus = BUTTON_PRESSED;
                else buttonStatus = BUTTON_HOVER;
            }
        }
    }

    void setSize( int w, int h ){ //must be called before setDest. Sets the width and height of button itself in source
        given_width = my_width = w;
        given_height = my_height = h;
    }

    //sets the position the button is rendered. Also needs width and height declared, defaulted to given_width
    void setDest( int x, int y, int width, int height ){
        if(!given_width || !given_height) printf("Warning! Button dimensions not set.\n");

        my_width = width;
        my_height = height;

        dest = {x, y, my_width, my_height};
    }

    void render(){      //checks if the button is pressed or not before rendering

        SDL_Rect source = {0, 0, given_width, given_height} ;
        if(buttonStatus == BUTTON_PRESSED) source.x = 2*given_width;
        else if( buttonStatus == BUTTON_HOVER) source.x = given_width;

        SDL_RenderCopy( gRenderer, button, &source, &dest );
    }

    int getStatus(){return buttonStatus;}

    private:
    SDL_Texture* button = NULL;
    SDL_Rect dest;
    int x = 0;
    int y = 0;

    int given_width = 0;
    int given_height = 0;

    int my_width = 0;
    int my_height = 0;
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
    //also sets no of sprite in the sheet, not of sprite per column and no of sprite per row
    void setinfo(int w, int h, int states , int per_row, int per_col ){ 
        width = w;
        height = h;
        sprite_states = states;
        sprites_per_row = per_row;
        sprites_per_col = per_col;
    }

    void setDest( int x, int y ){//sets the rectangle to render sprite in. setInfo muts've been called
        dest = {x, y, width, height};
    }

    void render(){
        int frame = slow/4; //frame holds which sprite state to render. Slowing speed by a factor of 4
        if(frame >= sprite_states) frame = slow = 0;    //resetting sprite to cycle around
        source = { width * (frame%sprites_per_row),  height * (frame/sprites_per_row), width , height };//setting which frame to be rendered
        SDL_RenderCopy( gRenderer, texture, &source, &dest );
        slow++;
    }

    private:
    SDL_Texture* texture;
    SDL_Rect source;
    SDL_Rect dest;
    int width = 0;
    int height = 0;
    int sprite_states = 0;
    int sprites_per_row = 0;
    int sprites_per_col = 0;
    int slow = 0;
};

Button button_start;
Button button_load;
Button button_options;
Button button_quit;

Sprite fire_sprite_left;
Sprite fire_sprite_right;

void loadmediaUI(){
    button_start.loadFromImage( "png/sprite_sheet/button_pressed_sprite_dark.png", 255, 255, 255);
    button_start.setSize(240, 120);
    button_start.setDest( 400, 100 + 0*120 , 480, 120);

    button_load.loadFromImage( "png/sprite_sheet/button_pressed_sprite_dark.png", 255, 255, 255);
    button_load.setSize(240, 120);
    button_load.setDest( 400, 100 + 1*120 , 480, 120);

    button_options.loadFromImage( "png/sprite_sheet/button_pressed_sprite_dark.png", 255, 255, 255);
    button_options.setSize(240, 120);
    button_options.setDest( 400, 100 + 2*120 , 480, 120);

    button_quit.loadFromImage( "png/sprite_sheet/button_pressed_sprite_dark.png", 255, 255, 255);
    button_quit.setSize(240, 120);
    button_quit.setDest( 400, 100 + 3*120 , 480, 120);

    fire_sprite_left.loadFromImage( "png/sprite_sheet/fire_sprite.png", 0, 0, 0 );
    fire_sprite_left.setinfo( 128, 128 , 25, 5, 5 );
    fire_sprite_left.setDest( 240, 296 );

    fire_sprite_right.loadFromImage( "png/sprite_sheet/fire_sprite.png", 0, 0, 0 );
    fire_sprite_right.setinfo( 128, 128 , 25, 5, 5 );
    fire_sprite_right.setDest( 912, 296 );
}

void loadUI(){
    while(!quit){
        if(!screenUI) break;
        while( SDL_PollEvent( &e ) ){
            if(e.type == SDL_QUIT) quit = 1;
            else {
                button_start.eventProcessor( e );
                button_load.eventProcessor( e );
                button_options.eventProcessor( e );
                button_quit.eventProcessor( e );
            }
        }

        SDL_SetRenderDrawColor( gRenderer, 0, 0, 0, 255);
        SDL_RenderClear( gRenderer );

        button_start.render(); 
        button_load.render(); 
        button_options.render();
        button_quit.render();        

        fire_sprite_left.render();
        fire_sprite_right.render();

        SDL_RenderPresent( gRenderer );
        printf("button_start status %d\n", button_start.getStatus());
        if( button_start.getStatus() == BUTTON_PRESSED ){
            screenUI = false;
            screenUInt = true;
            break;
        }
        if( button_load.getStatus() == BUTTON_PRESSED ){
            screenUI = false;
            screenUInt = true;
            break;
        }
        if( button_options.getStatus() == BUTTON_PRESSED ){
            screenUI = false;
            screenUInt = true;
            break;
        }
        if( button_quit.getStatus() == BUTTON_PRESSED ){
            screenUI = false;
            quit = true;
            break;
        }
    }
}

void UI(){
    loadmediaUI();
    loadUI();
}


LTexture noScreen;
Button noScreen_Back;

void loadmediaUInt(){
    noScreen.loadFromImage( "png/under_construction.png" );

    noScreen_Back.loadFromImage( "png/sprite_sheet/button_pressed_sprite.png", 255, 255, 255 );
    noScreen_Back.setSize( 240, 120 );
    noScreen_Back.setDest( 20, 20, 75, 75 );
}

void loadUInt(){
    while(!quit) {
        while(SDL_PollEvent( &e )){
            if( e.type == SDL_QUIT ) quit = 1;
            else noScreen_Back.eventProcessor( e );
        }

        SDL_SetRenderDrawColor( gRenderer, 0, 0, 0, 255 );
        SDL_RenderClear( gRenderer );

        noScreen.render(0, 0);
        noScreen_Back.render();

        SDL_RenderPresent( gRenderer );

        if( noScreen_Back.getStatus() == BUTTON_PRESSED ) {
            screenUInt = false;
            screenUI = true;
            break;
        }
    }
}


void UInt(){
    loadmediaUInt();
    loadUInt();
}


int main(int argc, char* argv[])
{
    init();
    quit = false;
    screenUI = true;
    screenUInt = false;
    

    while(!quit){
        if( screenUI ) UI();
        else if(screenUInt) UInt();
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