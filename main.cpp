#include "game.h"

void loadmedia() {
    gPlayer.loadFromFile("png/idle_player.png", "png/walk_player.png", "png/attack_player.png");
    gEnemy_one.loadFromFile("png/walk_enemy.png", "png/attack_enemy.png", "png/weapon_enemy.png");
    gEnemy_two.loadFromFile("png/walk_enemy.png", "png/attack_enemy.png", "png/weapon_enemy.png");
    gTile.loadImageFromFile("png/level-1-tiles.png");
    gTile.loadInfoFromFile("map/mapped.txt ");
}

int main(int argc, char* argv[]) {
    gGame->init("a", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0);
    SDL_Event e;
    bool quit = false;
    loadmedia();

    gPlayer.set_spawn_point(34, 4);

    gEnemy_one.set_spawn(8, 10);
    gEnemy_one.set_route(gEnemy_one.UP_DOWN);

    gEnemy_two.set_spawn(17, 21);
    gEnemy_two.set_route(gEnemy_two.LEFT_RIGHT);

    while (!quit) {
        SDL_PollEvent(&e);
        if (e.type == SDL_QUIT) quit = 1;

        SDL_RenderClear(gGame->renderer);
        SDL_SetRenderDrawColor(gGame->renderer, 0, 0, 0, 255);

        gPlayer.event_handler(e);
        gEnemy_one.handle_event();
        gEnemy_two.handle_event();

        gGame->camera_set(&gPlayer.dest);

        gTile.render(gGame->camera);
        gEnemy_one.render(gGame->camera);
        gEnemy_two.render(gGame->camera);
        gPlayer.render();

        SDL_RenderPresent(gGame->renderer);
    }

    gGame->close();
    return 0;
}