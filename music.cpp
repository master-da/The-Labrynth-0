//shift the functions to separate headers as necessary and delete unnecessary comments
<<<<<<< HEAD
#include "init.h"
=======

>>>>>>> Noki
#define MAX_SOUND_CHANNELS 8  //no of sound effects that can be played at the same time

Mix_Music *music;

enum channel {
    CHN_ANY = -1,
    CHN_PLAYER,
    CHN_ENEMY
};

enum sounds {
    SOUND_PLAYER_WALK,
    SOUND_PLAYER_DIE,
    SOUND_ENEMY_ATTACK,
    SOUND_ENEMY_DIE,
    SOUND_GATE_OPEN,
    SOUND_SWITCH,
    SOUND_MAX
};

//initialise sdl mixer
void initMixer() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
        error_m;
        exit(1);
    }

    Mix_AllocateChannels(MAX_SOUND_CHANNELS);
}

void initSounds() {
    memset(sounds, 0, sizeof(Mix_Chunk*) * SOUND_MAX);
    music = NULL;
    loadSounds();
}

//load sound effects
static void loadSounds() {
    sounds[SOUND_PLAYER_WALK] = Mix_LoadWAV("sound/sfx_step_grass_l.flac");
    sounds[SOUND_PLAYER_DIE] = Mix_LoadWAV("sound/Death.wav");
    sounds[SOUND_ENEMY_ATTACK] = Mix_LoadWAV("sound/Crossbow_Shot.wav");
    sounds[SOUND_ENEMY_DIE] = Mix_LoadWAV("sound/Death.wav");
    sounds[SOUND_GATE_OPEN] = Mix_LoadWAV("sound/doorOpen_1.ogg");
    sounds[SOUND_SWITCH] = Mix_LoadWAV("sound/metalClick.ogg");
}

//loads music into memory
void loadMusic(char *filename) {
    if (music != NULL) {
        Mix_HaltMusic();  //stops current music
        Mix_FreeMusic(music);
        music = NULL;
    }

    music = Mix_LoadMUS(filename);
}

//music either plays forever in a loop or once
void playMusic(int loop) {
    Mix_PlayMusic(music, (loop) ? -1 : 0);
}

void playSound(int id, int channel) {
    Mix_PlayChannel(channel, sounds[id], 0);
}


//PLACE IN PROPER STRUCTS, I.E. PLAYER/ENEMY
playSound(SOUND_PLAYER_WALK, CHN_PLAYER);  //when player walks
playSound(SOUND_PLAYER_DIE, CHN_PLAYER);   //when player dies
playSound(SOUND_ENEMY_DIE, CHN_ENEMY);     //when enemy dies
playSound(SOUND_ENEMY_ATTACK, CHN_ENEMY);  //when enemy attacks
playSound(SOUND_GATE_OPEN, CHN_ANY);       //when gate opens, sound plays through any free channel
playSound(SOUND_SWITCH, CHN_ANY);          //when switch in use, sound plays through any free channel
loadMusic("sound/Woodland_Fantasy.mp3");                       //menu
loadMusic("sound/Harp.mp3");                       //game running
loadMusic("sound/Enchanted_Festival.mp3");     //level complete