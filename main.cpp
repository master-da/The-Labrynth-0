#include <stdio.h>
#include <iostream>
#include <math.h>
#include <string>
#include <stdlib.h>
#include <cstring>
#include <algorithm>
#include <math.h>
#include <cstdio>
#include <vector>
#include <map>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#define error_sdl printf("sdl error. Error: %s\n", SDL_GetError());
#define error_img printf("image error. Error: %s\n", IMG_GetError());
#define error_tru printf("ttf error. Error: %s\n", TTF_GetError());

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;


//Loads individual image as texture
SDL_Texture *loadTexture(std::string path);

//The window we'll be rendering to
SDL_Window *gWindow = NULL;

//The window renderer
SDL_Renderer *gRenderer = NULL;

//Current displayed texture
SDL_Texture *gTexture = NULL;

//starts up SDL and creates window
bool init()
{
    //imitialisation flag
    bool success = true;

    //initialise SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        error_sdl;
        success = false;
    }
    else
    {
        //set texture filtering to linear
        if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"));
        printf("Warning: Linear texture filtering not enabled!");

        //Create window
		gWindow = SDL_CreateWindow( "Testing Background Renddering", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			error_sdl;
			success = false;
		}
        else
        {
            //create renderer for window
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
            if(gRenderer == NULL)
            {
                error_sdl;
                success = false;
            }
            else
            {
                //initialise renderer color
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

                //initialise PNG loading
                int imgFlags = IMG_INIT_PNG;
                if(!(IMG_Init(imgFlags) &imgFlags))
                {
                    error_img;
                    success = false;
                }
            }
        }
    }

    return success;
}

//loads media
bool loadMedia()
{
    //loading success flag
    bool success = true;

    //load PNG texture
    gTexture = loadTexture("fire.png");
    if(gTexture == NULL)
    {
        printf("Failed to load texture image!\n");
        success = false;
    }

    return success;
}

//frees media and shuts down SDL
void close()
{
    //free loaded image
    SDL_DestroyTexture(gTexture);
    gTexture = NULL;

    //destroy window
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gRenderer = NULL;
    gWindow = NULL;

    //quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
}

//loads individual image as texture
SDL_Texture *loadTexture(std::string path)
{
    //final texture
    SDL_Texture *newTexture = NULL;

    //load image at specified path
    SDL_Surface *loadedSurface = IMG_Load(path.c_str());

    if(loadedSurface == NULL)
    {
        error_img;
    }
    else
    {
        //create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if(newTexture == NULL)
            error_sdl;

        //get rid of old loaded surface
        SDL_FreeSurface(loadedSurface);
    }

    return newTexture;
}

int main(int argc, char *args[])
{
    //Start up SDL and create window
    if (!init())
    {
        printf("Failed to initialize!\n");
    }
    else
    {
        //Load media
        if (!loadMedia())
        {
            printf("Failed to load media!\n");
        }
        else
        {
            //Main loop flag
            bool quit = false;

            //Event handler
            SDL_Event e;

            //While application is running
            while (!quit)
            {
                //Handle events on queue
                while (SDL_PollEvent(&e) != 0)
                {
                    //User requests quit
                    if (e.type == SDL_QUIT)
                    {
                        quit = true;
                    }
                }

                //Clear screen
                SDL_RenderClear(gRenderer);

                //Render texture to screen
                SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);

                //Update screen
                SDL_RenderPresent(gRenderer);
            }
        }
    }

    //Free resources and close SDL
    close();

    return 0;
}