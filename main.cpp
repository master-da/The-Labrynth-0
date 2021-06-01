#include "init.h"
#include "game.h"
#include "tile.h"
#include "player.h"
#include "enemy.h"


void loadmedia(Tile* tile, Player* player, Enemy* enemy_one, Enemy* enemy_two) {
    tile->loadImageFromFile("png/level-1-tiles.png");
    tile->loadInfoFromFile("map/mapped.txt ");
    player->loadFromFile("png/idle_player.png", "png/walk_player.png", "png/attack_player.png", "png/hurt_player.png","png/dying_player.png");
    enemy_one->loadFromFile("png/walk_enemy.png", "png/attack_enemy.png", "png/hurt_enemy.png", "png/dying_enemy.png", "png/weapon_enemy.png");
    enemy_two->loadFromFile("png/walk_enemy.png", "png/attack_enemy.png", "png/hurt_enemy.png", "png/dying_enemy.png", "png/weapon_enemy.png");
}

int main(int argc, char* argv[]) {

    Game* game = new Game(1280, 960);
    Tile* tile = new Tile(32, 32, game);
    Player* player = new Player(32, 32, game, tile);
    Enemy* enemy_one = new Enemy(32, 32, game, tile, player);
    Enemy* enemy_two = new Enemy(32, 32, game, tile, player);


    game->init("a", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0);
    SDL_Event e;
    bool quit = false;
    loadmedia(tile, player, enemy_one, enemy_two);

    player->set_spawn_point(34, 4);

    enemy_one->set_spawn(22, 3);
    enemy_one->set_route(enemy_one->UP_DOWN);

    enemy_two->set_spawn(17, 21);
    enemy_two->set_route(enemy_two->LEFT_RIGHT);

    while (!quit) {
        SDL_PollEvent(&e);
        if (e.type == SDL_QUIT) quit = 1;

        SDL_RenderClear(game->renderer);
        SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);

        enemy_one->handle_event(e);
        enemy_two->handle_event(e);
        player->event_handler(e);

        game->camera_set(&player->dest);

        tile->render(game->camera);
        enemy_one->render(game->camera);
        enemy_two->render(game->camera);
        player->render();

        SDL_RenderPresent(game->renderer);
    }

    game->close();
    return 0;
}