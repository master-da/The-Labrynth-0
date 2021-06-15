#include "screen.h"

void UI(Game* game){

    SDL_Texture* logo;
    SDL_Surface* imgTemp = IMG_Load("png/logo.png");
    if(imgTemp == NULL) error_i
    logo = SDL_CreateTextureFromSurface(game->renderer, imgTemp);
    if(logo == NULL) error;

    SDL_Rect logodest = {(game->RENDER_WIDTH - imgTemp->w)/2, 20, imgTemp->w, imgTemp->h};

    SDL_FreeSurface(imgTemp);
    imgTemp = NULL;


    Button* button_start = new Button(game->BUTTON_START, game->BUTTON_REGULAR, game);
    button_start->loadFromFile("png/buttons/button_start.png");
    button_start->set_dest(250, 150);

    Button* button_load = new Button(game->BUTTON_LOAD, game->BUTTON_REGULAR, game);
    button_load->loadFromFile("png/buttons/button_load.png");
    button_load->set_dest(250, 225);

    Button* button_options = new Button(game->BUTTON_OPTIONS, game->BUTTON_REGULAR, game);
    button_options->loadFromFile("png/buttons/button_options.png");
    button_options->set_dest(250, 300);

    Button* buttont_hiscore = new Button(game->BUTTON_HISCORE, game->BUTTON_REGULAR, game);
    buttont_hiscore->loadFromFile("png/buttons/button_hiscore.png");
    buttont_hiscore->set_dest(250, 375);

    Button* button_quit = new Button(game->BUTTON_QUIT, game->BUTTON_REGULAR, game);
    button_quit->loadFromFile("png/buttons/button_quit.png");
    button_quit->set_dest(250, 450);

    Background* ui_bg = new Background(game);
    ui_bg->loadFromFile("png/background/layer_0.png", "png/background/layer_1.png", "png/background/layer_2.png");
    ui_bg->set_vel();
    ui_bg->set_rect();
    ui_bg->set_width();

    SDL_Event e;
    while(game->game_running && game->current_screen == game->UI_SCREEN){
        SDL_PollEvent(&e);
        if(e.type == SDL_QUIT) game->game_running = false;

        SDL_RenderClear(game->renderer);
        SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);


        button_start->handle_event(e);
        button_load->handle_event(e);
        button_options->handle_event(e);
        buttont_hiscore->handle_event(e);
        button_quit->handle_event(e);

        ui_bg->render();
        SDL_RenderCopy(game->renderer, logo, NULL, &logodest);

        button_start->render();
        button_load->render();
        button_options->render();
        buttont_hiscore->render();
        button_quit->render();

        SDL_RenderPresent(game->renderer);
    }

    SDL_DestroyTexture(logo);
    logo = NULL;
}

void pause_screen(Game* game, int score_){

    Button* button_home = new Button(game->BUTTON_HOME, game->BUTTON_REGULAR, game);
    button_home->loadFromFile("png/buttons/button_credit.png");
    button_home->set_dest(50, 200);

    Button* button_continue = new Button(game->BUTTON_CONTINUE, game->BUTTON_REGULAR, game);
    button_continue->loadFromFile("png/buttons/button_load.png");
    button_continue->set_dest(450, 200);

    Score* score = new Score(game, score_);
    score->set_height(100);

    SDL_Event e;

    while(game->game_running && game->game_pause){
        SDL_PollEvent(&e);
        if(e.type == SDL_QUIT) game->game_running = false;

        SDL_RenderClear(game->renderer);
        SDL_SetRenderDrawColor(game->renderer, 255, 255, 255, 255);

        // const Uint8* key = SDL_GetKeyboardState(NULL);
        // if(key[SDL_SCANCODE_ESCAPE]) game->game_pause = false;

        button_home->handle_event(e);
        button_continue->handle_event(e);

        score->render();
        button_home->render();
        button_continue->render();

        SDL_RenderPresent(game->renderer);
    }

}

void level_end_screen(Game* game, int score_, int on_level){
    Button* button_home = new Button(game->BUTTON_HOME, game->BUTTON_REGULAR, game);
    button_home->loadFromFile("png/buttons/button_credit.png");
    button_home->set_dest(50, 450);

    Button* button_next = new Button(game->BUTTON_NEXT, game->BUTTON_REGULAR, game);
    button_next->loadFromFile("png/buttons/button_load.png");
    button_next->set_dest(450, 450);

    Score* score = new Score(game, score_);
    score->set_height(100);

    SDL_Event e;

    while(game->game_running && game->current_screen == on_level){
        SDL_PollEvent(&e);
        if(e.type == SDL_QUIT) game->game_running = false;

        SDL_RenderClear(game->renderer);
        SDL_SetRenderDrawColor(game->renderer, 255, 255, 255, 255);
        
        button_home->handle_event(e);
        button_next->handle_event(e);

        score->render();
        button_home->render();
        button_next->render();

        SDL_RenderPresent(game->renderer);
    }
}

void level_choice(){

}

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
    game->level_end = false;
    game->set_level_dimension(1280, 960);

    Tile* tile = new Tile(32, 32, game);
    loadTile(tile, "png/level-1-tiles.png", "map/level_1.txt", 40, 30);

    Player* player = new Player(32, 32, game, tile);
    loadPlayer(player);
    player->set_spawn_point(34, 4);
    
    Enemy* enemy = new Enemy(32, 32, game, tile, player);
    loadEnemy(enemy);
    enemy->set_spawn(30, 25, enemy->LEFT_RIGHT);   

    SDL_Event e;

    while(game->game_running && game->current_screen == game->LEVEL_1){
        SDL_PollEvent(&e);
        if(e.type == SDL_QUIT) game->game_running = false;

        const Uint8* key = SDL_GetKeyboardState(NULL);
        if(key[SDL_SCANCODE_ESCAPE]) game->game_pause = true;

        if(game->game_pause) pause_screen(game, player->stats->score);

        SDL_RenderClear(game->renderer);
        SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);

        enemy->handle_event(e);
        player->handle_event(e);

        if(game->level_end) level_end_screen(game, player->stats->score, game->current_screen);

        game->camera_set(&player->dest);

        tile->render(game->camera);
        enemy->render(game->camera);
        player->render();

        SDL_RenderPresent(game->renderer);
    }
}

void level_two(Game* game){
    game->level_end = false;
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

    while(game->game_running && game->current_screen == game->LEVEL_2){
        SDL_PollEvent(&e);
        if(e.type == SDL_QUIT) game->game_running = false;

        const Uint8* key = SDL_GetKeyboardState(NULL);
        if(key[SDL_SCANCODE_ESCAPE])game->game_pause = true;

        if(game->game_pause) pause_screen(game, player->stats->score);

        SDL_RenderClear(game->renderer);
        SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);

        enemy_one->handle_event(e);
        enemy_two->handle_event(e);
        player->handle_event(e);

        if(game->level_end) level_end_screen(game, player->stats->score, game->current_screen);

        game->camera_set(&player->dest);

        tile->render(game->camera);
        enemy_one->render(game->camera);
        enemy_two->render(game->camera);
        player->render();

        SDL_RenderPresent(game->renderer);
    }
}

void level_three(Game* game){
    game->level_end = false;
    game->set_level_dimension(2560, 1920);

    Tile* tile = new Tile(32, 32, game);
    loadTile(tile, "png/level-1-tiles.png", "map/level_3.txt", 80, 60);

    Player* player = new Player(32, 32, game, tile);
    loadPlayer(player);
    player->set_spawn_point(77, 3);

    SDL_Event e;

    while(game->game_running && game->current_screen == game->LEVEL_3){
        SDL_PollEvent(&e);
        if(e.type == SDL_QUIT) game->game_running = false;

        const Uint8* key = SDL_GetKeyboardState(NULL);
        if(key[SDL_SCANCODE_ESCAPE])game->game_pause = true;

        if(game->game_pause) pause_screen(game, player->stats->score);

        SDL_RenderClear(game->renderer);
        SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);

        player->handle_event(e);

        if(game->level_end) level_end_screen(game, player->stats->score, game->current_screen);

        game->camera_set(&player->dest);

        tile->render(game->camera);
        player->render();

        SDL_RenderPresent(game->renderer);
    }
}