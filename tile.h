#pragma once

#include "init.h"
#include "game.h"


struct Tile {
    SDL_Texture* tileImage;

    int tile_width, tile_height;
    int tile_type[30][40];

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

    enum tile_identity {
        TILE_WALL,
        TILE_SLOPE,
        TILE_WALK,
        TILE_BUTTON,
        TILE_GATE,
        TILE_TOTAL
    };

    SDL_Rect tiles[TILE_TOTAL];

    Tile(int w, int h, Game* inpGame) {
        tile_width = w;
        tile_height = h;

        game = inpGame;

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

        tileImage = SDL_CreateTextureFromSurface(game->renderer, imgTemp);
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
                SDL_RenderCopy(game->renderer, tileImage, &tiles[TILE_WALK], &dest);

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

                SDL_RenderCopyEx(game->renderer, tileImage, &tiles[type], &dest, angle, NULL, SDL_FLIP_NONE);
            }
        }
    }
};