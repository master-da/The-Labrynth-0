#include "screen.h"

void loadTile(Tile* tile, std::string tile_path, std::string file_path, int level_w, int level_h){
    tile->loadImageFromFile(tile_path);
    tile->loadInfoFromFile(file_path, level_w, level_h);
}

void loadPlayer(Player* player){
    player->loadFromFile("png/idle_player.png", "png/walk_player.png", "png/attack_player.png", "png/hurt_player.png","png/dying_player.png");
}

void loadEnemy(Enemy* enemy_one){
    enemy_one->loadFromFile("png/walk_enemy.png", "png/attack_enemy.png", "png/hurt_enemy.png", "png/dying_enemy.png", "png/weapon_enemy.png", "png/health_bar.png");
}

void level_one(Game* game){
    game->set_level_dimension(1280, 960);


    Tile* tile = new Tile(32, 32, game);
    loadTile(tile, "png/level-1-tiles.png", "map/level_1.txt", 1280, 960);

    Player* player = new Player(32, 32, game, tile);
    loadPlayer(player);
    player->set_spawn_point(34, 4);
    
    Enemy* enemy = new Enemy(32, 32, game, tile, player);
    loadEnemy(enemy);
    enemy->set_spawn(7, 11, enemy->LEFT_RIGHT);   

    SDL_Event e;

    while(game->game_running){
        SDL_PollEvent(&e);
        if(e.type == SDL_QUIT) game->game_running = false;

        SDL_RenderClear(game->renderer);
        SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);

        enemy->handle_event(e);
        player->handle_event(e);

        game->camera_set(&player->dest);

        tile->render(game->camera);
        enemy->render(game->camera);
        player->render();

        SDL_RenderPresent(game->renderer);
    }
}

void level_two(Game* game){
    game->set_level_dimension(1280, 960);

    Tile* tile = new Tile(32, 32, game);
    loadTile(tile, "png/level-1-tiles.png", "map/level_2.txt", 40, 30);

    Player* player = new Player(32, 32, game, tile);
    loadPlayer(player);
    player->set_spawn_point(34, 3);

    Enemy* enemy_one = new Enemy(32, 32, game, tile, player);
    loadEnemy(enemy_one);
    enemy_one->set_spawn(8, 9, enemy_one->UP_DOWN);   
    
    Enemy* enemy_two = new Enemy(32, 32, game, tile, player);
    loadEnemy(enemy_two);
    enemy_two->set_spawn(18, 21, enemy_two->LEFT_RIGHT);   

    SDL_Event e;

    while(game->game_running){
        SDL_PollEvent(&e);
        if(e.type == SDL_QUIT) game->game_running = false;

        SDL_RenderClear(game->renderer);
        SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);

        enemy_one->handle_event(e);
        enemy_two->handle_event(e);
        player->handle_event(e);

        game->camera_set(&player->dest);

        tile->render(game->camera);
        enemy_one->render(game->camera);
        enemy_two->render(game->camera);
        player->render();

        SDL_RenderPresent(game->renderer);
    }
}

void level_three(Game* game){
    game->set_level_dimension(2560, 1920);

    Tile* tile = new Tile(32, 32, game);
    loadTile(tile, "png/level-1-tiles.png", "map/level_3.txt", 80, 60);

    Player* player = new Player(32, 32, game, tile);
    loadPlayer(player);
    player->set_spawn_point(77, 3);

    SDL_Event e;

    while(game->game_running){
        SDL_PollEvent(&e);
        if(e.type == SDL_QUIT) game->game_running = false;

        SDL_RenderClear(game->renderer);
        SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);

        player->handle_event(e);

        game->camera_set(&player->dest);

        tile->render(game->camera);
        player->render();

        SDL_RenderPresent(game->renderer);
    }
}