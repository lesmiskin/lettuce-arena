#include "lem.h"
#include "time.h"
#include "player.h"
#include "renderer.h"
#include "ai.h"
#include "input.h"
#include "scene.h"
#include "weapon.h"

// ai which does the following.
// moves in an X/Y position to...
// --- get a line of shooting sight with the player.
// --- stands there and shoots until i die.

const double MIN_DIR_CHANGE = 500;
const double MAX_DIR_CHANGE = 3000;

const int RIGHT = 0;
const int DOWN_RIGHT = 45;
const int DOWN = 90;
const int DOWN_LEFT = 135;
const int LEFT = 180;
const int UP_LEFT = 215;
const int UP = 270;
const int UP_RIGHT = 315;

static const int BAR_SIZE = 10;

const int BREATHER_CHANCE = 20;

const double SPREAD = 5;	// accuracy threshold. how wide the targetting range is from the exact angle.

double randomAngle() {
	// Random 8-way direction.

	// return degToRad(chance(50) ? 0 : 180);
	// return degToRad(chance(50) ? 90 : 135);

	double deg;
	switch(randomMq(0, 7)){
		case 1:
			deg = 45;
			break;
		case 2:
			deg = 90;
			break;
		case 3:
			deg = 135;
			break;
		case 4:
			deg = 180;
			break;
		case 5:
			deg = 225;
			break;
		case 6:
			deg = 270;
			break;
		case 7:
			deg = 315;
			break;
		default:
			deg = 0;
			break;
	}

	return degToRad(deg);
}

// accuracy threshold
bool inRange(double deg, double target) {
	return deg >= target-SPREAD && deg <= target+SPREAD; 
}

void shootAtOpponent(int enemyInc) {
	// printf("%f", radToDeg(degToRad(270)));
	// commands[CMD_QUIT] = true;

	Coord target;
	double bestDistance = 1000;
	Coord usPos = lemmings[enemyInc].coord;

	// What enemy are we closest to?
	for(int i=0; i < MAX_LEM; i++) {
		if(i == enemyInc) continue;				// don't shoot ourselves! :p
		if(lemmings[i].coord.x == 0) continue;
		if(lemmings[i].dead) continue;			// don't shoot corpses
		if(lemmings[i].quadrant != lemmings[enemyInc].quadrant) continue;

		// find out which enemy we're closest to
		Coord themPos = lemmings[i].coord;
		double distance = getDistance(usPos, themPos);

		// we're the shortest so far!
		if(distance < bestDistance) {
			target = themPos;
			bestDistance = distance;
		}
	}

	// calculate the ANGLE between us and the opponent.
	double rad = getAngle(usPos, target);
	double deg = radToDeg(rad);

	// if the opponent is in range on any of the 8 directions, shoot him.
	     if(inRange(deg, RIGHT)) 		shoot(enemyInc, RIGHT);
	else if(inRange(deg, DOWN_RIGHT)) 	shoot(enemyInc, DOWN_RIGHT);
	else if(inRange(deg, DOWN)) 		shoot(enemyInc, DOWN);
	else if(inRange(deg, DOWN_LEFT)) 	shoot(enemyInc, DOWN_LEFT);
	else if(inRange(deg, LEFT)) 		shoot(enemyInc, LEFT);
	else if(inRange(deg, UP_LEFT)) 		shoot(enemyInc, UP_LEFT);
	else if(inRange(deg, UP)) 			shoot(enemyInc, UP);
	else if(inRange(deg, UP_RIGHT)) 	shoot(enemyInc, UP_RIGHT);
}

// Note: most sensible to do perpendicular avoidance, lest we wedge into a corner.

double avoidRight() {
	return degToRad(LEFT);
}

double avoidLeft() {
	return degToRad(RIGHT);
}

double avoidTop() {
	return degToRad(DOWN);
}

double avoidBottom() {
	return degToRad(UP);
}

// TODO: Would be nice to have a "find closest" function.

void aiSmartFrame(int enemyInc) {
	if(DEBUG_STILL_ENEMIES) return;

	// searching for rocket launcher.
	if(lemmings[enemyInc].weap == 0) {

		// target nearest weapon.
		Coord target;
		double bestDistance = 1000;
		Coord usPos = lemmings[enemyInc].coord;

		bool anyWeapon = false;

		// What weapon are we closest to?
		for(int i=0; i < MAX_WEAPONS; i++) {
			if(!weapons[i].valid || weapons[i].pickedUp) continue;
			if(weapons[i].quadrant != lemmings[enemyInc].quadrant) continue;

			// don't home on items (yet!)
			if(weapons[i].type != W_ROCK && weapons[i].type != W_MACH) continue;

			// find out which enemy we're closest to
			Coord themPos = weapons[i].coord;
			double distance = getDistance(usPos, themPos);

			// we're the shortest so far!
			if(distance < bestDistance) {
				target = themPos;
				bestDistance = distance;
			}

			anyWeapon = true;
		}

		// home to it
		if(anyWeapon) {
			double angle = getAngle(usPos, target);

			// home on it.
			lemmings[enemyInc].angle = angle;		// face direction we're walking ih.
			Coord homeStep = getAngleStep(angle, LEM_SPEED, false);
			lemmings[enemyInc].coord.x += homeStep.x;
			lemmings[enemyInc].coord.y += homeStep.y;
			return;
		}
	}

	// shoot opponent, with a little wait between each shot.
	if(canShoot(enemyInc) && isDue(clock(), lemmings[enemyInc].lastShot, getReloadTime(enemyInc) * randomMq(1, 3))) {
		shootAtOpponent(enemyInc);
	}

	// double angle = radToDeg(lemmings[enemyInc].angle);
	// printf("%f\n", lemmings[enemyInc].coord.x);

	bool takingBreather = havingBreather(enemyInc);

	Lem l = lemmings[enemyInc];
	
	// lem stops for a bit after firing the launcher, otherwise looks too frenzied.
	if(l.weap == W_ROCK && !isDue(clock(), lemmings[enemyInc].lastShot, 300)) {
		return;
	}

	// Time to change direction?
	if (timer(&lemmings[enemyInc].en_lastDirTime, lemmings[enemyInc].en_nextDirTime)) {
		// Start a breather.
		if(chance(BREATHER_CHANCE) && !takingBreather) {
			lemmings[enemyInc].en_lastBreather = clock();
		}
		lemmings[enemyInc].en_nextDirTime = randomMq(MIN_DIR_CHANGE, MAX_DIR_CHANGE);
		lemmings[enemyInc].angle = randomAngle();
	}

	// stop the breather
	if(takingBreather && isDue(clock(), lemmings[enemyInc].en_lastBreather, randomMq(250, 750))) {
		lemmings[enemyInc].en_lastBreather = 0;
	}
	// take a breather
	else if(takingBreather) {
		return;
	}

	// Walk towards homing direction
	Coord homeStep = getAngleStep(lemmings[enemyInc].angle, LEM_SPEED, false);

	// move if we can.
	Coord hope = makeCoord(lemmings[enemyInc].coord.x + homeStep.x, lemmings[enemyInc].coord.y + homeStep.y);
	Move m = tryMove(hope, lemmings[enemyInc].coord, enemyInc);
	lemmings[enemyInc].coord = m.result;

	// if we hit something, try to move away from it on the next frame.
	if(!m.freeDir.up) 		lemmings[enemyInc].angle = avoidTop();
	if(!m.freeDir.down) 	lemmings[enemyInc].angle = avoidBottom();
	if(!m.freeDir.left) 	lemmings[enemyInc].angle = avoidLeft();
	if(!m.freeDir.right) 	lemmings[enemyInc].angle = avoidRight();
	// printf("%f - %f\n", lemmings[0].angle.x, lemmings[0].angle.y);
}