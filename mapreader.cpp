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
#include <fstream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define error {printf("sdl error in line %d. Error: %s\n", __LINE__, SDL_GetError());}
#define error_i {printf("image error in line %d. Error: %s\n", __LINE__, IMG_GetError());}
#define error_m {printf("mixer error in line %d. Error: %s\n", __LINE__, MIX_GetError());}

const int WIDTH = 1280;
const int HEIGHT = 960;

void close();

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;

void init(){
    if(SDL_Init( SDL_INIT_VIDEO ) < 0) error;

    if( !(IMG_Init( IMG_INIT_PNG ) & IMG_INIT_PNG) ) error_i;

    gWindow = SDL_CreateWindow( "main2win", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN );
    if(gWindow == NULL) error;

    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    if(gRenderer == NULL) error;

    SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
}

class LTexture
{
	public:
		//Initializes variables
		LTexture();

		//Deallocates memory
		~LTexture();

		//Loads image at specified path
		bool loadFromFile( std::string path );

		//Deallocates texture
		void free();

		//Set color modulation
		void setColor( Uint8 red, Uint8 green, Uint8 blue );

		//Set blending
		void setBlendMode( SDL_BlendMode blending );

		//Set alpha modulation
		void setAlpha( Uint8 alpha );
		
		//Renders texture at given point
		void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );

		//Gets image dimensions
		int getWidth();
		int getHeight();

		//Pixel manipulators
		bool lockTexture();
		bool unlockTexture();
		void* getPixels();
		int getPitch();

	private:
		//The actual hardware texture
		SDL_Texture* mTexture;
		void* mPixels;
		int mPitch;

		//Image dimensions
		int mWidth;
		int mHeight;
};
LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
	mPixels = NULL;
	mPitch = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

bool LTexture::loadFromFile( std::string path )
{
	free();

	SDL_Texture* newTexture = NULL;

	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat( loadedSurface, SDL_GetWindowPixelFormat( gWindow ), 0 );
		if( formattedSurface == NULL ){
			printf( "Unable to convert loaded surface to display format! SDL Error: %s\n", SDL_GetError() );
		}
		else{
			newTexture = SDL_CreateTexture( gRenderer, SDL_GetWindowPixelFormat( gWindow ), SDL_TEXTUREACCESS_STREAMING, formattedSurface->w, formattedSurface->h );
			if( newTexture == NULL ){
				printf( "Unable to create blank texture! SDL Error: %s\n", SDL_GetError() );
			}
			else{
				//Lock texture for manipulation
				SDL_LockTexture( newTexture, NULL, &mPixels, &mPitch );

				//Copy loaded/formatted surface pixels
				memcpy( mPixels, formattedSurface->pixels, formattedSurface->pitch * formattedSurface->h );

				//Unlock texture to update
				SDL_UnlockTexture( newTexture );
				mPixels = NULL;

				//Get image dimensions
				mWidth = formattedSurface->w;
				mHeight = formattedSurface->h;
			}

			//Get rid of old formatted surface
			SDL_FreeSurface( formattedSurface );
		}	
		
		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

void LTexture::free()
{
	//Free texture if it exists
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
		mPixels = NULL;
		mPitch = 0;
	}
}

void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture::setBlendMode( SDL_BlendMode blending )
{
	SDL_SetTextureBlendMode( mTexture, blending );
}
		
void LTexture::setAlpha( Uint8 alpha )
{
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
{
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	if( clip != NULL )
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}
	SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip );
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}

bool LTexture::lockTexture()
{
	bool success = true;

	if( mPixels != NULL ){
		printf( "Texture is already locked!\n" );
		success = false;
	}
	//Lock texture
	else{
		if( SDL_LockTexture( mTexture, NULL, &mPixels, &mPitch ) != 0 )
		{
			printf( "Unable to lock texture! %s\n", SDL_GetError() );
			success = false;
		}
	}

	return success;
}

bool LTexture::unlockTexture()
{
	bool success = true;

	//Texture is not locked
	if( mPixels == NULL ){
		printf( "Texture is not locked!\n" );
		success = false;
	}
	//Unlock texture
	else{
		SDL_UnlockTexture( mTexture );
		mPixels = NULL;
		mPitch = 0;
	}

	return success;
}

void* LTexture::getPixels()
{
	return mPixels;
}

int LTexture::getPitch()
{
	return mPitch;
}

LTexture map;
std::ofstream gFile;

void loadmedia(){
    map.loadFromFile("png/level-1-design.png");
    gFile.open("mapped.txt");
}

int main(int argc, char* argv[])
{
    init();
    loadmedia();
    
    if(!map.lockTexture()) error;

    SDL_PixelFormat* mappingformat = SDL_AllocFormat( SDL_GetWindowPixelFormat( gWindow ) );

    Uint32* pixel = (Uint32*)map.getPixels();
    int pixelcount = (map.getPitch()/4) * map.getHeight();

    Uint32 wall = SDL_MapRGB( mappingformat, 0, 0, 0 );
    Uint32 slope = SDL_MapRGB( mappingformat, 255, 0, 0 );
    Uint32 walk = SDL_MapRGB( mappingformat, 255, 255, 255 );
    Uint32 button = SDL_MapRGB( mappingformat, 0, 0, 255 );
    Uint32 gate = SDL_MapRGB( mappingformat, 0, 255, 0 );

    Uint8 r, g, b;

    for(int i=0; i<30; i++){
        for(int j=0; j<40; j++){

            SDL_GetRGB( pixel[40*i+j], mappingformat, &r, &g, &b);

            if( SDL_MapRGB( mappingformat, r, g, b) == wall ) gFile << "0000 ";
            else if( SDL_MapRGB( mappingformat, r, g, b) == slope ) gFile << "0001 ";
            else if( SDL_MapRGB( mappingformat, r, g, b) == walk ) gFile << "0002 ";
            else if( SDL_MapRGB( mappingformat, r, g, b) == button ) gFile << "0003 ";
            else if( SDL_MapRGB( mappingformat, r, g, b) == gate ) gFile << "0004 ";
        }
        gFile << "\n";
    }
    SDL_FreeFormat( mappingformat );
    close();
    return 0;
}

void close(){

    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    SDL_DestroyRenderer( gRenderer );
    gRenderer = NULL;

    gFile.close();
    SDL_Quit();
    IMG_Quit();
}