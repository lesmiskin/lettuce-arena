#include <assert.h>
#include "assets.h"
#include "renderer.h"

typedef struct {
    char* filename;
} AssetDef;

typedef struct {
    char* filename;
    int volume;
} SoundDef;

static char* assetPath;
static Asset *assets;
static int assetCount;
static SoundAsset *sounds;
static int soundCount;

SoundAsset getSound(char *path) {
    //Loop through register until key is found, or we've exhausted the array's iteration.
    for(int i=0; i < soundCount; i++) {
        if(strcmp(sounds[i].key, path) == 0)			//if strings match.
            return sounds[i];
    }

    fatalError("Could not find Asset in register", path);
}

static Asset makeAsset(AssetDef definition) {
    assert(renderer != NULL);

    char *absPath = combineStrings(assetPath, definition.filename);
    //Check existence on file system.
    if(!fileExists(absPath))
        fatalError("Could not find Asset on disk", absPath);

    //Load file from disk.
    SDL_Surface *original = IMG_Load(absPath);
    free(absPath);

    Asset asset = {	definition.filename	};

    //Build initial texture and assign.
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, original);

    asset.texture = texture;

    //Free up RAM used for original surface object.
    free(original);

    return asset;
}

SDL_Texture *getTexture(char *path) {
    return getAsset(path).texture;
}
Asset getAsset(char *path) {
    //Loop through register until key is found, or we've exhausted the array's iteration.
    for(int i=0; i < assetCount; i++) {
        if(strcmp(assets[i].key, path) == 0)			//if strings match.
            return assets[i];
    }

    fatalError("Could not find Asset in register", path);
}

void shutdownAssets(void) {
    free(assetPath);
    free(assets);

    for(int i=0; i < soundCount; i++) Mix_FreeChunk(sounds[i].sound);

    free(sounds);
}

static void loadImages(void) {
    //Define assets to be loaded.
    AssetDef definitions[] = {
        { "enemy.png" },
        { "player.png" },
        { "dirt.png" },
        { "rocket.png" },
        { "puff.png" },
        { "worker.png" },
        { "lem-01.png" },
        { "lem-02.png" },
        { "lem-03.png" },
        { "lem-04.png" },
    };

    //Infer asset path from current directory.
    char* workingPath = SDL_GetBasePath();
    char assetsFolder[] = "assets/";
    assetPath = combineStrings(workingPath, assetsFolder);

    //Allocate memory to Asset register.
    assetCount = sizeof(definitions) / sizeof(AssetDef);
    assets = malloc(sizeof(Asset) * assetCount);

    //Build and load each Asset into the register.
    for(int i=0; i < assetCount; i++) {
        assets[i] = makeAsset(definitions[i]);
    }
}

static void loadSounds(void) {
    const int SOUND_VOLUME = 12;

    SoundDef defs[] = {
//        { "bell.wav", SOUND_VOLUME }
    };

    soundCount = sizeof(defs) / sizeof(SoundDef);
    sounds = malloc(sizeof(SoundAsset) * soundCount);

    for(int i=0; i < soundCount; i++) {
        //Load music.
        char* path = combineStrings(assetPath, defs[i].filename);
        Mix_Chunk* chunk = Mix_LoadWAV(path);
        if(!chunk) fatalError("Could not find Asset on disk", path);

        //Reduce volume if called for.
        if(defs[i].volume < SDL_MIX_MAXVOLUME) Mix_VolumeChunk(chunk, defs[i].volume);

        //Add to register
        SoundAsset snd = {
            defs[i].filename,
            chunk
        };
        sounds[i] = snd;
    }
}

void initAssets(void) {
    loadImages();
    loadSounds();
}

void play(char* path) {
    Mix_PlayChannel(-1, getSound(path).sound, 0);
}
