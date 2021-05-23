#pragma once

#include <stdio.h>
#include <iostream>
#include <math.h>
#include <string>
#include <stdlib.h>
#include <cstring>
#include <algorithm>
#include <fstream>
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
#define error_m {printf("mixer error in line %d. Error: %s\n", __LINE__, MIX_GetError());}

struct Game{
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Rect camera;
    int SCREEN_WIDTH, SCREEN_HEIGHT;
    int RENDER_WIDTH, RENDER_HEIGHT;
    int current_screen;

    enum screens{
        UI_SCREEN,
        START_SCREEN,
        LOAD_SCREEN,
        OPTIONS_SCREEN,
        HISCORE_SCREEN,
        QUIT_SCREEN 
    };
    enum buttonid{
        BUTTON_START,
        BUTTON_LOAD,
        BUTTON_OPTIONS,
        BUTTON_HISCORE,
        BUTTON_QUIT,
        BUTTON_BACK
    };

    Game( int width , int height ){
        window = NULL;
        renderer = NULL;
        SCREEN_WIDTH = width;
        SCREEN_HEIGHT = height;
        RENDER_WIDTH = 800;
        RENDER_HEIGHT = 600;
        camera = {0, 0, RENDER_WIDTH, RENDER_HEIGHT};
        current_screen = UI_SCREEN;
    }
    ~Game(){
        close();
    }

    void init( std::string name, int x, int y, bool fullscreen ){
        if(SDL_Init( SDL_INIT_VIDEO ) == 0) printf("SDL Initialised with video\n");
        else error;

        window = SDL_CreateWindow( name.c_str(), x, y, RENDER_WIDTH, RENDER_HEIGHT, fullscreen?SDL_WINDOW_FULLSCREEN:SDL_WINDOW_SHOWN );
        if( window ) printf("Window intialised\n");
        else error;

        if(IMG_Init( IMG_INIT_PNG )&IMG_INIT_PNG == IMG_INIT_PNG) printf("Image Initialised with PNG\n");
        else error_i;

        renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC );
        if(renderer) printf("Renderer initialised\n");
        else error;
    }

    void button_action( int buttonID ){
        if( buttonID == BUTTON_START ) current_screen = START_SCREEN;
        if( buttonID == BUTTON_LOAD ) current_screen = LOAD_SCREEN;
        if( buttonID == BUTTON_OPTIONS ) current_screen = OPTIONS_SCREEN;
        if( buttonID == BUTTON_HISCORE ) current_screen = HISCORE_SCREEN;
        if( buttonID == BUTTON_QUIT ) current_screen = QUIT_SCREEN;
        if( buttonID == BUTTON_BACK ) current_screen = UI_SCREEN;
    }

    bool collission(SDL_Rect* a, SDL_Rect* b){
        if(a->x > b->x+b->w) return 0;
        if(a->x+a->w < b->x) return 0;
        if(a->y > b->y+b->h) return 0;
        if(a->y+a->w < b->y) return 0;
        return 1;
    }

    void camera_set(SDL_Rect* rect){
        camera.x = rect->x+rect->w/2-RENDER_WIDTH/2;
        camera.y = rect->y+rect->h/2-RENDER_HEIGHT/2;
        if(camera.x < 0) camera.x = 0;
        if(camera.x+camera.w > SCREEN_WIDTH) camera.x = SCREEN_WIDTH-camera.w;
        if(camera.y < 0) camera.y = 0;
        if(camera.y + camera.h > SCREEN_HEIGHT) camera.y = SCREEN_HEIGHT-camera.h;
    }

    void close(){
        SDL_DestroyWindow( window );
        window = NULL;
        
        SDL_DestroyRenderer( renderer );
        renderer = NULL;

        SDL_Quit();
        IMG_Quit();
    }
};

Game* gGame = new Game( 1280, 960 );

struct LTexture{
    SDL_Texture* texture;
    SDL_Rect pos;
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
        // if(imgTemp == NULL) error_i;
        
        texture = SDL_CreateTextureFromSurface( gGame->renderer, imgTemp);
        if( texture ) printf("Texture Loaded\n");
        else error;
        // if(texture == NULL) error;

        width = pos.w = imgTemp->w;
        height = pos.h = imgTemp->h;
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

    void setDest(int x, int y, int w, int h ){
        pos = {x, y, 50, 50};
    }

    void render(){
        // printf("\n...\nx %d y %d w %d h %d\n...\n", pos.x, pos.y, pos.w, pos.h);
        if( SDL_RenderCopy( gGame->renderer, texture, NULL, &pos) < 0) error;
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

    void setDest(int x, int y, int w, int h){
        // button_pos = {x, y, button[BUTTON_DEFAULT].width, button[BUTTON_DEFAULT].height }; 
        button_pos = {x, y, w, h }; 
        for(int i=0; i<3; i++) button[i].setDest(x, y, w, h);
    }

    void render( SDL_Event e){
        event_handler( e );
        // SDL_RenderCopy( gGame->renderer, button[button_status].texture, NULL, &button_pos );
        button[button_status].render();
    }

    void event_handler( SDL_Event e){
        int x, y;
        SDL_GetMouseState( &x, &y );

        if(x < button[BUTTON_DEFAULT].pos.x) button_status = BUTTON_DEFAULT;
        else if(x > button[BUTTON_DEFAULT].pos.x + button[BUTTON_DEFAULT].pos.w) button_status = BUTTON_DEFAULT;
        else if(y < button[BUTTON_DEFAULT].pos.y) button_status = BUTTON_DEFAULT;
        else if(y > button[BUTTON_DEFAULT].pos.y + button[BUTTON_DEFAULT].pos.h) button_status = BUTTON_DEFAULT;

        else if( e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT && e.button.state == SDL_PRESSED ){
            button_status = BUTTON_PRESSED;
        }
        else if( e.type == SDL_MOUSEBUTTONUP  && e.button.button == SDL_BUTTON_LEFT && e.button.state == SDL_RELEASED ){
            button_status = BUTTON_HOVER;
            gGame->button_action( buttonID );
        }
        else button_status = BUTTON_HOVER;

    }
};

struct Tile{
    SDL_Texture* tileImage;
    
    int tile_width, tile_height;
    int tile_type[30][40];

    struct gate_switch_parent{
        int gate_row;
        int gate_col;
    }par[10];

    enum tile_identity{
        TILE_WALL,
        TILE_SLOPE,
        TILE_WALK,
        TILE_BUTTON,
        TILE_GATE,
        TILE_TOTAL
    };

    SDL_Rect tiles[TILE_TOTAL];

    Tile( int w, int h ){
        tile_width = w;
        tile_height = h;

        tiles[TILE_WALL] = {32, 0, 32, 32};
        tiles[TILE_SLOPE] = {0, 32, 32, 32};
        tiles[TILE_WALK] = {0, 0, 32, 32};
        tiles[TILE_BUTTON] = {64, 0, 32, 32};
        tiles[TILE_GATE] = {32, 32, 32, 32};
    }

    void loadImageFromFile( std::string path ){

        SDL_Surface* imgTemp = IMG_Load( path.c_str() );
        if( imgTemp ) printf("Image loaded from \"%s\"\n", path.c_str() );
        else error_i;
        
        tileImage = SDL_CreateTextureFromSurface( gGame->renderer, imgTemp);
        if( tileImage ) printf("tileimage Loaded\n");
        else error;

        SDL_FreeSurface( imgTemp );
    }

    void loadInfoFromFile( std::string path ){
        std::ifstream map( path );

        if(!map.fail()) printf("map loaded from path \"%s\"\n", path.c_str());
        else printf("Failed to load map\n");

        for(int i=0; i<30; i++){
            for(int j=0; j<40; j++){
                map >> tile_type[i][j];

                if(tile_type[i][j]% 100 == 4) {
                    par[tile_type[i][j]/ 100].gate_row = i;
                    par[tile_type[i][j]/ 100].gate_col = j;
                }
            }
        }
        map.close();
    }

    int tile_gate_wall_collission (SDL_Rect* a, int trim ){
        int topleft = tile_type[ (a->y + trim) / tile_height ][ (a->x + trim)/ tile_width ] % 100;
        int topright = tile_type[ (a->y+trim)/tile_height ][ (a->x+a->w-trim)/tile_width ] % 100;
        int botleft = tile_type[ (a->y+a->h-trim)/tile_height ][ (a->x+trim)/tile_width ] % 100;
        int botright = tile_type[ (a->y+a->h-trim)/tile_height ][ (a->x+a->w-trim)/tile_width ] % 100;

        return( (topleft == TILE_WALL || topleft == TILE_SLOPE || topleft == TILE_GATE) ||
        (topright == TILE_WALL || topright == TILE_SLOPE || topright == TILE_GATE) ||
        (botleft == TILE_WALL || botleft == TILE_SLOPE || botleft == TILE_GATE) ||
        (botright == TILE_WALL || botright == TILE_SLOPE || botright == TILE_GATE) );
    }

    int tile_button_collission (SDL_Rect* a){
        if (tile_type[ a->y / tile_height ][ a->x / tile_width ] % 100 == TILE_BUTTON) 
            return tile_type[ a->y / tile_height ][ a->x / tile_width ] / 100; 

        if (tile_type[ a->y / tile_height ][ (a->x+a->w) / tile_width ] % 100 == TILE_BUTTON) 
            return tile_type[ a->y / tile_height ][ (a->x+a->w) / tile_width ] / 100; 

        if (tile_type[ (a->y+a->h) / tile_height ][ a->x / tile_width ] % 100 == TILE_BUTTON) 
            return tile_type[ (a->y+a->h) / tile_height ][ a->x / tile_width ] / 100; 

        if (tile_type[ (a->y+a->h) / tile_height ][ (a->x+a->w) / tile_width ] % 100 == TILE_BUTTON) 
            return tile_type[ (a->y+a->h) / tile_height ][ (a->x+a->w) / tile_width ] / 100; 

        else return 0;
    }

    void render( SDL_Rect& camera ){
        int x, y;
        int renderX = 0, renderY = 0;
        int translateX = (camera.x)%tile_width, translateY = (camera.y)%tile_height;

        for(y = camera.y; y<= (camera.y+camera.h+tile_height); y += tile_height, renderY += tile_height, renderX = 0){
            for(x = camera.x; x <= (camera.x+camera.w+tile_width); x += tile_width, renderX += tile_width){

                SDL_Rect dest = {renderX-translateX, renderY-translateY, tile_width, tile_height};
                
                int type = tile_type [ (int)(y / tile_height) ] [ (int)(x / tile_width) ] % 100;

                SDL_RenderCopy( gGame->renderer, tileImage, &tiles[TILE_WALK], &dest);

                float angle = 0.0;

                if(type == TILE_SLOPE){
                    if(tile_type [ (int)(y / tile_height) ] [ (int)(x / tile_width) + 1] == TILE_WALL) angle = 90.0;
                    else if(tile_type [ (int)(y / tile_height) ] [ (int)(x / tile_width) - 1] == TILE_WALL) angle = 270.0;
                    else if(tile_type [ (int)(y / tile_height) + 1] [ (int)(x / tile_width) ] == TILE_WALL) angle = 180.0;
                }
                else if(type == TILE_GATE && tile_type [ (int)(y / tile_height) + 1] [ (int)(x / tile_width) ] <= TILE_SLOPE) angle = 90.0;

                SDL_RenderCopyEx( gGame->renderer, tileImage, &tiles[type], &dest, angle, NULL, SDL_FLIP_NONE);
            }
        }
    }

}gTile(32, 32);

struct Player{

    enum player_status_list{
        PLAYER_IDLE,//0
        PLAYER_MOVE,//1
        PLAYER_TOTAL_STATI//2
    };

    int xVel, yVel;
    int frame;
    int slow_factor[PLAYER_TOTAL_STATI];

    SDL_RendererFlip flip;

    int sprite_per_row[PLAYER_TOTAL_STATI];
    int sprite_per_col[PLAYER_TOTAL_STATI];
    int player_status;

    SDL_Texture* look[PLAYER_TOTAL_STATI];
    SDL_Rect src;
    SDL_Rect dest;
    SDL_Point player_center;

    Player(int x, int y, int w, int h){
        xVel = 0;
        yVel = 0;
        frame = 0;

        slow_factor[PLAYER_IDLE] = 7;
        slow_factor[PLAYER_MOVE] = 4;

        sprite_per_row[PLAYER_IDLE] = 6;
        sprite_per_col[PLAYER_IDLE] = 2;
        sprite_per_row[PLAYER_MOVE] = 6;
        sprite_per_col[PLAYER_MOVE] = 3;
        player_status = PLAYER_IDLE;

        player_center = {x*w + w/2, y*h + h/2};
        dest = {x*w, y*h, w, h};
        src = {0, 0, w, h};

        flip = SDL_FLIP_NONE;
    }
    ~Player(){
        xVel = yVel = 0;
    }

    void loadFromFile( std::string idle_path, std::string move_path ){

        SDL_Surface* imgTemp = IMG_Load( idle_path.c_str() );
        if( imgTemp ) printf("Image loaded from \"%s\"\n", idle_path.c_str() );
        else error_i;
        
        look[PLAYER_IDLE] = SDL_CreateTextureFromSurface( gGame->renderer, imgTemp);
        if( look[PLAYER_IDLE] ) printf("player texture Loaded\n");
        else error;

        imgTemp = IMG_Load( move_path.c_str() );
        if( imgTemp ) printf("Image loaded from \"%s\"\n", move_path.c_str() );
        else error_i;
        
        look[PLAYER_MOVE] = SDL_CreateTextureFromSurface( gGame->renderer, imgTemp);
        if( look[PLAYER_MOVE] ) printf("player texture Loaded\n");
        else error;

        SDL_FreeSurface( imgTemp );
    }

    void event_handler( SDL_Event& e ){

        static bool keyUpCheck = false;

        if(e.type == SDL_KEYDOWN && !e.key.repeat ){

            if(e.key.keysym.sym == SDLK_w) yVel = -6, player_status = PLAYER_MOVE, frame = 0, keyUpCheck = true;
            if(e.key.keysym.sym == SDLK_s) yVel = 6, player_status = PLAYER_MOVE, frame = 0, keyUpCheck = true;
            if(e.key.keysym.sym == SDLK_a) xVel = -6, player_status = PLAYER_MOVE, frame = 0, keyUpCheck = true, flip = SDL_FLIP_HORIZONTAL;
            if(e.key.keysym.sym == SDLK_d) xVel = 6, player_status = PLAYER_MOVE, frame = 0, keyUpCheck = true, flip = SDL_FLIP_NONE;
            
            if(e.key.keysym.sym == SDLK_e){
                int tile_button_collission = gTile.tile_button_collission(&dest);

                if(tile_button_collission){
                    int gate_row = gTile.par[tile_button_collission].gate_row;
                    int gate_col = gTile.par[tile_button_collission].gate_col;
                    gTile.tile_type[gate_row][gate_col] = gTile.TILE_WALK;
                }
            }
        }
        else if(e.type == SDL_KEYUP && keyUpCheck) xVel = yVel = 0, player_status = PLAYER_IDLE, frame = 0, keyUpCheck = false;

        frame++;
        frame %= (sprite_per_row[player_status] * sprite_per_col[player_status] * slow_factor[player_status] );

        bool wallCollission = false;

        dest.x += xVel;
        if(dest.x<0 || dest.x + dest.w > gGame->SCREEN_WIDTH || gTile.tile_gate_wall_collission(&dest, 7) ) dest.x -= xVel; 

        wallCollission = false;

        dest.y += yVel;
        if(dest.y<0 || dest.y + dest.h > gGame->SCREEN_HEIGHT || gTile.tile_gate_wall_collission(&dest, 7)) dest.y -= yVel;

        player_center.x = dest.x + dest.w / 2;
        player_center.y = dest.y + dest.h / 2;
    }

    void render(){
        int current_frame = frame/slow_factor[player_status];

        dest.x -= gGame->camera.x;
        dest.y -= gGame->camera.y;

        src.x = (current_frame % sprite_per_row[player_status]) * src.w;
        src.y = (current_frame / sprite_per_row[player_status]) * src.h;

        SDL_RenderCopyEx( gGame->renderer, look[player_status], &src, &dest , 0.0, NULL, flip );

        dest.x += gGame->camera.x;
        dest.y += gGame->camera.y;
    }

}gPlayer(34, 4, 32, 32);//gPlayer size 32, 32

struct Enemy{
    
    enum enemy_status_list{
        ENEMY_PATROL,
        ENEMY_ATTACK,
        ENEMY_TOTAL
    };

    int originX;
    int originY;
    int xVel;
    int yVel;
    bool route;
    int enemy_status;
    int frame;
    int slow_factor;
    int route_length;
    int sprites_per_row;
    int sprites_per_col;
    int attack_range;

    SDL_Texture* look;
    SDL_Rect src;
    SDL_Rect dest;
    SDL_RendererFlip flip;
    SDL_Point enemy_center;

    Enemy(SDL_Rect dimensions, bool route){

        src = { 0, 0, dimensions.w, dimensions.h };
        dest = { dimensions.x*dimensions.w, dimensions.y*dimensions.h, dimensions.w, dimensions.h };
        originX = dimensions.x;
        originY = dimensions.y;
        enemy_center = {dimensions.x*dimensions.w + dimensions.w/2, dimensions.y*dimensions.h + dimensions.h / 2};

        xVel = route*2;
        yVel = !route*2;
        frame = 0;
        slow_factor = 6;
        route_length = 50;
        sprites_per_row = 6;
        sprites_per_col = 3;
        attack_range = 200;

        enemy_status = ENEMY_PATROL;
        flip = SDL_FLIP_NONE;
        look = NULL;
    }
    ~Enemy(){
        SDL_DestroyTexture( look );
        look = NULL;
    }

    void loadFromFile( std::string path ){
        SDL_Surface* tempImage = IMG_Load( path.c_str() );
        if(tempImage == NULL) error_i;

        look = SDL_CreateTextureFromSurface( gGame->renderer, tempImage );
        if( look ) printf("Enemy Texture was loaded from \"%s\"\n", path.c_str());
        else error;

        SDL_FreeSurface( tempImage );
        tempImage = NULL;
    }

    struct LineOfSight{

        int dx;
        int dy;
        int range;

        SDL_Point start;
        SDL_Point end;

        LineOfSight( SDL_Point* player_center , SDL_Point* enemy_center, int& attack_range){
            start = *player_center;
            end = *enemy_center;
            
            dx = end.x - start.x;
            dy = end.y  - start.y;
            range = attack_range * attack_range;
        }

        bool in_range(){
            return dx * dx + dy * dy <= range;
        }

        bool low_slope( SDL_Point start, SDL_Point end ){
            
            int y_increment = gTile.tile_width;
            int x_increment = gTile.tile_height;
            if( dy < 0 ) y_increment = -y_increment, dy = -dy;

            int decision_factor = 2 * dy - dx;
            while(start.x < end.x){
                start.x += x_increment;

                if(decision_factor > 0) start.y += y_increment, decision_factor += (2*(dy-dx));
                else decision_factor += (2*dy);

                int tileType = gTile. tile_type [start.y/gTile.tile_height][start.x/gTile.tile_width];
                if( (tileType == gTile.TILE_WALL) || (tileType == gTile.TILE_SLOPE) || (tileType ==gTile.TILE_GATE) ) {
                    return 0;
                }
            }

            return 1;
        }

        bool hi_slope( SDL_Point start, SDL_Point end ){
            if( dy < 0 ) return low_slope( end, start );

            int x_increment = gTile.tile_height;
            int y_increment = gTile.tile_width;
            if( dx < 0 ) x_increment = -x_increment, dx = -dx;

            int decision_factor = 2 * dx - dy;
            while(start.y < end.y){
                start.y += y_increment;

                if(decision_factor > 0) start.x += x_increment, decision_factor += (2*(dx-dy));
                else decision_factor += (2*dx);

                int tileType = gTile. tile_type [start.y/gTile.tile_height][start.x/gTile.tile_width];
                if( (tileType == gTile.TILE_WALL) || (tileType == gTile.TILE_SLOPE) || (tileType ==gTile.TILE_GATE) ) {
                    return 0;
                }
            }

            return 1;
        }

        bool line() {
            if( dx * dx + dy * dy > range ) {
                return 0;
            }
            if( abs(dy) > abs(dx) ) {
                if(dy > 0) return hi_slope( start, end) ;
                else return hi_slope( end, start );
            }
            else {
                if(dx > 0) return low_slope( start, end );
                else return low_slope( end, start );
            }
        }
    };

    void handle_event(){
        LineOfSight loS( &gPlayer.player_center, &enemy_center, attack_range );
        if(loS.line())  {
            return;
        }

        bool wallCollission = false;

        dest.x += xVel;
        if(dest.x<0 || dest.x + dest.w > gGame->SCREEN_WIDTH || gTile.tile_gate_wall_collission(&dest, 0) || dest.x == originX + route_length || dest.x == originX) xVel = -xVel;

        wallCollission = false;

        dest.y += yVel;
        if(dest.y<0 || dest.y + dest.h > gGame->SCREEN_HEIGHT || gTile.tile_gate_wall_collission(&dest, 0) || dest.y == originY + route_length || dest.y == originY) yVel = -yVel;

        enemy_center = {dest.x + dest.w / 2, dest.y + dest.h / 2};

        if(xVel > 0) flip = SDL_FLIP_NONE;
        if(xVel < 0) flip = SDL_FLIP_HORIZONTAL;
        frame++;
        frame %= (sprites_per_row*sprites_per_col*slow_factor);
    }

    void render( SDL_Rect& camera ){

        int current_frame = frame/slow_factor;
        src.x = current_frame % sprites_per_row;
        src.y = current_frame / sprites_per_row;

        int translateX = camera.x;
        int translateY = camera.y;

        dest.x -= translateX;
        dest.y -= translateY;

        SDL_RenderCopyEx( gGame->renderer, look, &src, &dest, 0.0, NULL, flip );

        dest.x += translateX;
        dest.y += translateY;
    }
}gEnemy_one_lr({26, 24, 32, 32}, 1);