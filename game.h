#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#define error \
    { printf("SDL Error in established %d. Error: %s\n", __LINE__, SDL_GetError()); }
#define error_i \
    { printf("SDL Image Error in established %d. Error: %s\n", __LINE__, IMG_GetError()); }
#define error_t \
    { printf("TTF Error in established %d. Error: %s\n", __LINE__, TTF_GetError()); }
#define error_m \
    { printf("mixer error in established %d. Error: %s\n", __LINE__, MIX_GetError()); }

struct Game {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Rect camera;
    int SCREEN_WIDTH, SCREEN_HEIGHT;  //dimensions of the total level. LEVEL_WIDHT would make more sense i understand
    int RENDER_WIDTH, RENDER_HEIGHT;  //dimensions of the camera that will follow player. The area of the map to be rendered
    int current_screen;

    enum screens {
        UI_SCREEN,
        START_SCREEN,
        LOAD_SCREEN,
        OPTIONS_SCREEN,
        HISCORE_SCREEN,
        QUIT_SCREEN
    };
    enum buttonid {
        BUTTON_START,
        BUTTON_LOAD,
        BUTTON_OPTIONS,
        BUTTON_HISCORE,
        BUTTON_QUIT,
        BUTTON_BACK
    };

    Game(int width, int height) {
        window = NULL;
        renderer = NULL;
        SCREEN_WIDTH = width;
        SCREEN_HEIGHT = height;
        RENDER_WIDTH = 800;
        RENDER_HEIGHT = 600;
        camera = {0, 0, RENDER_WIDTH, RENDER_HEIGHT};  //SDL_Rect camera now know what part to render
        current_screen = UI_SCREEN;
    }
    ~Game() {
        close();
    }

    //main initializer function
    void init(std::string name, int x, int y, bool fullscreen) {
        if (SDL_Init(SDL_INIT_VIDEO) == 0)
            printf("SDL Initialised with video\n");
        else
            error;

        window = SDL_CreateWindow(name.c_str(), x, y, RENDER_WIDTH, RENDER_HEIGHT, fullscreen ? SDL_WINDOW_FULLSCREEN : SDL_WINDOW_SHOWN);
        if (window)
            printf("Window intialised\n");
        else
            error;

        if (IMG_Init(IMG_INIT_PNG) & (IMG_INIT_PNG == IMG_INIT_PNG))
            printf("Image Initialised with PNG\n");
        else
            error_i;

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (renderer)
            printf("Renderer initialised\n");
        else
            error;
    }

    void button_action(int buttonID) {
        if (buttonID == BUTTON_START) current_screen = START_SCREEN;
        if (buttonID == BUTTON_LOAD) current_screen = LOAD_SCREEN;
        if (buttonID == BUTTON_OPTIONS) current_screen = OPTIONS_SCREEN;
        if (buttonID == BUTTON_HISCORE) current_screen = HISCORE_SCREEN;
        if (buttonID == BUTTON_QUIT) current_screen = QUIT_SCREEN;
        if (buttonID == BUTTON_BACK) current_screen = UI_SCREEN;
    }

    //AABB collision detection. Obsolete. I didn't use it anywhere
    bool collision(SDL_Rect* a, SDL_Rect* b) {
        if (a->x > b->x + b->w) return 0;
        if (a->x + a->w < b->x) return 0;
        if (a->y > b->y + b->h) return 0;
        if (a->y + a->w < b->y) return 0;
        return 1;
    }

    //after every player movement, focus camera on player head, and render a 800x600 region around player
    void camera_set(SDL_Rect* rect) {
        camera.x = rect->x + rect->w / 2 - RENDER_WIDTH / 2;
        camera.y = rect->y + rect->h / 2 - RENDER_HEIGHT / 2;
        if (camera.x < 0) camera.x = 0;
        if (camera.x + camera.w > SCREEN_WIDTH) camera.x = SCREEN_WIDTH - camera.w;
        if (camera.y < 0) camera.y = 0;
        if (camera.y + camera.h > SCREEN_HEIGHT) camera.y = SCREEN_HEIGHT - camera.h;
    }

    void close() {
        SDL_DestroyWindow(window);
        window = NULL;

        SDL_DestroyRenderer(renderer);
        renderer = NULL;

        SDL_Quit();
        IMG_Quit();
    }
};

Game* gGame = new Game(1280, 960);

//LTexture. Obsolete. Didn't use it anywhere
struct LTexture {
    SDL_Texture* texture;
    SDL_Rect pos;
    int width;
    int height;

    LTexture() {
        texture = NULL;
        width = 0;
        height = 0;
    }
    ~LTexture() {
        SDL_DestroyTexture(texture);
        width = 0;
        height = 0;
    }

    void loadFromFile(std::string path) {
        SDL_Surface* imgTemp = IMG_Load(path.c_str());
        if (imgTemp)
            printf("Image loaded from \"%s\"\n", path.c_str());
        else
            error_i;
        // if(imgTemp == NULL) error_i;

        texture = SDL_CreateTextureFromSurface(gGame->renderer, imgTemp);
        if (texture)
            printf("Texture Loaded\n");
        else
            error;
        // if(texture == NULL) error;

        width = pos.w = imgTemp->w;
        height = pos.h = imgTemp->h;
        SDL_FreeSurface(imgTemp);
    }

    void mapFromFile(std::string path, Uint8 r, Uint8 g, Uint8 b) {
        SDL_Surface* imgTemp = IMG_Load(path.c_str());
        if (imgTemp)
            printf(" Image loaded from %s\n", path.c_str());
        else
            error_i;

        SDL_SetColorKey(imgTemp, SDL_TRUE, SDL_MapRGB(imgTemp->format, r, g, b));

        texture = SDL_CreateTextureFromSurface(gGame->renderer, imgTemp);
        if (texture)
            printf(" Texture Loaded\n");
        else
            error;

        width = imgTemp->w;
        height = imgTemp->h;
        SDL_FreeSurface(imgTemp);
    }

    void setDest(int x, int y, int w, int h) {
        pos = {x, y, 50, 50};
    }

    void render() {
        // printf("\n...\nx %d y %d w %d h %d\n...\n", pos.x, pos.y, pos.w, pos.h);
        if (SDL_RenderCopy(gGame->renderer, texture, NULL, &pos) < 0) error;
    }
};

//buttons that doesn't work now lol
struct Button {
    LTexture button[3];
    SDL_Rect button_pos;
    int button_status;
    int buttonID;
    enum button_status {
        BUTTON_DEFAULT,
        BUTTON_HOVER,
        BUTTON_PRESSED
    };

    Button(int ID) {
        buttonID = ID;
    }
    ~Button() {
        button_status = BUTTON_DEFAULT;
    }

    void loadFromFile(std::string default_path, std::string hover_path, std::string pressed_path) {
        button[BUTTON_DEFAULT].loadFromFile(default_path);
        button[BUTTON_HOVER].loadFromFile(hover_path);
        button[BUTTON_PRESSED].loadFromFile(pressed_path);
    }

    void setDest(int x, int y, int w, int h) {
        // button_pos = {x, y, button[BUTTON_DEFAULT].width, button[BUTTON_DEFAULT].height };
        button_pos = {x, y, w, h};
        for (int i = 0; i < 3; i++) button[i].setDest(x, y, w, h);
    }

    void render(SDL_Event e) {
        event_handler(e);
        // SDL_RenderCopy( gGame->renderer, button[button_status].texture, NULL, &button_pos );
        button[button_status].render();
    }

    void event_handler(SDL_Event e) {
        int x, y;
        SDL_GetMouseState(&x, &y);

        if (x < button[BUTTON_DEFAULT].pos.x)
            button_status = BUTTON_DEFAULT;
        else if (x > button[BUTTON_DEFAULT].pos.x + button[BUTTON_DEFAULT].pos.w)
            button_status = BUTTON_DEFAULT;
        else if (y < button[BUTTON_DEFAULT].pos.y)
            button_status = BUTTON_DEFAULT;
        else if (y > button[BUTTON_DEFAULT].pos.y + button[BUTTON_DEFAULT].pos.h)
            button_status = BUTTON_DEFAULT;

        else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT && e.button.state == SDL_PRESSED) {
            button_status = BUTTON_PRESSED;
        } else if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT && e.button.state == SDL_RELEASED) {
            button_status = BUTTON_HOVER;
            gGame->button_action(buttonID);
        } else
            button_status = BUTTON_HOVER;
    }
};

//Tile information of the map being rendered. what tile goes where
struct Tile {
    SDL_Texture* tileImage;

    int tile_width, tile_height;
    int tile_type[30][40];

    //array par[i] associates i'th switch to i'th gate. for switch i, par[i] has it's gate's column and row
    struct Gate_switch_parent {
        int gate_row[5];
        int gate_col[5];
        int cnt;  //number of gates associated with switch

        Gate_switch_parent() {
            cnt = 0;
        }
    } par[10];

    enum tile_identity {
        TILE_WALL,
        TILE_SLOPE,
        TILE_WALK,
        TILE_BUTTON,
        TILE_GATE,
        TILE_TOTAL
    };

    SDL_Rect tiles[TILE_TOTAL];

    Tile(int w, int h) {
        tile_width = w;
        tile_height = h;

        //location of different tile types in my tilesheet. NEEDS TO BE CHANGED FOR NEW TILESHEET
        tiles[TILE_WALL] = {32, 0, 32, 32};
        tiles[TILE_SLOPE] = {0, 32, 32, 32};
        tiles[TILE_WALK] = {0, 0, 32, 32};
        tiles[TILE_BUTTON] = {64, 0, 32, 32};
        tiles[TILE_GATE] = {32, 32, 32, 32};
    }

    //loads tilesheep from file
    void loadImageFromFile(std::string path) {
        SDL_Surface* imgTemp = IMG_Load(path.c_str());
        if (imgTemp)
            printf("Image loaded from \"%s\"\n", path.c_str());
        else
            error_i;

        tileImage = SDL_CreateTextureFromSurface(gGame->renderer, imgTemp);
        if (tileImage)
            printf("tileimage Loaded\n");
        else
            error;

        SDL_FreeSurface(imgTemp);
    }

    //loads map info. What tile goes where. Which tile from the tilesheet goes where
    void loadInfoFromFile(std::string path) {
        std::ifstream map(path);

        if (!map.fail())
            printf("map loaded from path \"%s\"\n", path.c_str());
        else
            printf("Failed to load map\n");

        for (int i = 0; i < 30; i++) {
            for (int j = 0; j < 40; j++) {
                map >> tile_type[i][j];

                if (tile_type[i][j] % 100 == 4) {
                    int parent_gate = tile_type[i][j] / 100;
                    par[parent_gate].gate_row[par[parent_gate].cnt] = i;
                    par[parent_gate].gate_col[par[parent_gate].cnt++] = j;
                }
            }
        }
        map.close();
    }

    //detects and entity's collision with gate or wall. trims the entity's dimensions so it can fit into smaller spaces
    int tile_gate_wall_collission(SDL_Rect* a, int trim) {
        int topleft = tile_type[(a->y + trim) / tile_height][(a->x + trim) / tile_width] % 100;
        int topright = tile_type[(a->y + trim) / tile_height][(a->x + a->w - trim) / tile_width] % 100;
        int botleft = tile_type[(a->y + a->h - trim) / tile_height][(a->x + trim) / tile_width] % 100;
        int botright = tile_type[(a->y + a->h - trim) / tile_height][(a->x + a->w - trim) / tile_width] % 100;

        return ((topleft == TILE_WALL || topleft == TILE_SLOPE || topleft == TILE_GATE) ||
                (topright == TILE_WALL || topright == TILE_SLOPE || topright == TILE_GATE) ||
                (botleft == TILE_WALL || botleft == TILE_SLOPE || botleft == TILE_GATE) ||
                (botright == TILE_WALL || botright == TILE_SLOPE || botright == TILE_GATE));
    }

    //checks if an entity is colliding with some button
    int tile_button_collission(SDL_Rect* a) {
        if (tile_type[a->y / tile_height][a->x / tile_width] % 100 == TILE_BUTTON)
            return tile_type[a->y / tile_height][a->x / tile_width] / 100;

        if (tile_type[a->y / tile_height][(a->x + a->w) / tile_width] % 100 == TILE_BUTTON)
            return tile_type[a->y / tile_height][(a->x + a->w) / tile_width] / 100;

        if (tile_type[(a->y + a->h) / tile_height][a->x / tile_width] % 100 == TILE_BUTTON)
            return tile_type[(a->y + a->h) / tile_height][a->x / tile_width] / 100;

        if (tile_type[(a->y + a->h) / tile_height][(a->x + a->w) / tile_width] % 100 == TILE_BUTTON)
            return tile_type[(a->y + a->h) / tile_height][(a->x + a->w) / tile_width] / 100;

        else
            return 0;
    }

    //takes camera as argument. Renders all tiles that fit into the camera. Probably renders 1 tile along all directions outside the camera
    void render(SDL_Rect& camera) {
        int x, y;
        int renderX = 0, renderY = 0;

        //if camera border of the camera is in the middle of some tile, renders the tiles slightly outside the range of the camera
        int translateX = (camera.x) % tile_width, translateY = (camera.y) % tile_height;

        //inerates from left of the camera to right, and top to bottom to find all the tiles that fit within
        for (y = camera.y; y <= (camera.y + camera.h + tile_height); y += tile_height, renderY += tile_height, renderX = 0) {
            for (x = camera.x; x <= (camera.x + camera.w + tile_width); x += tile_width, renderX += tile_width) {
                SDL_Rect dest = {renderX - translateX, renderY - translateY, tile_width, tile_height};

                int type = tile_type[(int)(y / tile_height)][(int)(x / tile_width)] % 100;

                //renders walkable tile under all tiles. so gates and switches seems like they're on walkable tiles
                SDL_RenderCopy(gGame->renderer, tileImage, &tiles[TILE_WALK], &dest);

                float angle = 0.0;

                //makes sure dark side of the slope tile is facing the walkable tile
                //NEED TO CHANGE IF USING NEW TEXTUREPACK
                if (type == TILE_SLOPE) {
                    if (tile_type[(int)(y / tile_height)][(int)(x / tile_width) + 1] == TILE_WALL)
                        angle = 90.0;
                    else if (tile_type[(int)(y / tile_height)][(int)(x / tile_width) - 1] == TILE_WALL)
                        angle = 270.0;
                    else if (tile_type[(int)(y / tile_height) + 1][(int)(x / tile_width)] == TILE_WALL)
                        angle = 180.0;
                }
                // else if(type == TILE_GATE && tile_type [ (int)(y / tile_height) + 1] [ (int)(x / tile_width) ] <= TILE_SLOPE) angle = 90.0;
                else if (type == TILE_GATE) {
                    if ((tile_type[y / tile_height + 1][x / tile_width] == TILE_WALL) ||
                        (tile_type[y / tile_height + 1][x / tile_width] == TILE_SLOPE) ||
                        (tile_type[y / tile_height + 1][x / tile_width] == TILE_GATE) ||
                        (tile_type[y / tile_height - 1][x / tile_width] == TILE_WALL) ||
                        (tile_type[y / tile_height - 1][x / tile_width] == TILE_SLOPE) ||
                        (tile_type[y / tile_height - 1][x / tile_width] == TILE_GATE)) angle = 90.0;
                }

                SDL_RenderCopyEx(gGame->renderer, tileImage, &tiles[type], &dest, angle, NULL, SDL_FLIP_NONE);
            }
        }
    }
};

Tile gTile(32, 32);  //tiles are of dimensions 32x32px

//Player structure
struct Player {
    enum player_status_list {
        PLAYER_IDLE,
        PLAYER_MOVE,
        PLAYER_ATTACK,
        PLAYER_TOTAL_STATI
    };

    int xVel, yVel;                       //amout of pixels player displaces by every time it moves
    int frame;                            //the sprite from the spritesheet that will be renderer
    int slow_factor[PLAYER_TOTAL_STATI];  //player being slowed a little

    SDL_RendererFlip flip;

    //walk, attack, idle -each sprite sheet has different numbers of sprites per row and column
    int sprite_per_row[PLAYER_TOTAL_STATI];
    int sprite_per_col[PLAYER_TOTAL_STATI];
    int player_status;  //knows if the player is idle, walking or attacking

    bool attacking;  //player can't move if it is in attack animation

    SDL_Texture* look[PLAYER_TOTAL_STATI];  //look is player's texture. differnt spritesheet for walking attacking etc
    SDL_Rect src;                           //source rectangle from the spritesheet
    SDL_Rect dest;                          //destination rentangle in the map
    SDL_Point player_center;                //center of player's body. need it for line of sight

    Player(int w, int h) {
        xVel = 0;
        yVel = 0;
        frame = 0;
        attacking = 0;

        slow_factor[PLAYER_IDLE] = 7;
        slow_factor[PLAYER_MOVE] = 4;
        slow_factor[PLAYER_ATTACK] = 4;

        sprite_per_row[PLAYER_IDLE] = 6;
        sprite_per_col[PLAYER_IDLE] = 2;
        sprite_per_row[PLAYER_MOVE] = 6;
        sprite_per_col[PLAYER_MOVE] = 3;
        sprite_per_row[PLAYER_ATTACK] = 6;
        sprite_per_col[PLAYER_ATTACK] = 2;
        player_status = PLAYER_IDLE;

        dest = {0, 0, w, h};
        src = {0, 0, w, h};

        flip = SDL_FLIP_NONE;
    }
    ~Player() {
        xVel = yVel = 0;
    }

    //loading all the player sprites
    void loadFromFile(std::string idle_path, std::string move_path, std::string attack_path) {
        SDL_Surface* imgTemp = IMG_Load(idle_path.c_str());
        if (imgTemp)
            printf("Image loaded from \"%s\"\n", idle_path.c_str());
        else
            error_i;

        look[PLAYER_IDLE] = SDL_CreateTextureFromSurface(gGame->renderer, imgTemp);
        if (look[PLAYER_IDLE])
            printf("player texture Loaded\n");
        else
            error;

        imgTemp = IMG_Load(move_path.c_str());
        if (imgTemp)
            printf("Image loaded from \"%s\"\n", move_path.c_str());
        else
            error_i;

        look[PLAYER_MOVE] = SDL_CreateTextureFromSurface(gGame->renderer, imgTemp);
        if (look[PLAYER_MOVE])
            printf("player texture Loaded\n");
        else
            error;

        imgTemp = IMG_Load(attack_path.c_str());
        if (imgTemp)
            printf("Image loaded from \"%s\"\n", attack_path.c_str());
        else
            error_i;

        look[PLAYER_ATTACK] = SDL_CreateTextureFromSurface(gGame->renderer, imgTemp);
        if (look[PLAYER_ATTACK])
            printf("player texture Loaded\n");
        else
            error;

        SDL_FreeSurface(imgTemp);
        imgTemp = NULL;
    }

    //only used to check if enemy throwable is colliding with platyer
    bool rect_collission(SDL_Rect& rect) {
        if (rect.x + rect.w < dest.x) return 0;
        if (rect.x > dest.x + dest.w) return 0;
        if (rect.y + rect.h < dest.y) return 0;
        if (rect.y > dest.y + dest.h) return 0;
        return 1;
    }

    //set where the player will spawn for the level
    void set_spawn_point(int x, int y) {
        dest.x = x * dest.w;
        dest.y = y * dest.h;

        player_center = {x * dest.w + dest.w / 2, y * dest.h + dest.h / 2};
    }

    //moving player based on input
    void move() {
        frame++;
        frame %= (sprite_per_row[PLAYER_MOVE] * sprite_per_col[PLAYER_MOVE] * slow_factor[PLAYER_MOVE]);

        dest.x += xVel;
        if (dest.x < 0 || dest.x + dest.w > gGame->SCREEN_WIDTH || gTile.tile_gate_wall_collission(&dest, 7)) dest.x -= xVel;

        dest.y += yVel;
        if (dest.y < 0 || dest.y + dest.h > gGame->SCREEN_HEIGHT || gTile.tile_gate_wall_collission(&dest, 7)) dest.y -= yVel;

        player_center.x = dest.x + dest.w / 2;
        player_center.y = dest.y + dest.h / 2;
    }

    //player will soon have it's stas defined. Work in progress lol
    void get_damaged() {
    }

    void idle() {
        frame++;
        frame %= (sprite_per_row[PLAYER_IDLE] * sprite_per_col[PLAYER_IDLE] * slow_factor[PLAYER_IDLE]);
    }

    void attack() {
        frame++;
        if (frame >= (sprite_per_row[PLAYER_ATTACK] * sprite_per_col[PLAYER_ATTACK] * slow_factor[PLAYER_ATTACK])) frame = 0, player_status = PLAYER_IDLE, attacking = false;
    }

    //handles players actions based on human input. Takes argument SDL_Event
    void event_handler(SDL_Event& e) {
        static bool keyUpCheck = false;  //will only check for SDL_KEYUP if we already have had the event SDL_KEYDOWN

        if (!attacking && e.type == SDL_KEYDOWN && !e.key.repeat) {
            if (e.key.keysym.sym == SDLK_w) yVel = -6, player_status = PLAYER_MOVE, frame = 0, keyUpCheck = true;
            if (e.key.keysym.sym == SDLK_s) yVel = 6, player_status = PLAYER_MOVE, frame = 0, keyUpCheck = true;
            if (e.key.keysym.sym == SDLK_a) xVel = -6, player_status = PLAYER_MOVE, frame = 0, keyUpCheck = true, flip = SDL_FLIP_HORIZONTAL;
            if (e.key.keysym.sym == SDLK_d) xVel = 6, player_status = PLAYER_MOVE, frame = 0, keyUpCheck = true, flip = SDL_FLIP_NONE;

            if (e.key.keysym.sym == SDLK_e) {
                int tile_button_collission = gTile.tile_button_collission(&dest);

                //if player collides with a switch and pressed e, corresponding gate will open
                if (tile_button_collission) {
                    while (gTile.par[tile_button_collission].cnt) {
                        gTile.par[tile_button_collission].cnt--;
                        int gate_row = gTile.par[tile_button_collission].gate_row[gTile.par[tile_button_collission].cnt];
                        int gate_col = gTile.par[tile_button_collission].gate_col[gTile.par[tile_button_collission].cnt];
                        gTile.tile_type[gate_row][gate_col] = gTile.TILE_WALK;
                    }
                }
            }
        }

        //player can't move if it's in attack animation. Therefore the !attacking condition
        else if (!attacking && e.type == SDL_KEYUP && keyUpCheck)
            xVel = yVel = 0, player_status = PLAYER_IDLE, frame = 0, keyUpCheck = false;
        else if (!attacking && e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
            player_status = PLAYER_ATTACK, frame = 0, attacking = true;

        if (player_status == PLAYER_IDLE)
            idle();
        else if (player_status == PLAYER_MOVE)
            move();
        else if (player_status == PLAYER_ATTACK)
            attack();
    }

    void render() {
        //slowing player a little
        int current_frame = frame / slow_factor[player_status];

        //centering player on the screen
        dest.x -= gGame->camera.x;
        dest.y -= gGame->camera.y;

        //deciding which frame in the spritesheet to render
        src.x = (current_frame % sprite_per_row[player_status]) * src.w;
        src.y = (current_frame / sprite_per_row[player_status]) * src.h;

        SDL_RenderCopyEx(gGame->renderer, look[player_status], &src, &dest, 0.0, NULL, flip);

        //reverting player to it's original location onthe whole map
        dest.x += gGame->camera.x;
        dest.y += gGame->camera.y;
    }
};

Player gPlayer(32, 32);  //gPlayer size 32, 32, player being rendered at 34*32, 4*32 in the level

//Enemy structure
struct Enemy {
    enum enemy_status_list {
        ENEMY_PATROL,
        ENEMY_ATTACK,
        ENEMY_TOTAL
    };

    enum route {
        UP_DOWN,
        LEFT_RIGHT
    };

    //using bresenham's line drawing algorithm to find a straight line between player and enemy
    struct LineOfSight {
        int dx;  //change in x coordinate from enemy to player
        int dy;  //change in y coordinate from enemy to player
        int range;

        //starting point and endingpoint of the line
        SDL_Point start;
        SDL_Point end;

        //checking if player is in range of enemy
        bool in_range() {
            return dx * dx + dy * dy <= range;
        }

        //resetting enemy position and player position
        void reset(SDL_Point* player_center, SDL_Point* enemy_center) {
            start = *player_center;
            end = *enemy_center;

            dx = end.x - start.x;
            dy = end.y - start.y;
        }

        bool low_slope(SDL_Point& start, SDL_Point& end) {
            if (dx < 0) {
                dx = -dx;
                dy = -dy;
                return low_slope(end, start);
            }

            int y_increment = 1;
            int x_increment = 1;
            if (dy < 0) y_increment = -y_increment, dy = -dy;

            int decision_factor = 2 * dy - dx;

            while (start.x < end.x) {
                start.x += x_increment;
                if (decision_factor > 0)
                    start.y += y_increment, decision_factor += (2 * (dy - dx));
                else
                    decision_factor += (2 * dy);

                int tileType = gTile.tile_type[start.y / gTile.tile_height][start.x / gTile.tile_width];
                if ((tileType == gTile.TILE_WALL) || (tileType == gTile.TILE_SLOPE)) return 0;
            }
            return 1;
        }

        bool hi_slope(SDL_Point& start, SDL_Point& end) {
            if (dy < 0) {
                dx = -dx;
                dy = -dy;
                return hi_slope(end, start);
            }
            int x_increment = 1;
            int y_increment = 1;
            if (dx < 0) x_increment = -x_increment, dx = -dx;
            int decision_factor = 2 * dx - dy;

            while (start.y < end.y) {
                start.y += y_increment;

                if (decision_factor > 0)
                    start.x += x_increment, decision_factor += (2 * (dx - dy));
                else
                    decision_factor += (2 * dx);

                int tileType = gTile.tile_type[start.y / gTile.tile_height][start.x / gTile.tile_width];
                if ((tileType == gTile.TILE_WALL) || (tileType == gTile.TILE_SLOPE)) return 0;
            }
            return 1;
        }

        //returns 1 if it could establish an uninturrepted line between enemy and player
        bool established() {
            if (dx * dx + dy * dy > range) return 0;
            if (abs(dy) > abs(dx))
                return hi_slope(start, end);
            else
                return low_slope(start, end);
        }
    };

    int originX;  //origin where enemy was spawned. Enemy can move a little about this point
    int originY;
    int route_length;  //maximum displacenemt of enemy from spawn
    int xVel;          //displacement of enemy per loop when its patrolling
    int yVel;
    int vel;
    int enemy_status;                  //whether enemy is attacking
    int frame;                         //frame of enemy to be renderer
    int slow_factor[ENEMY_TOTAL];      //slowing enemy a little
    int sprites_per_row[ENEMY_TOTAL];  //enemy has different amount of sprites in each of it's spritesheet
    int sprites_per_col[ENEMY_TOTAL];

    Uint32 attack_range;  //range for enemy's attack
    Uint32 attack_delay;  //delay between two attacks

    int projectile_vel;      //magnitude of projectile's velocity vector
    double projectile_xVel;  //displacement of projectile per loop when launched
    double projectile_yVel;
    double projectile_angle;  //angle by which projectile rotates

    bool projectile_launched;  //whether enemy already launched a projectile

    SDL_Texture* look[ENEMY_TOTAL];  //different textures for different enemy status. attack patrol etc.
    SDL_Texture* projectile;         //texture for projectile. Enemy launches it's weapon
    SDL_Rect src;                    //source rectangle from the spritesheer
    SDL_Rect dest;                   //destination rectangle in the map
    SDL_Rect projectile_dest;        //destination rectangle in the map for projectile
    SDL_RendererFlip flip;           //enemy faces left when it's going/attacking to the left
    SDL_Point enemy_center;          //enemy's body center. Used for establishing line of sight
    SDL_Point projectile_center;     //Center of projectile. Obsolete. Don't delete
    LineOfSight line_of_sight;

    Enemy(int w, int h) {
        src = {0, 0, w, h};
        dest = {0, 0, w, h};
        projectile_dest = {0, 0, 24, 8};

        frame = 0;
        vel = 2;
        slow_factor[ENEMY_PATROL] = 6;
        slow_factor[ENEMY_ATTACK] = 6;
        route_length = 120;
        sprites_per_row[ENEMY_PATROL] = 6;
        sprites_per_col[ENEMY_PATROL] = 3;
        sprites_per_row[ENEMY_ATTACK] = 6;
        sprites_per_col[ENEMY_ATTACK] = 2;
        attack_range = 180;
        attack_delay = 4000;

        projectile_launched = false;
        projectile_vel = 2;
        projectile_xVel = 0.0;
        projectile_yVel = 0.0;
        projectile_angle = 0.0;

        line_of_sight.range = attack_range * attack_range;
        enemy_status = ENEMY_PATROL;
        flip = SDL_FLIP_NONE;
        for (int i = 0; i < ENEMY_TOTAL; i++) look[i] = NULL;
    }
    ~Enemy() {
        for (int i = 0; i < ENEMY_TOTAL; i++) {
            SDL_DestroyTexture(look[i]);
            look[i] = NULL;
        }
    }

    //loading all the textures for enemy
    void loadFromFile(std::string walk_path, std::string attack_path, std::string weapon_path) {
        SDL_Surface* tempImage = IMG_Load(walk_path.c_str());
        if (tempImage == NULL) error_i;

        look[ENEMY_PATROL] = SDL_CreateTextureFromSurface(gGame->renderer, tempImage);
        if (look[ENEMY_PATROL])
            printf("Enemy patrol Texture was loaded from \"%s\"\n", walk_path.c_str());
        else
            error;
        tempImage = NULL;

        tempImage = IMG_Load(attack_path.c_str());
        if (tempImage == NULL) error_i;

        look[ENEMY_ATTACK] = SDL_CreateTextureFromSurface(gGame->renderer, tempImage);
        if (look[ENEMY_ATTACK])
            printf("Enemy attack Texture was loaded from \"%s\"\n", attack_path.c_str());
        else
            error;
        tempImage = NULL;

        tempImage = IMG_Load(weapon_path.c_str());
        if (tempImage == NULL) error_i;

        projectile = SDL_CreateTextureFromSurface(gGame->renderer, tempImage);
        if (projectile)
            printf("Enemy weapon was loaded from \"%s\"\n", weapon_path.c_str());
        else
            error;

        SDL_FreeSurface(tempImage);
        tempImage = NULL;
    }

    //setting player spawn point
    void set_spawn(int x, int y) {
        originX = x * dest.w;
        originY = y * dest.h;

        enemy_center = {x * dest.w + dest.w / 2, y * dest.h + dest.h / 2};

        dest.x = originX;
        dest.y = originY;
    }

    //setting player patrol route. LEFT_RIGHT if along x axis, TOP_BOTTOM if alongy axis
    void set_route(bool route) {
        xVel = route * vel;
        yVel = !route * vel;
    }

    void move() {
        //moving projectile when enemy is in move stance
        if (projectile_launched) move_projectile();

        dest.x += xVel;
        if (dest.x < 0 || dest.x + dest.w > gGame->SCREEN_WIDTH || gTile.tile_gate_wall_collission(&dest, 0) || dest.x == originX + route_length || dest.x == originX) xVel = -xVel;

        dest.y += yVel;
        if (dest.y < 0 || dest.y + dest.h > gGame->SCREEN_HEIGHT || gTile.tile_gate_wall_collission(&dest, 0) || dest.y == originY + route_length || dest.y == originY) yVel = -yVel;

        enemy_center = {dest.x + dest.w / 2, dest.y + dest.h / 2};

        if (xVel > 0) flip = SDL_FLIP_NONE;
        if (xVel < 0) flip = SDL_FLIP_HORIZONTAL;
        frame++;
        frame %= (sprites_per_row[enemy_status] * sprites_per_col[enemy_status] * slow_factor[enemy_status]);

        //of projectile is not launched, projectile is on the enemy body
        if (!projectile_launched)
            projectile_dest.x = dest.x,
            projectile_dest.y = dest.y;
    }

    //makes projectile leave enemy body and go for the player
    void launch_projectile() {
        projectile_launched = true;
        projectile_center = enemy_center;
        const int dy = gPlayer.player_center.y - enemy_center.y;
        const int dx = gPlayer.player_center.x - enemy_center.x;
        double component_factor = atan((double)abs(dy) / (double)abs(dx));

        projectile_xVel = (dx > 0 ? 1.0 : -1.0) * (double)projectile_vel * cos(component_factor);
        projectile_yVel = (dy > 0 ? 1.0 : -1.0) * (double)projectile_vel * sin(component_factor);
    }

    //once projectile is launched, moves projectile towards plater
    void move_projectile() {
        static double projectile_xPos = 0;
        static double projectile_yPos = 0;

        projectile_center.x -= (int)projectile_xPos;
        projectile_xPos += projectile_xVel;

        projectile_center.y -= (int)projectile_yPos;
        projectile_yPos += projectile_yVel;

        projectile_angle += 20;
        if (projectile_angle >= 360.0) projectile_angle = 0;

        //if projectile has already travelled it's attack distance, it teleports back to enemybody
        if ((int)projectile_xPos * (int)projectile_xPos + (int)projectile_yPos * (int)projectile_yPos > attack_range * attack_range) {
            projectile_launched = false;
            projectile_xPos = 0;
            projectile_yPos = 0;
            return;
        }

        projectile_center.x += (int)projectile_xPos;
        projectile_center.y += (int)projectile_yPos;
        projectile_dest.x = projectile_center.x - projectile_dest.w / 2;
        projectile_dest.y = projectile_center.y - projectile_dest.h / 2;

        //if projectile collides with player, player will soon get damaged. Work in Progress
        if (gPlayer.rect_collission(projectile_dest)) {
            gPlayer.get_damaged();

            projectile_launched = false;
            projectile_xPos = 0;
            projectile_yPos = 0;
            return;
        }
        //if projectile collides with wall, it teleports back to enemy body
        else if (gTile.tile_gate_wall_collission(&projectile_dest, 0)) {
            projectile_launched = false;
            projectile_xPos = 0;
            projectile_yPos = 0;
            return;
        }
    }

    //player attack stance
    void attack() {
        static Uint32 start = SDL_GetTicks() - attack_delay;

        //launches a projectile after attack delay
        if (SDL_GetTicks() - start > attack_delay) launch_projectile(), start = SDL_GetTicks(), frame = 0;

        //moving projectile when player is in attack stance
        if (projectile_launched) move_projectile();
        if (frame >= 0) frame++;
        if (frame >= (sprites_per_row[enemy_status] * sprites_per_col[enemy_status] * slow_factor[enemy_status])) frame = -1;
    }

    //enemy responding to player events.
    void handle_event() {
        //resetting points to establish line of sight each time player or enemy moves
        line_of_sight.reset(&gPlayer.player_center, &enemy_center);

        //checks if it could establish a line of sight. Changes stance to attack if it could
        if (line_of_sight.established()) {
            if (enemy_status == ENEMY_PATROL) frame = 0;

            flip = enemy_center.x - gPlayer.player_center.x < 0 ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

            enemy_status = ENEMY_ATTACK;
        } else {  //else continues patrolling it's route
            if (enemy_status == ENEMY_ATTACK) frame = 0;
            enemy_status = ENEMY_PATROL;
        }

        if (enemy_status == ENEMY_PATROL)
            move();
        else if (enemy_status == ENEMY_ATTACK)
            attack();
    }

    //rendering enemy based on camera. Enemy will always be rendered no matter where camera is;
    void render(SDL_Rect& camera) {
        int current_frame = frame / slow_factor[enemy_status];
        src.x = (current_frame % sprites_per_row[enemy_status]) * src.w;
        src.y = (current_frame / sprites_per_row[enemy_status]) * src.h;

        int translateX = camera.x;
        int translateY = camera.y;

        //rendering enemy it it's own patrol route, even if not in camera range
        dest.x -= translateX;
        dest.y -= translateY;
        projectile_dest.x -= translateX;
        projectile_dest.y -= translateY;

        SDL_RenderCopyEx(gGame->renderer, look[enemy_status], &src, &dest, 0.0, NULL, flip);
        if (projectile_launched)  //if projectile is launched, renders projectile
            SDL_RenderCopyEx(gGame->renderer, projectile, NULL, &projectile_dest, projectile_angle, NULL, SDL_FLIP_NONE);

        dest.x += translateX;
        dest.y += translateY;
        projectile_dest.x += translateX;
        projectile_dest.y += translateY;
    }
};

Enemy gEnemy_one(32, 32);  //enemy is 32x32px in size
Enemy gEnemy_two(32, 32);  //enemy is 32x32px in size