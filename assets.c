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

        { "base-tile.png" },
        { "base-ne.png" },

        { "space.png" },
		{ "star-bright.png" },
		{ "star-dim.png" },
		{ "star-dark.png" },
        { "ta-floor.png" },

        { "health-r.png" },
        { "health-g.png" },
        { "health-y.png" },
        { "dark-red.png" },
        { "black.png" },

        { "w_mach-0.png" },

        { "w_rock-n.png" },
        { "w_rock-s.png" },
        { "w_rock-e.png" },
        { "w_rock-w.png" },

        { "w_rock-ne.png" },
        { "w_rock-nw.png" },
        { "w_rock-se.png" },
        { "w_rock-sw.png" },

        { "w_rock2-0.png" },
        { "w_rock3-0.png" },
        { "w_rock3-0-fire.png" },
        { "w_rock3-1.png" },
        { "w_rock3-2.png" },
        { "w_rock-0.png" },
        { "w_rock-1.png" },
        { "w_rock-0-fire.png" },

        { "tele-0.png" },
        { "tele-1.png" },
        { "tele-2.png" },
 
        { "rocket-n.png" },
        { "rocket-s.png" },
        { "rocket-e.png" },
        { "rocket-w.png" },
        { "rocket-ne.png" },
        { "rocket-se.png" },
        { "rocket-nw.png" },
        { "rocket-sw.png" },

        { "rocket-01.png" },
        { "rocket-02.png" },
        { "rocket-03.png" },
        { "rocket-04.png" },

        { "flash.png" },

        { "puff-01.png" },
        { "puff-02.png" },
        { "puff-03.png" },

        { "lem-pink-01.png" },
        { "lem-pink-02.png" },
        { "lem-pink-03.png" },
        { "lem-pink-04.png" },
        { "lem-pink-dead-dark.png" },
        { "lem-pink-stun-01.png" },
        { "lem-pink-stun-02.png" },

        { "lem-red-exp-01.png" },
        { "lem-red-exp-02.png" },
        { "lem-red-exp-03.png" },
        { "lem-blue-exp-01.png" },
        { "lem-blue-exp-02.png" },
        { "lem-blue-exp-03.png" },
        { "lem-orange-exp-01.png" },
        { "lem-orange-exp-02.png" },
        { "lem-orange-exp-03.png" },
        { "lem-pink-exp-01.png" },
        { "lem-pink-exp-02.png" },
        { "lem-pink-exp-03.png" },

        { "lem-orange-01.png" },
        { "lem-orange-02.png" },
        { "lem-orange-03.png" },
        { "lem-orange-04.png" },
        { "lem-orange-dead-dark.png" },
        { "lem-orange-stun-01.png" },
        { "lem-orange-stun-02.png" },

        { "lem-red-01.png" },
        { "lem-red-02.png" },
        { "lem-red-03.png" },
        { "lem-red-04.png" },
        { "lem-red-stun-01.png" },
        { "lem-red-stun-02.png" },

        { "lem-green-01.png" },
        { "lem-green-02.png" },
        { "lem-green-03.png" },
        { "lem-green-04.png" },
        { "lem-green-dead-dark.png" },
        { "lem-green-stun-01.png" },
        { "lem-green-stun-02.png" },

        { "lem-blue-01.png" },
        { "lem-blue-02.png" },
        { "lem-blue-03.png" },
        { "lem-blue-04.png" },
        { "lem-blue-dead-dark.png" },
        { "lem-blue-stun-01.png" },
        { "lem-blue-stun-02.png" },

        { "stars-01.png" },
        { "stars-02.png" },
        { "stars-03.png" },
        { "stars-04.png" },

        { "exp-01.png" },
        { "exp-02.png" },
        { "exp-03.png" },
        { "exp-04.png" },
        { "exp-05.png" },
        { "exp-06.png" },

        { "arrow.png" },
        { "p1.png" },
        { "p1-arrow.png" },

        { "ground-big.png" },
        { "ground.png" },
        { "ground2.png" },
        { "ground3.png" },
        { "ground4.png" },

        { "rock.png" },
        { "rock2.png" },
        { "rock3.png" },
        { "rock4.png" },
        { "rock5.png" },
        { "rock6.png" },
        { "rock7.png" },

        { "white.png" },
        { "cross.png" },
        { "score.png" },
        { "score-2.png" },

        { "font-0.png" },
        { "font-1.png" },
        { "font-2.png" },
        { "font-3.png" },
        { "font-4.png" },
        { "font-5.png" },
        { "font-6.png" },
        { "font-7.png" },
        { "font-8.png" },
        { "font-9.png" },

        { "font-a.png" },
        { "font-b.png" },
        { "font-c.png" },
        { "font-d.png" },
        { "font-e.png" },
        { "font-f.png" },
        { "font-g.png" },
        { "font-h.png" },
        { "font-i.png" },
        { "font-j.png" },
        { "font-k.png" },
        { "font-l.png" },
        { "font-m.png" },
        { "font-n.png" },
        { "font-o.png" },
        { "font-p.png" },
        { "font-q.png" },
        { "font-r.png" },
        { "font-s.png" },
        { "font-t.png" },
        { "font-u.png" },
        { "font-v.png" },
        { "font-w.png" },
        { "font-x.png" },
        { "font-y.png" },
        { "font-z.png" },
        { "font-bang.png" },
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