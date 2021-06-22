#pragma once

#include "init.h"
#include "game.h"


struct Tile {

    enum tile_identity {
        TILE_WALL0,
        TILE_WALL1,
        TILE_WALL2,
        TILE_WALL3,
        TILE_WALL4,

        TILE_WALK0,
        TILE_WALK1,
        TILE_WALK2,
        TILE_WALK3,
        TILE_WALK4,

        TILE_BUTTON_DEFAULT,
        TILE_BUTTON_TRIGGERED,

        TILE_GATE_DEFAULT,
        TILE_GATE_OPEN,

        TILE_CHEST_DEFAULT,
        TILE_CHEST_OPEN,

        TILE_TOTAL
    };

    SDL_Texture* tile_image;
    SDL_Texture* chest_image;
    SDL_Texture* button_image;
    SDL_Texture* gate_image;

    SDL_Rect tiles[TILE_TOTAL];

    int tile_width, tile_height;
    int tile_type[90][120];
    int slow_factor;

    Game* game;

    //array par[i] associates i'th switch to i'th gate. for switch i, par[i] has it's gate's column and row
    struct Gate_switch_parent {
        int gate_row[5];
        int gate_col[5];
        int cnt;  //number of gates associated with switch

        Gate_switch_parent() {
            cnt = 0;
        }
    } par[10];

    Tile(int w, int h, Game* game_) {
        tile_width = w;
        tile_height = h;
        slow_factor = 6;

        game = game_;

        //location of different tile types in my tilesheet. NEEDS TO BE CHANGED FOR NEW TILESHEET
        tiles[TILE_WALL0] = {32, 0, 32, 32};
        tiles[TILE_WALL1] = {0, 32, 32, 32};

        tiles[TILE_WALK0] = {0, 0, 32, 32};

        tiles[TILE_BUTTON_DEFAULT] = {64, 0, 32, 32};
        tiles[TILE_BUTTON_TRIGGERED] = {64, 0, 32, 32};

        tiles[TILE_GATE_DEFAULT] = {32, 32, 32, 32};
        tiles[TILE_GATE_OPEN] = {32, 32, 32, 32};

        tiles[TILE_CHEST_DEFAULT] = {0, 0, 42, 32};
        tiles[TILE_CHEST_OPEN] = {42, 0, 42, 32};
        
    }

    //loads tilesheep from file
    void loadImageFromFile(std::string path) {

        char chest_path[] = "png/chest.png";

        SDL_Surface* imgTemp = IMG_Load(path.c_str());
        if (imgTemp == NULL) error_i;
        tile_image = SDL_CreateTextureFromSurface(game->renderer, imgTemp);
        if (tile_image == NULL) error;


        imgTemp = IMG_Load(chest_path);
        if(imgTemp == NULL) error_i 
        chest_image = SDL_CreateTextureFromSurface(game->renderer, imgTemp);
        if(chest_image == NULL) error;

        SDL_FreeSurface(imgTemp);
    }

    //loads map info. What tile goes where. Which tile from the tilesheet goes where
    void loadInfoFromFile(std::string path, int w, int h) {
        std::ifstream map(path);

        if (!map.fail())
            printf("map loaded from path \"%s\"\n", path.c_str());
        else
            printf("Failed to load map\n");

        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                map >> tile_type[i][j];

                if (tile_type[i][j] % 100 == TILE_GATE_DEFAULT) {
                    int parent_gate = tile_type[i][j] / 100;
                    par[parent_gate].gate_row[par[parent_gate].cnt] = i;
                    par[parent_gate].gate_col[par[parent_gate].cnt++] = j;
                }
            }
        }
        map.close();
    }

    //detects and entity's collision with gate or wall. trims the entity's dimensions so it can fit into smaller spaces
    bool tile_gate_wall_collission(SDL_Rect* a, int trim) {
        int topleft = tile_type[(a->y + trim) / tile_height][(a->x + trim) / tile_width] % 100;
        int topright = tile_type[(a->y + trim) / tile_height][(a->x + a->w - trim) / tile_width] % 100;
        int botleft = tile_type[(a->y + a->h - trim) / tile_height][(a->x + trim) / tile_width] % 100;
        int botright = tile_type[(a->y + a->h - trim) / tile_height][(a->x + a->w - trim) / tile_width] % 100;

        return ((topleft <= TILE_WALL4 || topleft == TILE_GATE_DEFAULT) ||
                (topright <= TILE_WALL4 || topright == TILE_GATE_DEFAULT) ||
                (botleft <= TILE_WALL4 || botleft == TILE_GATE_DEFAULT) ||
                (botright <= TILE_WALL4 || botright == TILE_GATE_DEFAULT));
    }

    //detects and entity's collision with wall. trims the entity's dimensions so it can fit into smaller spaces
    bool tile_wall_collission(SDL_Rect* a, int trim) {
        int topleft = tile_type[(a->y + trim) / tile_height][(a->x + trim) / tile_width] % 100;
        int topright = tile_type[(a->y + trim) / tile_height][(a->x + a->w - trim) / tile_width] % 100;
        int botleft = tile_type[(a->y + a->h - trim) / tile_height][(a->x + trim) / tile_width] % 100;
        int botright = tile_type[(a->y + a->h - trim) / tile_height][(a->x + a->w - trim) / tile_width] % 100;

        return (topleft <= TILE_WALL4 ||
                topright <= TILE_WALL4 ||
                botleft <= TILE_WALL4 ||
                botright <= TILE_WALL4);
    }

    bool tile_chest_collission(SDL_Rect* a, int trim) {
        int topleft = tile_type[(a->y + trim) / tile_height][(a->x + trim) / tile_width] % 100;
        int topright = tile_type[(a->y + trim) / tile_height][(a->x + a->w - trim) / tile_width] % 100;
        int botleft = tile_type[(a->y + a->h - trim) / tile_height][(a->x + trim) / tile_width] % 100;
        int botright = tile_type[(a->y + a->h - trim) / tile_height][(a->x + a->w - trim) / tile_width] % 100;

        return (topleft == TILE_CHEST_DEFAULT || topleft == TILE_CHEST_OPEN ||
                topright == TILE_CHEST_DEFAULT || topright == TILE_CHEST_OPEN ||
                botleft == TILE_CHEST_DEFAULT || botleft == TILE_CHEST_OPEN ||
                botright == TILE_CHEST_DEFAULT || botright == TILE_CHEST_OPEN);
    }

    SDL_Point tile_chest_contact(SDL_Rect* a){
        int x = (a->x + a->w / 2) / tile_width;
        int y = (a->y + a->h / 2) / tile_width;

        // printf("x %d y %d\n", x, y);

        if(tile_type[y][x+1] == TILE_CHEST_DEFAULT) return {x+1, y};
        else if(tile_type[y][x-1] == TILE_CHEST_DEFAULT) return {x-1, y};
        else if(tile_type[y+1][x] == TILE_CHEST_DEFAULT) return {x, y+1};
        else if(tile_type[y-1][x] == TILE_CHEST_DEFAULT) return {x, y-1};
        else if(tile_type[y+1][x+1] == TILE_CHEST_DEFAULT) return {x+1, y+1};
        else if(tile_type[y+1][x-1] == TILE_CHEST_DEFAULT) return {x-1, y+1};
        else if(tile_type[y-1][x+1] == TILE_CHEST_DEFAULT) return {x+1, y-1};
        else if(tile_type[y-1][x-1] == TILE_CHEST_DEFAULT) return {x-1, y-1};
        else return {-1, 0};
    }

    //checks if an entity is colliding with some button
    int tile_button_collission(SDL_Rect* a) {
        if (tile_type[a->y / tile_height][a->x / tile_width] % 100 == TILE_BUTTON_DEFAULT)
            return tile_type[a->y / tile_height][a->x / tile_width] / 100;

        if (tile_type[a->y / tile_height][(a->x + a->w) / tile_width] % 100 == TILE_BUTTON_DEFAULT)
            return tile_type[a->y / tile_height][(a->x + a->w) / tile_width] / 100;

        if (tile_type[(a->y + a->h) / tile_height][a->x / tile_width] % 100 == TILE_BUTTON_DEFAULT)
            return tile_type[(a->y + a->h) / tile_height][a->x / tile_width] / 100;

        if (tile_type[(a->y + a->h) / tile_height][(a->x + a->w) / tile_width] % 100 == TILE_BUTTON_DEFAULT)
            return tile_type[(a->y + a->h) / tile_height][(a->x + a->w) / tile_width] / 100;

        else
            return 0;
    }

    bool tile_endgame_collission(SDL_Rect* a, int trim) {
        int topleft = tile_type[(a->y + trim) / tile_height][(a->x + trim) / tile_width];
        int topright = tile_type[(a->y + trim) / tile_height][(a->x + a->w - trim) / tile_width];
        int botleft = tile_type[(a->y + a->h - trim) / tile_height][(a->x + trim) / tile_width];
        int botright = tile_type[(a->y + a->h - trim) / tile_height][(a->x + a->w - trim) / tile_width];
        return (topleft == 9999 ||
                topright == 9999 ||
                botleft == 9999 ||
                botright == 9999);
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
                SDL_RenderCopy(game->renderer, tile_image, &tiles[TILE_WALK0], &dest);

                float angle = 0.0;

                //makes sure dark side of the slope tile is facing the walkable tile
                //NEED TO CHANGE IF USING NEW TEXTUREPACK
                if (type <= TILE_WALL4) {
                    if (tile_type[(int)(y / tile_height)][(int)(x / tile_width) + 1] == TILE_WALL1)
                        angle = 90.0;
                    else if (tile_type[(int)(y / tile_height)][(int)(x / tile_width) - 1] == TILE_WALL1)
                        angle = 270.0;
                    else if (tile_type[(int)(y / tile_height) + 1][(int)(x / tile_width)] == TILE_WALL1)
                        angle = 180.0;
                    SDL_RenderCopyEx(game->renderer, tile_image, &tiles[type], &dest, angle, NULL, SDL_FLIP_NONE);
                }

                else if(type <= TILE_WALK4)
                    SDL_RenderCopyEx(game->renderer, tile_image, &tiles[type], &dest, angle, NULL, SDL_FLIP_NONE);

                else if(type <= TILE_BUTTON_TRIGGERED)
                    SDL_RenderCopyEx(game->renderer, tile_image, &tiles[type], &dest, angle, NULL, SDL_FLIP_NONE);

                else if (type <= TILE_GATE_OPEN) {
                    if ((tile_type[y / tile_height + 1][x / tile_width] <= TILE_WALL1) ||
                        (tile_type[y / tile_height + 1][x / tile_width] == TILE_GATE_OPEN) ||
                        (tile_type[y / tile_height + 1][x / tile_width] == TILE_GATE_DEFAULT) ||
                        (tile_type[y / tile_height - 1][x / tile_width] <= TILE_WALL1) ||
                        (tile_type[y / tile_height - 1][x / tile_width] == TILE_GATE_OPEN) ||
                        (tile_type[y / tile_height - 1][x / tile_width] == TILE_GATE_DEFAULT)) angle = 90.0;
                    SDL_RenderCopyEx(game->renderer, tile_image, &tiles[type], &dest, angle, NULL, SDL_FLIP_NONE);
                }

                else if(type <= TILE_CHEST_OPEN){
                    SDL_RenderCopy(game->renderer, chest_image, &tiles[type], &dest);
                }
            }
        }
    }
};