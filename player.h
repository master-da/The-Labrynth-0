#pragma once

#include "game.h"
#include "init.h"
#include "tile.h"

struct Player {
    enum player_status_list {
        PLAYER_IDLE,
        PLAYER_MOVE,
        PLAYER_ATTACK,
        PLAYER_HURT,
        PLAYER_DYING,
        PLAYER_TOTAL_STATI
    };

    struct Player_stats {
        int hit_points;
        int defence;
        int attack_damage;
        int attack_radius;

        Player_stats() {
            hit_points = 20;
            defence = 10;
            attack_damage = 15;
            attack_radius = 24;
        }
    };

    int xVel, yVel;                       //amout of pixels player displaces by every time it moves
    int frame;                            //the sprite from the spritesheet that will be renderer
    int slow_factor[PLAYER_TOTAL_STATI];  //player being slowed a little

    SDL_RendererFlip flip;

    //walk, attack, idle -each sprite sheet has different numbers of sprites per row and column
    int sprite_per_row[PLAYER_TOTAL_STATI];
    int sprite_per_col[PLAYER_TOTAL_STATI];
    int player_status;  //knows if the player is idle, walking or attacking

    bool dead;  //player can't move if it is in attack animation

    Tile* tile;
    Game* game;
    Player_stats* stats = new Player_stats;

    SDL_Texture* look[PLAYER_TOTAL_STATI];  //look is player's texture. differnt spritesheet for walking attacking etc
    SDL_Rect src;                           //source rectangle from the spritesheet
    SDL_Rect dest;                          //destination rentangle in the map
    SDL_Point player_center;                //center of player's body. need it for line of sight
    Sint32 enemy_damage_event_code;

    Player(int w, int h, Game* inpGame, Tile* inpTile) {
        xVel = 0;
        yVel = 0;
        frame = 0;
        dead = false;
        tile = inpTile;
        game = inpGame;

        slow_factor[PLAYER_IDLE] = 7;
        slow_factor[PLAYER_MOVE] = 4;
        slow_factor[PLAYER_ATTACK] = 4;
        slow_factor[PLAYER_HURT] = 2;
        slow_factor[PLAYER_DYING] = 4;

        sprite_per_row[PLAYER_IDLE] = 6;
        sprite_per_col[PLAYER_IDLE] = 2;
        sprite_per_row[PLAYER_MOVE] = 6;
        sprite_per_col[PLAYER_MOVE] = 3;
        sprite_per_row[PLAYER_ATTACK] = 6;
        sprite_per_col[PLAYER_ATTACK] = 2;
        sprite_per_row[PLAYER_HURT] = 6;
        sprite_per_col[PLAYER_HURT] = 2;
        sprite_per_row[PLAYER_DYING] = 5;
        sprite_per_col[PLAYER_DYING] = 3;

        player_status = PLAYER_IDLE;

        dest = {0, 0, w, h};
        src = {0, 0, w, h};
        enemy_damage_event_code = 420;

        flip = SDL_FLIP_NONE;
    }
    ~Player() {
        xVel = yVel = 0;
    }

    //loading all the player sprites
    void loadFromFile(std::string idle_path, std::string move_path, std::string attack_path, std::string hurt_path, std::string dying_path) {
        SDL_Surface* imgTemp = IMG_Load(idle_path.c_str());
        if (imgTemp)
            printf("Image loaded from \"%s\"\n", idle_path.c_str());
        else
            error_i;

        look[PLAYER_IDLE] = SDL_CreateTextureFromSurface(game->renderer, imgTemp);
        if (look[PLAYER_IDLE])
            printf("player texture Loaded\n");
        else
            error;

        imgTemp = IMG_Load(move_path.c_str());
        if (imgTemp)
            printf("Image loaded from \"%s\"\n", move_path.c_str());
        else
            error_i;

        look[PLAYER_MOVE] = SDL_CreateTextureFromSurface(game->renderer, imgTemp);
        if (look[PLAYER_MOVE])
            printf("player texture Loaded\n");
        else
            error;

        imgTemp = IMG_Load(attack_path.c_str());
        if (imgTemp)
            printf("Image loaded from \"%s\"\n", attack_path.c_str());
        else
            error_i;

        look[PLAYER_ATTACK] = SDL_CreateTextureFromSurface(game->renderer, imgTemp);
        if (look[PLAYER_ATTACK])
            printf("player texture Loaded\n");
        else
            error;

        imgTemp = IMG_Load(hurt_path.c_str());
        if (imgTemp)
            printf("Image loaded from \"%s\"\n", hurt_path.c_str());
        else
            error_i;

        look[PLAYER_HURT] = SDL_CreateTextureFromSurface(game->renderer, imgTemp);
        if (look[PLAYER_HURT])
            printf("player texture Loaded\n");
        else
            error;

        imgTemp = IMG_Load(dying_path.c_str());
        if (imgTemp)
            printf("Image loaded from \"%s\"\n", dying_path.c_str());
        else
            error_i;

        look[PLAYER_DYING] = SDL_CreateTextureFromSurface(game->renderer, imgTemp);
        if (look[PLAYER_DYING])
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

    bool center_rect_collision(SDL_Rect& rectangle) {
        int x = player_center.x;
        int y = player_center.y;
        int r = stats->attack_radius;

        if (player_center.x < rectangle.x)
            x = rectangle.x;
        else if (player_center.x > rectangle.x + rectangle.w)
            x = rectangle.x + rectangle.w;

        if (player_center.y < rectangle.y)
            x = rectangle.y;
        else if (player_center.y > rectangle.y + rectangle.h)
            y = rectangle.y + rectangle.w;

        x = player_center.x - x;
        y = player_center.y - y;

        int distance = x * x + y * y;
        return (distance <= r * r);
    }

    //set where the player will spawn for the level
    void set_spawn_point(int x, int y) {
        dest.x = x * dest.w;
        dest.y = y * dest.h;

        player_center = {x * dest.w + dest.w / 2, y * dest.h + dest.h / 2};
    }

    //moving player based on input.
    void move() {
        frame++;
        frame %= (sprite_per_row[PLAYER_MOVE] * sprite_per_col[PLAYER_MOVE] * slow_factor[PLAYER_MOVE]);

        dest.x += xVel;
        if (dest.x < 0 || dest.x + dest.w > game->SCREEN_WIDTH || tile->tile_gate_wall_collission(&dest, 7)) dest.x -= xVel;

        dest.y += yVel;
        if (dest.y < 0 || dest.y + dest.h > game->SCREEN_HEIGHT || tile->tile_gate_wall_collission(&dest, 7)) dest.y -= yVel;

        player_center.x = dest.x + dest.w / 2;
        player_center.y = dest.y + dest.h / 2;
    }

    //player will soon have it's stas defined. Work in progress lol
    void get_damaged() {
        frame++;
        if (frame > (sprite_per_row[PLAYER_HURT] * sprite_per_col[PLAYER_HURT] * slow_factor[PLAYER_HURT])) player_status = PLAYER_IDLE, frame = 0;
    }

    void idle() {
        frame++;
        frame %= (sprite_per_row[PLAYER_IDLE] * sprite_per_col[PLAYER_IDLE] * slow_factor[PLAYER_IDLE]);
    }

    void attack() {
        if(!frame){

            
            // printf("here %d %d %d %d\n", attack_rect.x, attack_rect.y, attack_rect.w, attack_rect.h);

            SDL_Event enemy_damage_event;
            SDL_memset(&enemy_damage_event, 0, sizeof(enemy_damage_event));
            enemy_damage_event.type = SDL_RegisterEvents(1);

            if(enemy_damage_event.type == (Uint32)-1) error
            else {
                enemy_damage_event.user.code = game->enemy_damaged;
                SDL_PushEvent(&enemy_damage_event);
            }
        }

        frame++;
        if (frame >= (sprite_per_row[PLAYER_ATTACK] * sprite_per_col[PLAYER_ATTACK] * slow_factor[PLAYER_ATTACK])) {
            frame = 0;
            player_status = PLAYER_IDLE;
            return;
        }
    }

    void die() {
        frame++;
        if (frame >= (sprite_per_row[PLAYER_DYING] * sprite_per_col[PLAYER_DYING] * slow_factor[PLAYER_DYING])) frame--, dead = true;
    }

    //handles players actions based on human input. Takes argument SDL_Event to check human activity. And tile pointer to check button activity
    void event_handler(SDL_Event& e) {
        if (dead) return;

        static bool keyUpCheck = false;  //will only check for SDL_KEYUP if we already have had the event SDL_KEYDOWN

        if (player_status < PLAYER_ATTACK && e.type == SDL_KEYDOWN && !e.key.repeat) {
            if (e.key.keysym.sym == SDLK_w) yVel = -6, player_status = PLAYER_MOVE, frame = 0, keyUpCheck = true;
            if (e.key.keysym.sym == SDLK_s) yVel = 6, player_status = PLAYER_MOVE, frame = 0, keyUpCheck = true;
            if (e.key.keysym.sym == SDLK_a) xVel = -6, player_status = PLAYER_MOVE, frame = 0, keyUpCheck = true, flip = SDL_FLIP_HORIZONTAL;
            if (e.key.keysym.sym == SDLK_d) xVel = 6, player_status = PLAYER_MOVE, frame = 0, keyUpCheck = true, flip = SDL_FLIP_NONE;

            if (e.key.keysym.sym == SDLK_e) {
                int tile_button_collission = tile->tile_button_collission(&dest);

                //if player collides with a switch and pressed e, corresponding gate will open
                if (tile_button_collission) {
                    while (tile->par[tile_button_collission].cnt) {
                        tile->par[tile_button_collission].cnt--;
                        int gate_row = tile->par[tile_button_collission].gate_row[tile->par[tile_button_collission].cnt];
                        int gate_col = tile->par[tile_button_collission].gate_col[tile->par[tile_button_collission].cnt];
                        tile->tile_type[gate_row][gate_col] = tile->TILE_WALK;
                    }
                }
            }
        }

        //player can't move if it's not idle or moving alreadt. Therefore the < PLAYER_ATTACK condition
        else if (player_status < PLAYER_ATTACK && e.type == SDL_KEYUP && keyUpCheck)
            xVel = yVel = 0, player_status = PLAYER_IDLE, frame = 0, keyUpCheck = false;
        else if (player_status < PLAYER_ATTACK && e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
            player_status = PLAYER_ATTACK, frame = 0;
        else if (e.user.code == game->player_damaged) {

            player_status = PLAYER_HURT, frame = 0;
            e.user.code = 2;
            // stats->hit_points -= *((int*)e.user.data1);
            if (stats->hit_points <= 0) player_status = PLAYER_DYING;
            
        }

        if (player_status == PLAYER_IDLE)
            idle();
        else if (player_status == PLAYER_MOVE)
            move();
        else if (player_status == PLAYER_ATTACK)
            attack();
        else if (player_status == PLAYER_HURT)
            get_damaged();
        else if (player_status == PLAYER_DYING)
            die();
    }

    void render() {
        //slowing player a little
        int current_frame = frame / slow_factor[player_status];

        //centering player on the screen
        dest.x -= game->camera.x;
        dest.y -= game->camera.y;

        //deciding which frame in the spritesheet to render
        src.x = (current_frame % sprite_per_row[player_status]) * src.w;
        src.y = (current_frame / sprite_per_row[player_status]) * src.h;

        SDL_RenderCopyEx(game->renderer, look[player_status], &src, &dest, 0.0, NULL, flip);

        //reverting player to it's original location onthe whole map
        dest.x += game->camera.x;
        dest.y += game->camera.y;
    }
};
