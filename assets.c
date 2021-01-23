#include <assert.h>
#include "assets.h"
#include "renderer.h"
#include "mysdl.h"

typedef struct {
    char* filename;
	bool makeWhiteVersion;
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

	asset.textures[ASSET_DEFAULT] = texture;

	// make a white version
	if(definition.makeWhiteVersion) {
		colouriseSprite(original, makeColour(255,0,0,255), 1 /* additive */);
		SDL_Texture *hitTexture = SDL_CreateTextureFromSurface(renderer, original);
		asset.textures[ASSET_WHITE] = hitTexture;
	}

    //Free up RAM used for original surface object.
    free(original);

    return asset;
}

SDL_Texture *getTexture(char *path) {
	return getTextureVersion(path, ASSET_DEFAULT);
}
SDL_Texture *getTextureVersion(char *path, AssetVersion version) {
	return getAsset(path).textures[version];
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

        { "light.png", false },
        { "base-tile.png", false },
        { "base-ne.png", false },

        { "shadow-1.png", false },
        { "shadow-2.png", false },

        { "ammo-bullets.png", false },
        { "ammo-rocket.png", false },
        { "health-pack.png", false },
        { "powerup-health-3.png", false },
        { "powerup-health-4.png", false },
        { "powerup-health-6.png", false },
        { "powerup-ammo-2.png", false },

        { "powerup-ammo-3-0.png", false },
        { "powerup-ammo-3-1.png", false },
        { "powerup-health-5-0.png", false },
        { "powerup-health-5-1.png", false },

        { "bar.png", false },
        { "space.png", false },
		{ "star-bright.png", false },
		{ "star-dim.png", false },
		{ "star-dark.png", false },
        { "ta-floor.png", false },

        { "health-r.png", false },
        { "health-g.png", false },
        { "health-y.png", false },
        { "dark-red.png", false },
        { "black.png", false },

        { "w_mach.png", true },
        { "bullet.png", false },

		// rocket carry
        { "w_rock-n.png", true },
        { "w_rock-s.png", true },
        { "w_rock-e.png", true },
        { "w_rock-w.png", true },
        { "w_rock-ne.png", true },
        { "w_rock-nw.png", true },
        { "w_rock-se.png", true },
        { "w_rock-sw.png", true },

		// rocket pickup
        { "w_rock3-0.png", false },
        { "w_rock3-1.png", false },
        { "w_rock3-2.png", false },

        { "tele-0.png", false },
        { "tele-1.png", false },
        { "tele-2.png", false },
 
        { "rocket-n.png", false },
        { "rocket-s.png", false },
        { "rocket-e.png", false },
        { "rocket-w.png", false },
        { "rocket-ne.png", false },
        { "rocket-se.png", false },
        { "rocket-nw.png", false },
        { "rocket-sw.png", false },

        { "rocket-01.png", false },
        { "rocket-02.png", false },
        { "rocket-03.png", false },
        { "rocket-04.png", false },

        { "flash.png", false },

        { "puff-01.png", false },
        { "puff-02.png", false },
        { "puff-03.png", false },

        { "lem-pink-01.png", true },
        { "lem-pink-02.png", true },
        { "lem-pink-03.png", true },
        { "lem-pink-04.png", true },
        { "lem-pink-dead-dark.png", true },
        { "lem-pink-stun-01.png", true },
        { "lem-pink-stun-02.png", true },

        { "lem-red-exp-01.png", true },
        { "lem-red-exp-02.png", true },
        { "lem-red-exp-03.png", true },
        { "lem-blue-exp-01.png", true },
        { "lem-blue-exp-02.png", true },
        { "lem-blue-exp-03.png", true },
        { "lem-orange-exp-01.png", true },
        { "lem-orange-exp-02.png", true },
        { "lem-orange-exp-03.png", true },
        { "lem-pink-exp-01.png", true },
        { "lem-pink-exp-02.png", true },
        { "lem-pink-exp-03.png", true },

        { "lem-orange-01.png", true },
        { "lem-orange-02.png", true },
        { "lem-orange-03.png", true },
        { "lem-orange-04.png", true },
        { "lem-orange-dead-dark.png", true },
        { "lem-orange-stun-01.png", true },
        { "lem-orange-stun-02.png", true },

        { "lem-red-01.png", true },
        { "lem-red-02.png", true },
        { "lem-red-03.png", true },
        { "lem-red-04.png", true },
        { "lem-red-stun-01.png", true },
        { "lem-red-stun-02.png", true },

        { "lem-green-01.png", true },
        { "lem-green-02.png", true },
        { "lem-green-03.png", true },
        { "lem-green-04.png", true },
        { "lem-green-dead-dark.png", true },
        { "lem-green-stun-01.png", true },
        { "lem-green-stun-02.png", true },

        { "lem-blue-01.png", true },
        { "lem-blue-02.png", true },
        { "lem-blue-03.png", true },
        { "lem-blue-04.png", true },
        { "lem-blue-dead-dark.png", true },
        { "lem-blue-stun-01.png", true },
        { "lem-blue-stun-02.png", true },

        { "stars-01.png", false },
        { "stars-02.png", false },
        { "stars-03.png", false },
        { "stars-04.png", false },

        { "exp-01.png", false },
        { "exp-02.png", false },
        { "exp-03.png", false },
        { "exp-04.png", false },
        { "exp-05.png", false },
        { "exp-06.png", false },

        { "arrow.png", false },
        { "p1.png", false },
        { "p1-arrow.png", false },

        { "ground-big.png", false },
        { "ground.png", false },
        { "ground2.png", false },
        { "ground3.png", false },
        { "ground4.png", false },

        { "rock.png", false },
        { "rock2.png", false },
        { "rock3.png", false },
        { "rock4.png", false },
        { "rock5.png", false },
        { "rock6.png", false },
        { "rock7.png", false },

        { "white.png", false },
        { "cross.png", false },
        { "cross-2.png", false },
        { "score.png", false },
        { "score-2.png", false },

        { "font--.png", false },
        { "font-0.png", false },
        { "font-1.png", false },
        { "font-2.png", false },
        { "font-3.png", false },
        { "font-4.png", false },
        { "font-5.png", false },
        { "font-6.png", false },
        { "font-7.png", false },
        { "font-8.png", false },
        { "font-9.png", false },

        { "font-a.png", false },
        { "font-b.png", false },
        { "font-c.png", false },
        { "font-d.png", false },
        { "font-e.png", false },
        { "font-f.png", false },
        { "font-g.png", false },
        { "font-h.png", false },
        { "font-i.png", false },
        { "font-j.png", false },
        { "font-k.png", false },
        { "font-l.png", false },
        { "font-m.png", false },
        { "font-n.png", false },
        { "font-o.png", false },
        { "font-p.png", false },
        { "font-q.png", false },
        { "font-r.png", false },
        { "font-s.png", false },
        { "font-t.png", false },
        { "font-u.png", false },
        { "font-v.png", false },
        { "font-w.png", false },
        { "font-x.png", false },
        { "font-y.png", false },
        { "font-z.png", false },
        { "font-bang.png", false },
        { "la-title.png", false },
        { "la-title-a.png", false },
        { "la-title-b.png", false },

        { "w_mach.png", false },
        { "indicator.png", false },
        { "wall.png", false },
        { "keen-tile-2.png", false }
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
    const int SOUND_VOLUME = 128;

    SoundDef defs[] = {
       { "pickup.wav", SOUND_VOLUME },
       { "exp.wav", SOUND_VOLUME },
       { "expdeath.wav", SOUND_VOLUME },
       { "mach2.wav", SOUND_VOLUME },
       { "mach6-rapid.wav", SOUND_VOLUME },
       { "mach6-end.wav", SOUND_VOLUME },
       { "mach7.wav", 64 },
       { "mach8.wav", 64 },
       { "teleport.wav", SOUND_VOLUME },
       { "teleport2.wav", SOUND_VOLUME },
       { "health.wav", SOUND_VOLUME },
       { "hit.wav", SOUND_VOLUME },
       { "hit2.wav", SOUND_VOLUME },
       { "powerup.wav", SOUND_VOLUME },
       { "fall.wav", 76 },
       { "tele.wav", SOUND_VOLUME },
       { "mach2.wav", SOUND_VOLUME },
       { "mach.wav", 64 },
       { "rocket.wav", 64 }
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

void playOn(char* path, int channel) {
    Mix_PlayChannel(channel, getSound(path).sound, 0);
}

void play(char* path) {
    Mix_PlayChannel(-1, getSound(path).sound, 0);
}
