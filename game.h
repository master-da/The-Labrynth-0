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
#include <SDL2/SDL_mixer.h>

#define error {printf("SDL Error in line %d. Error: %s\n", __LINE__, SDL_GetError());}
#define error_i {printf("SDL Image Error in line %d. Error: %s\n", __LINE__, IMG_GetError());}
#define error_t {printf("TTF Error in line %d. Error: %s\n", __LINE__, TTF_GetError());}


struct Game{
    SDL_Window* window;
    SDL_Renderer* renderer;
    int SCREEN_WIDTH, SCREEN_HEIGHT;
    int current_screen;
    enum screens{
        UI_SCREEN,
        START_SCREEN,
        LOAD_SCREEN,
        OPTIONS_SCREEN,
        HISCORE_SCREEN,
    }
    enum buttonid{
        BUTTON_START,
        BUTTON_LOAD,
        BUTTON_OPTIONS,
        BUTTON_HISCORE,
        BUTTON_QUIT
    };

    Game( int width , int height){
        window = NULL;
        renderer = NULL;
        SCREEN_WIDTH = width;
        SCREEN_HEIGHT = height;
        current_screen = UI_SCREEN;
    }
    ~Game(){
        SDL_DestroyWindow( window );
        window = NULL;
        SDL_DestroyRenderer( renderer );
        renderer = NULL;
        close();
    }

    init( std::string name, int x, int y, bool fullscreen ){
        if(SDL_Init( SDL_INIT_VIDEO ) == 0) printf("SDL Initialised with video\n");
        else error;

        window = SDL_CreateWindow( name.c_str(), x, y, SCREEN_WIDTH, SCREEN_HEIGHT, fullscreen?SDL_WINDOW_FULLSCREEN:SDL_WINDOW_SHOWN );
        if( window ) printf("Window intialised\n");
        else error;

        if(IMG_Init( IMG_INIT_PNG )&IMG_INIT_PNG == IMG_INIT_PNG) printf("Image Initialised with PNG\n");
        else error_i;

        renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC );
        if(renderer) printf("Renderer initialised\n");
        else error;
    }

    close(){
        SDL_Quit();
        IMG_Quit();
    }
};

Game* gGame = new Game( 1280, 720 );

struct LTexture{
    SDL_Texture* texture;
    int width;
    int height;

    LTexture(){
        
        texture = NULL;
        width = 0;
        height = 0;
        
    }
    ~LTexture(){
        
        SDL_DestroyTexture( texture );
        width = 0;
        height = 0;
        
    }

    void loadFromFile( std::string path ){

        SDL_Surface* imgTemp = IMG_Load( path.c_str() );
        if( imgTemp ) printf("Image loaded from \"%s\"\n", path.c_str() );
        else error_i;
        
        texture = SDL_CreateTextureFromSurface( gGame->renderer, imgTemp);
        if( texture ) printf("Texture Loaded\n");
        else error;

        width = imgTemp->w;
        height = imgTemp->h;
        SDL_FreeSurface( imgTemp );
    }

    void mapFromFile( std::string path, Uint8 r, Uint8 g, Uint8 b){

        SDL_Surface* imgTemp = IMG_Load( path.c_str() );
        if( imgTemp ) printf(" Image loaded from %s\n", path.c_str() );
        else error_i;
        
        SDL_SetColorKey( imgTemp, SDL_TRUE, SDL_MapRGB( imgTemp->format, r, g, b ) );

        texture = SDL_CreateTextureFromSurface( gGame->renderer, imgTemp);
        if( texture ) printf(" Texture Loaded\n");
        else error;

        width = imgTemp->w;
        height = imgTemp->h;
        SDL_FreeSurface( imgTemp );
    }

    void render(int x, int y){
        SDL_Rect dest = {x, y, width, height};
        SDL_RenderCopy( gGame->renderer, texture, NULL, &dest);
    }

};

struct Button{
    LTexture button[3];
    SDL_Rect button_pos;
    int button_status;
    int buttonID;
    enum button_status{
        BUTTON_DEFAULT,
        BUTTON_HOVER,
        BUTTON_PRESSED
    };

    Button(int ID){
        buttonID = ID;
        
    }
    ~Button(){
        button_status = BUTTON_DEFAULT;
    }

    void loadFromFile( std::string default_path, std::string hover_path, std::string pressed_path){
        button[BUTTON_DEFAULT].loadFromFile( default_path );
        button[BUTTON_HOVER].loadFromFile( hover_path );
        button[BUTTON_PRESSED].loadFromFile( pressed_path );
    }

    void setDest(int x, int y){ button_pos = {x, y, button[BUTTON_DEFAULT].width, button[BUTTON_DEFAULT].height }; }

    void render( SDL_Event e){
        event_handler( e );
        SDL_RenderCopy( gGame->renderer, button[button_status], NULL, button_pos );
    }

    void event_handler( SDL_Event e){
        int x, y;
        SDL_GetMouseState( x, y );

        if(x < button_pos.x) button_status = BUTTON_DEFAULT;
        if(x > button_pos.x + button_pos.w) button_status = BUTTON_DEFAULT;
        if(y < button_pos.y) button_status = BUTTON_DEFAULT;
        if(y > button_pos.y + button_pos.h) button_status = BUTTON_DEFAULT;

        if( e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT && e.button.state == SDL_PRESSED ){
            buttos_s = BUTTON_PRESSED;
        }
        else if( e.type == SDL_MOUSEBUTTONUP  && e.button.button == SDL_BUTTON_LEFT && e.buton.state == SDL_RELEASED ){
            button_status = BUTTON_HOVER;
        }
        else button_status = BUTTON_HOVER;

    }
};