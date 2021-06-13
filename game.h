#pragma once

#include "init.h"

struct Game {
    
    enum screens {
        UI_SCREEN,
        START_SCREEN,
        LOAD_SCREEN,
        OPTIONS_SCREEN,
        HISCORE_SCREEN,
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
        BUTTON_BACK
    };

    struct Events{
        Sint32 player_damaged;
        Sint32 enemy_damaged;
        Sint32 reset_event;
        
        void reset(SDL_Event& e){
            e.user.code = reset_event;
        }
    };

    SDL_Window* window;
    SDL_Renderer* renderer;
    Events event;
    SDL_Rect camera;
    int LEVEL_WIDTH, LEVEL_HEIGHT;  //dimensions of the total level. LEVEL_WIDHT would make more sense i understand
    int RENDER_WIDTH, RENDER_HEIGHT;  //dimensions of the camera that will follow player. The area of the map to be rendered
    int current_screen;
    bool game_running;

    Game() {
        window = NULL;
        renderer = NULL;
        // LEVEL_WIDTH = width;
        // LEVEL_HEIGHT = height;
        RENDER_WIDTH = 800;
        RENDER_HEIGHT = 600;
        camera = {0, 0, RENDER_WIDTH, RENDER_HEIGHT};  //SDL_Rect camera now know what part to render

        event.player_damaged = 44;
        event.enemy_damaged = 45;
        event.reset_event = 2;
        game_running = true;

        current_screen = LEVEL_3;
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

    void resize_window(int w_, int h_){
        RENDER_WIDTH = w_;
        RENDER_HEIGHT = h_;
        camera.w = RENDER_WIDTH;
        camera.h = RENDER_HEIGHT;
        SDL_SetWindowSize(window, RENDER_WIDTH, RENDER_HEIGHT);
    }

    void button_action(int buttonID) {
        if (buttonID == BUTTON_START) current_screen = START_SCREEN;
        if (buttonID == BUTTON_LOAD) current_screen = LOAD_SCREEN;
        if (buttonID == BUTTON_OPTIONS) current_screen = OPTIONS_SCREEN;
        if (buttonID == BUTTON_HISCORE) current_screen = HISCORE_SCREEN;
        if (buttonID == BUTTON_QUIT) current_screen = QUIT_SCREEN;
        if (buttonID == BUTTON_BACK) current_screen = UI_SCREEN;
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

    void close() {
        SDL_DestroyWindow(window);
        window = NULL;

        SDL_DestroyRenderer(renderer);
        renderer = NULL;

        SDL_Quit();
        IMG_Quit();
    }
};


//LTexture. Obsolete. Didn't use it anywhere

//buttons that doesn't work now lol
// struct Button {
//     LTexture button[3];
//     SDL_Rect button_pos;
//     int button_status;
//     int buttonID;
//     enum button_status {
//         BUTTON_DEFAULT,
//         BUTTON_HOVER,
//         BUTTON_PRESSED
//     };

//     Button(int ID) {
//         buttonID = ID;
//     }
//     ~Button() {
//         button_status = BUTTON_DEFAULT;
//     }

//     void loadFromFile(std::string default_path, std::string hover_path, std::string pressed_path) {
//         button[BUTTON_DEFAULT].loadFromFile(default_path);
//         button[BUTTON_HOVER].loadFromFile(hover_path);
//         button[BUTTON_PRESSED].loadFromFile(pressed_path);
//     }

//     void setDest(int x, int y, int w, int h) {
//         // button_pos = {x, y, button[BUTTON_DEFAULT].width, button[BUTTON_DEFAULT].height };
//         button_pos = {x, y, w, h};
//         for (int i = 0; i < 3; i++) button[i].setDest(x, y, w, h);
//     }

//     void render(SDL_Event e) {
//         event_handler(e);
//         // SDL_RenderCopy( gGame->renderer, button[button_status].texture, NULL, &button_pos );
//         button[button_status].render();
//     }

//     void event_handler(SDL_Event e) {
//         int x, y;
//         SDL_GetMouseState(&x, &y);

//         if (x < button[BUTTON_DEFAULT].pos.x)
//             button_status = BUTTON_DEFAULT;
//         else if (x > button[BUTTON_DEFAULT].pos.x + button[BUTTON_DEFAULT].pos.w)
//             button_status = BUTTON_DEFAULT;
//         else if (y < button[BUTTON_DEFAULT].pos.y)
//             button_status = BUTTON_DEFAULT;
//         else if (y > button[BUTTON_DEFAULT].pos.y + button[BUTTON_DEFAULT].pos.h)
//             button_status = BUTTON_DEFAULT;

//         else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT && e.button.state == SDL_PRESSED) {
//             button_status = BUTTON_PRESSED;
//         } else if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT && e.button.state == SDL_RELEASED) {
//             button_status = BUTTON_HOVER;
//             gGame->button_action(buttonID);
//         } else
//             button_status = BUTTON_HOVER;
//     }
// };

