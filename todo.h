
// melee fist or pistol.
// strafe ability.
// fix 'walk through wall' weapon homing.




// TODO: Game will sometimes freeze immediately upon launch (no rendered frame).
// - A rebuild fixes it, after a while?
// - Something to do with ground texture rendering. 

// initial screen (basic state integer toggle)
// - on keypress, changes to actual game.
// - on esc press, changes to title.
// - on changing to game, everything resets.

// title:
// - starry screen.
// - logo slams down with explosions.
// - two lemmings spawn and march around the logo.


// title:
// - starry screen with platform.
// - 


// - on title show, show screen with stars behind.
// - on title show, little platform that lemmings spawn on.
// - spawns happen repeatedly until at-most two, or at intervals.







// holes in map, walls, weapons, spawn locations -- all defined in TEXT definition.
// walls have shadowed edges to them.

// lems cannot cross walls.
// walls are defined per-quadrant.
// lems will not fire across walls.
// lems decide to move into other quadrant if bored.








// melee fist

// maps defined in text.
// weapons and powerups bob with shadow.

// up/down lemming animation
// double animation frames
// lemmings pallete swap

// chaingun instead of machinegun
// spinning rocket.

// colors for powerup text + frag text
// skull icon on killer (whoever it is)
// skull icon next to frags in bar.

// machinegun icon in bar is off-center







// * logic to "pause" after a shot is broken. only affects shooting decision, not whether to stand still.

// FUN TO DO
// WALLS ON THE MAPS: different maps, different walls.
// different floor texture

// INTERESTING
// special devices like lemmings/quake: spring pushers, crushers.
// weapon tube outputs (innovation)
// launcher spring (devices like lemmings)

// GOOD
// better machinegun handling for enemies (stop when shooting)
// 3-way shotgun with satisfying SFX
// bumpers that push onto powerup platform
// toy with strafe key (current structure map be too limiting)
// jumping (causes fast)
// sprite crouches down when firing.
// thin walkways to fall off, space in background.
// pistol weapon
// melee fist (alex kidd whapping)
// show local ammo indicator when pickup ammo powerup
// shell flip animatino when firing machinegun.
// up/down walking animations

// MUST BE DONE
// machinegun sprite

// MEH
// bazooka sprite
// middle floor is transparent with "shine" graphic (NO ALPHA)
// restore rocket spinning
// larger rocket puffs
// monster cast of characters (innovation)
// map themes, graveyeard, space (innovation)
// secondary weapon (e.g. trap, pipe bomb)
// dance dungeon-style indicators

// ARCHIVE
// large corner arenas for battles
// center pathway has powerup (precarious)

// teleporters?
// layout with 5 quads:
// /---+---+---\
// |   |   |   |
// +---+---+---+
// |   | P |   |
// +---+---+---+
// |   |   |   |
// \---+---+---/

// ---------------------------------
// |                                |
// |                                |
// |        .      P     .          |
// |                                |
// |                                |
// |                                |
// |      R        P         R      |
// |                                |
// |                                |
// |                                |
// |                                |
// |      R        P         R      |
// |                                |
// |                                |
// |---------------------------------

// 32 cells
// 14 cells

// - will home to weapon onscreen if:
	// - no weapon currently.

// - will move to another quadrant if:
	// - been there for ages (look around for different action, powerups etc).
	// - no lemming is in the quadrant, and wouldn't benefit from getting any weapon or powerup here.
	// - low health and there are enemies there which could kill us
	// - no weapon (i.e. decided against a home on original screen)

// - if enemies onscreen and have weap / weapon is onscreen, 75% chance to stay onscreen (combat logic).

// if health 

// - home to weapon if have none.								SETUP
// - 50/50 chance home to other screen if have none.			SETUP
// - noone in quadrant -> go to another							HUNT

// lemming decides to visit other quadrant in these situations:
// - when in that half of the screen (50/50 chance)
// - when been in current quadrant for a while (10 seconds)
// - when low health (wanting to escape + no health powerup in this screen)
// - when no weapon (wanting to escape + no weapon in this screen)
// - when noone to fight (wanting a fight)
// - when most players are elsewhere (wanting a bigger fight)

// nicer powerup bubbles (flashing and bobbing)
// weapons should be in powerup bubbles too (obvious)
// more ammo, ammo powerup gives rapid fire.

// double the animation frames.
// pallete swap rather than recolor
// pickup text stays onscreen for longer.
// restore gibs and blood when monsters come back.
// align powerups with tile (e.g. middle of tile)
// enemies home to powerups if nearby

// quadrants you can move into, stepping over threshold.
// nicer floors


// powerup: uzi (double speed, fast projectile)
// powerup: cluster rockets (three rockets at a time)

// strafe button / auto-strafe
// default pistol or fist (enlarges ala Alex Kidd)
// bazooka sprite
// ak-47 or chaingun

// cast of characters: lettuce, vampire, octopus, eyeball, yorp/garg, frank, skullman, deepone, hairball,  (orbb), mib
// character select screen
// intro mode cuts between action shots in different arenas.
// ctf mode
// split screen mode
// 30 maps (get friends to make)
// themes (e.g. graveyard, space)

// pistol when starting
// machinegun carry sprite
// large white puffs for rockets.
// rocket more solid and vivid.
// different death on machinegun.
// BOUNCING PIXELS on explosion.
// reinstate nice green indicator arrows (bounce).
// strafe button.

// double animation frames
// up-down walking sprites.

// weapons in crates
// jump and bounce if machinegun death.
// "you fragged x" text is not actually centered.
// ak-47 carry graphic
// bazooka graphic
// spawn point textures

// random crates drops with powerups
// new bazooka sprite (pipe)
// ak-47 carry sprite
// new character ip
// double character animation.
// up/down sprites
// palette swap for skins
// model map off q3dm from top-down
// sounds

// "killed x" to be in blue
// "fragged by x" to be in red
// "1st place with x" to be in blue
// still say 1st even if tied
// scoreboard highlight to be white/blue based on lead.
// ensure "tied for lead", and tied for 2nd count as blue and red.
// gothic / hell map with lava and rocks

// dynamite / bomb
// figure out why enemy shots end up going in WOBBLY dir still.
// up/down character sprites
// bug: stop enemies not clipping when homing to rockets.
// bug: stop enemies cheating on angle when homing.

// opt: Scrolling stars are VERY unoptimised. replace with scrolling bg's?
// Fade in / fade out

// up/down movement gif's

// WALLS -----------------------------
// wall segments in middle of the map.
// array-style wall definition.
// monsters wont shoot through walls.
// monsters will deflect if bumping against a wall.

// zelda-esque map sections, inspired from q3dm17 (outside area, inner area etc.)
// BUG: enemies can (still) get stuck on one another.







// enemies walk through players when homing into rockets.
// enemies cheat by homing directly to rockets rather than legally walking.


// rocket is too high-color, reduce it to be more pixelly.
// quad damage powerup
// pools of lava















// - if we are NEAR a wall (i.e. block of two or more, then MOVE AROUND IT)
// directional gibs based on rocket direction.
// death/explosion gets pushed back, like in regular hits.

// powerup dropins (ala worms)
// all pickups are in wood crates that you bash open (ala gta)

// inspiration: worms (names, crates, powerups)
// inspiration: quake 3 arena (respawning, pace, weapons, scoreboards)
// inspiration: gta (crates, machinegun)
// inspiration: lemmings (explosions, sprites)




// when killed, or KILL, show that lemming's name in yellow.
// demo mode (game can play itself -- spectator mode)
// shooting animation (overlaid muzzle-flash sprite)
// spawn points have a funny tile (e.g. quake 2 teleporter)

// icon when killed, or we frag someone
// "frags" and "deaths"
// no. of deaths on scoreboard
// blue lettering when killing / 1st place.
// red lettering when dead

// frag plumes above enemies that just killed someone

// title screen:
// - double-sized lettering
// - flashing "press any key"
// - rippling bright lettering.









// ((
// gameplay tweaks: player speed.
// gameplay tweaks: bounding box.
// ))

// sound effects
// add a wall block.


// time limit + counter

// game.cfg (fraglimit, time limit)
// botnames.cfg
// maps.cfg
// keys.cfg (arrows vs. wsad, ctrl vs other keys)

// simple punch to bash enemies around with






// powerup... invincibility.
// powerup... splash damage.
// gameplay: experiment with ammo + health bars



// dont overlap players
// title screen with headlamp rotation ala commander keen.
// ability to PUNCH (alex kidd style)
// stop enemy cheating with angular motion towards weapons
// annoying / unclear what happens when enemy grabs gun then shoots immediately and we respawn immediately
// ability to customise text (w/ colors)
// spinning rockets again.
// [easy] health packs
// [easy] spinning rocket sprites.
// [easy] 1-frame large muzzleflash.
// [easy] up/down walking sprites.
// player bumps back if part of an explosion.
// splash damage?




// ability to scroll, and enter different areas
// simple walls for strategy.
// enemies don't shoot if wall is in the way.


    // . . . .         . . . .
    // .                     .
    // .                     .
    // .                     .

    // .                     .
    // .                     .
    // .                     .
    // . . . .         . . . .

// QUAD DAMAGE powerup spawns infrequently in middle of map.

// chairs and computer desks lying around.
// office theme (worker sprites, chairs, desks with computers)
// office worker sprite.

// realistic walking styles:
// a) walk in a random direction for a random amount of time, then change direction (officespace-style).
// c) while walking, 50/50 chance to zig-zag on the way.
// OR
// a) pick a random place on the map, and walk to it.

// reactions:
// a) if pickup is available, high chance to walk to it.
// b) if weapon is available, higher chance to walk to it.
// c) if detect opponent within X threshold (random), then change direction AWAY from him.
