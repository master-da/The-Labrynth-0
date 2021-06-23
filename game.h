#pragma once

#include "init.h"

struct Game {
    
    enum screens {
        UI_SCREEN,
        LOAD_SCREEN,
        OPTIONS_SCREEN,
        HISCORE_SCREEN,
        LEVEL_CHOICE,
        LEVEL_1,
        LEVEL_2,
        LEVEL_3,
        QUIT_SCREEN
    };
    
    enum buttonid {
        BUTTON_START,
        BUTTON_LOAD,
        BUTTON_OPTIONS,
        BUTTON_HISCORE,
        BUTTON_QUIT,
        BUTTON_NEXT,
        BUTTON_CONTINUE,
        BUTTON_HOME
    };

    enum button_size{
        BUTTON_SMALL,
        BUTTON_REGULAR
    };
    
    enum tiletypes{
        ACCESSIBLE,
        UNACCESSIBLE
    };

    enum sound_channel_list{
        MUSIC_CHANNEL,
        SFX_CHANNEL_1,
        SFX_CHANNEL_2,
        TOTAL_CHANNEL
    };

    struct Events{
        Sint32 player_damaged;
        Sint32 enemy_damaged;
        Sint32 door_opened;
        Sint32 chest_opened;
        Sint32 reset_event;
        
        void create_event(Sint32 code_, int* data1_, int* data2_){
            SDL_Event e;
            SDL_memset(&e, 0, sizeof(e));
            e.type = SDL_RegisterEvents(1);

            if(e.type == (Uint32)-1) error
            else {
                e.user.code = code_;
                if(data1_ != NULL)e.user.data1 = (int*)data1_;
                if(data2_ != NULL)e.user.data2 = (int*)data2_;
                SDL_PushEvent(&e);
            }
        }

        void reset(SDL_Event& e){
            e.user.code = reset_event;
        }
    };
 
    SDL_Window* window;
    SDL_Renderer* renderer;
    Events event;
    SDL_Rect camera;
    TTF_Font* font;
    SDL_Texture* number[10];
    SDL_Texture* your_score;

    int LEVEL_WIDTH, LEVEL_HEIGHT;  //dimensions of the total level.
    int RENDER_WIDTH, RENDER_HEIGHT;  //dimensions of the camera that will follow player. The area of the map to be rendered
    int current_screen;
    int sound_channel[TOTAL_CHANNEL];
    int sound_level[TOTAL_CHANNEL];
    bool game_running;
    bool level_end;
    bool game_pause;

    Game() {
        window = NULL;
        renderer = NULL;
        RENDER_WIDTH = 800;
        RENDER_HEIGHT = 600;
        camera = {0, 0, RENDER_WIDTH, RENDER_HEIGHT};  //SDL_Rect camera now know what part to render

        event.player_damaged = 44;
        event.enemy_damaged = 45;
        event.door_opened = 46;
        event.chest_opened = 47;
        event.reset_event = 2;
        game_running = true;
        game_pause = false;

        sound_channel[MUSIC_CHANNEL] = 1;
        sound_channel[SFX_CHANNEL_1] = 2;
        sound_channel[SFX_CHANNEL_2] = 2;

        sound_level[MUSIC_CHANNEL] = MIX_MAX_VOLUME;
        sound_level[SFX_CHANNEL_1] = MIX_MAX_VOLUME;
        sound_level[SFX_CHANNEL_2] = MIX_MAX_VOLUME;

        current_screen = LEVEL_1;
    }
    
    ~Game() {
        close();
    }

    //main initializer function
    void init(std::string name, bool fullscreen) {
        if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) == 0)
            printf("SDL Initialised with video\n");
        else
            error;

        window = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, RENDER_WIDTH, RENDER_HEIGHT, fullscreen ? SDL_WINDOW_FULLSCREEN : SDL_WINDOW_SHOWN);
        if (window)
            printf("Window intialised\n");
        else
            error;

        if (IMG_Init(IMG_INIT_PNG) & (IMG_INIT_PNG == IMG_INIT_PNG))
            printf("Image Initialised with PNG\n");
        else
            error_i;

        if(TTF_Init() != -1)
            printf("TTF initialized\n");
        else
            error_t

        if((Mix_Init(MIX_INIT_MP3)&MIX_INIT_MP3) == MIX_INIT_MP3)
            printf("Mixer Initialized with mp3\n");
        else 
            error_m

        if(!Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096))
            printf("Audio Opened\n");
        else 
            error_m

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (renderer)
            printf("Renderer initialised\n");
        else
            error;
    }

    void font_loader(int font_size_){
        font = TTF_OpenFont("fonts/Amatic-Bold.ttf", font_size_);
        if(font == NULL) error_t

        SDL_Surface* tempSurf = TTF_RenderText_Solid(font, "YOUR SCORE", {0, 0, 0});
        if(tempSurf == NULL) error_t
        your_score = SDL_CreateTextureFromSurface(renderer, tempSurf);

        font = TTF_OpenFont("fonts/AmaticSC-Regular.ttf", font_size_);
        if(font == NULL) error_t

        for(int i=0; i<10; i++){
            tempSurf = TTF_RenderText_Solid(font, std::to_string(i).c_str(), {0, 0, 0});
            if(tempSurf == NULL) error_t
            number[i] = SDL_CreateTextureFromSurface(renderer, tempSurf);
        }

        SDL_FreeSurface(tempSurf);
        tempSurf = NULL;
    }

    void resize_window(int w_, int h_){
        RENDER_WIDTH = w_;
        RENDER_HEIGHT = h_;
        camera.w = RENDER_WIDTH;
        camera.h = RENDER_HEIGHT;
        SDL_SetWindowSize(window, RENDER_WIDTH, RENDER_HEIGHT);
    }

    void button_action(int buttonID){
        if(buttonID == BUTTON_START) current_screen = LEVEL_1;
        if(buttonID == BUTTON_LOAD) current_screen = LEVEL_CHOICE;
        if(buttonID == BUTTON_OPTIONS) current_screen = OPTIONS_SCREEN;
        if(buttonID == BUTTON_HISCORE) current_screen = LOAD_SCREEN;
        if(buttonID == BUTTON_QUIT) game_running = false;
        if(buttonID == BUTTON_CONTINUE) game_pause = false;
        if(buttonID == BUTTON_NEXT) current_screen = current_screen + 1;
        if(buttonID == BUTTON_HOME) current_screen = UI_SCREEN, game_pause = false;
    }

    //AABB collision detection.
    bool collision(SDL_Rect* a, SDL_Rect* b) {
        if (a->x > b->x + b->w) return 0;
        if (a->x + a->w < b->x) return 0;
        if (a->y > b->y + b->h) return 0;
        if (a->y + a->w < b->y) return 0;
        return 1;
    }

    void set_level_dimension(int width, int height){
        LEVEL_WIDTH = width;
        LEVEL_HEIGHT = height;
    }

    //after every player movement, focus camera on player head, and render a 800x600 region around player
    void camera_set(SDL_Rect* rect) {
        camera.x = rect->x + rect->w / 2 - RENDER_WIDTH / 2;
        camera.y = rect->y + rect->h / 2 - RENDER_HEIGHT / 2;
        if (camera.x < 0) camera.x = 0;
        if (camera.x + camera.w > LEVEL_WIDTH) camera.x = LEVEL_WIDTH - camera.w;
        if (camera.y < 0) camera.y = 0;
        if (camera.y + camera.h > LEVEL_HEIGHT) camera.y = LEVEL_HEIGHT - camera.h;
    }

    void music_volume_coltrol(char action_){
        if(action_ == 'd') 
            sound_level[MUSIC_CHANNEL] = sound_level[MUSIC_CHANNEL] - 16 < 0? 0 : sound_level[MUSIC_CHANNEL] - 16;
        else if (action_ == 'i') 
            sound_level[MUSIC_CHANNEL] = sound_level[MUSIC_CHANNEL] + 16 > MIX_MAX_VOLUME? MIX_MAX_VOLUME : sound_level[MUSIC_CHANNEL] + 16;
        else printf("volume input not recognized\n");
    }

    void sfx_volume_coltrol(char action_){
        if(action_ == 'd') {
            sound_level[SFX_CHANNEL_1] = sound_level[SFX_CHANNEL_1] - 16 < 0? 0 : sound_level[SFX_CHANNEL_1] - 16;
            sound_level[SFX_CHANNEL_2] = sound_level[SFX_CHANNEL_2] - 16 < 0? 0 : sound_level[SFX_CHANNEL_2] - 16;
        }
        else if (action_ == 'i') {
            sound_level[SFX_CHANNEL_1] = sound_level[SFX_CHANNEL_1] + 16 > MIX_MAX_VOLUME? MIX_MAX_VOLUME : sound_level[SFX_CHANNEL_1] + 16;
            sound_level[SFX_CHANNEL_2] = sound_level[SFX_CHANNEL_2] + 16 > MIX_MAX_VOLUME? MIX_MAX_VOLUME : sound_level[SFX_CHANNEL_2] + 16;
        }
        else printf("volume input not recognized\n");
    }

    void close() {
        SDL_DestroyWindow(window);
        window = NULL;

        SDL_DestroyRenderer(renderer);
        renderer = NULL;

        SDL_Quit();
        IMG_Quit();
        TTF_Quit();
    }
};

struct Score{
    int score;
    Game* game;
    int disp_score;
    int height;
    SDL_Rect text_rect;
    SDL_Rect num_rect;

    Score(Game* game_, int score_){
        game = game_;
        score = score_;
        disp_score = 0;
        height = 100;
        text_rect = {50, 100, height, 50};
        num_rect = {600, 100, height, 50};
    }

    void set_height(int h_){
        height = h_;
        text_rect = {50, 100, height, 50};
        num_rect = {600, 100, height, 50};
    }

    void render(){
        SDL_RenderCopy(game->renderer, game->your_score, NULL, &text_rect);

        int tmp = disp_score;
        while(tmp){
            SDL_RenderCopy(game->renderer, game->number[tmp%10], NULL, &num_rect);
            num_rect.x -= num_rect.w;
            tmp /= 10;
        }
        num_rect.x = 600;
        if(disp_score<score) disp_score++;
    }
};

// struct Collectible{

//     enum rewards{
//         HEALTH_REGEN,
//         BERSERK,
//         IMMUNITY,
//         AR_BUFF,
//         REWARD_TOTAL
//     };

//     int slow_factor;
//     int frame;
//     int* reward;
//     SDL_Texture* look;
//     SDL_Rect src;
//     Game* game = new Game();

//     Collectible(Game* game_){
//         slow_factor = 4;
//         frame = 0;
//         game = game_;
//         src = {0, 0, 42, 32};
//         reward = NULL;
//     }

//     void loadFromFile(){

//         char chest_path[] = "png/chest.png";

//         SDL_Surface* imgTemp = IMG_Load(chest_path);
//         if(imgTemp == NULL) error_i 
//         look = SDL_CreateTextureFromSurface(game->renderer, imgTemp);
//         if(look == NULL) error;
//     }

//     void open_chest(){
//         frame++;
//         src.x = (frame / slow_factor) * src.w;
//         if(frame/slow_factor == 2) *reward = rand() % REWARD_TOTAL;
//     }

//     void handle_event(SDL_Event e){
//         if(reward == NULL){
//             if(e.user.code == game->event.chest_opened) frame = 1;
//             if(frame) open_chest();
//         }
//     }
    
//     void render(SDL_Rect* dest){
//         SDL_RenderCopy(game->renderer, look, &src, dest);
//     }

// };
