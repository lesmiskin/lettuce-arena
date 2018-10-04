#include "enemy.h"
#include "lem.h"
#include "time.h"
#include "enemy.h"
#include "player.h"
#include "renderer.h"
#include "ai.h"
#include "input.h"
#include "scene.h"

// ai which does the following.
// moves in an X/Y position to...
// --- get a line of shooting sight with the player.
// --- stands there and shoots until i die.

const double RIGHT = 0;
const double DOWN_RIGHT = 45;
const double DOWN = 90;
const double DOWN_LEFT = 135;
const double LEFT = 180;
const double UP_LEFT = 215;
const double UP = 270;
const double UP_RIGHT = 315;

const int BREATHER_CHANCE = 20;

const double SPREAD = 5;	// how wide the targetting range is from the exact angle.

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
	for(int i=0; i < MAX_ENEMY; i++) {
		if(i == enemyInc) continue;				// don't shoot ourselves! :p
		if(lemmings[i].coord.x == 0) continue;
		if(lemmings[i].dead) continue;			// don't shoot corpses

		// find out which enemy we're closest to
		Coord themPos = lemmings[i].coord;
		double distance = getDistance(usPos, themPos);

		// we're the shortest so far!
		if(distance < bestDistance) {
			target = themPos;
			bestDistance = distance;
		}
	}

	// Throw player in the mix.
	if(getDistance(usPos, pos) < bestDistance) {
		target = pos;
	}

	// calculate the ANGLE between us and the opponent.
	double rad = getAngle(usPos, target);
	double deg = radToDeg(rad);

	// if the opponent is in range on any of the 8 directions, shoot him.
	     if(inRange(deg, RIGHT)) 		fireAngleShot(enemyInc, RIGHT);
	else if(inRange(deg, DOWN_RIGHT)) 	fireAngleShot(enemyInc, DOWN_RIGHT);
	else if(inRange(deg, DOWN)) 		fireAngleShot(enemyInc, DOWN);
	else if(inRange(deg, DOWN_LEFT)) 	fireAngleShot(enemyInc, DOWN_LEFT);
	else if(inRange(deg, LEFT)) 		fireAngleShot(enemyInc, LEFT);
	else if(inRange(deg, UP_LEFT)) 		fireAngleShot(enemyInc, UP_LEFT);
	else if(inRange(deg, UP)) 			fireAngleShot(enemyInc, UP);
	else if(inRange(deg, UP_RIGHT)) 	fireAngleShot(enemyInc, UP_RIGHT);
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

	// avoid other enemies (not working :p)
	// for(int i=0; i < MAX_ENEMY; i++) {
	// 	if(i == enemyInc || !lemmings[i].valid || lemmings[i].dead) continue;
	// 	Rect r = makeRect(lemmings[i].coord.x, lemmings[i].coord.y, 100, 100);
	// 	// printf("%f, %f, %d, %d, %f, %f, %f, %f\n", r.x, r.y, r.width, r.height, pos.x, pos.y, lemmings[enemyInc].coord.x, lemmings[enemyInc].coord.y);
	// 	// 	commands[CMD_QUIT] = true;
	// 	// 	return;

	// 	// if we're in enemy bounds, stop moving in that direction.
	// 	if(inBounds(lemmings[enemyInc].coord, r)) {
	// 		commands[CMD_QUIT] = true;
	// 		return;
	// 	}
	// }

	// searching for rocket launcher.
	if(!lemmings[enemyInc].hasRock) {

		// target nearest weapon.
		Coord target;
		double bestDistance = 1000;
		Coord usPos = lemmings[enemyInc].coord;

		// What enemy are we closest to?
		for(int i=0; i < MAX_WEAPONS; i++) {
			if(!weapons[i].valid || weapons[i].pickedUp) continue;

			// find out which enemy we're closest to
			Coord themPos = weapons[i].coord;
			double distance = getDistance(usPos, themPos);

			// we're the shortest so far!
			if(distance < bestDistance) {
				target = themPos;
				bestDistance = distance;
			}
		}

		double angle = getAngle(usPos, target);

		// home on it.
		lemmings[enemyInc].en_idleTarget = angle;		// face direction we're walking ih.
		Coord homeStep = getAngleStep(angle, ENEMY_SPEED, false);
		lemmings[enemyInc].coord.x += homeStep.x;
		lemmings[enemyInc].coord.y += homeStep.y;
		return;
	}

	// shoot opponent, with a little wait between each shot.
	if(canShoot(enemyInc) && isDue(clock(), lemmings[enemyInc].lastShot, SHOT_RELOAD * randomMq(1, 3))) {
		shootAtOpponent(enemyInc);
	}

	// double angle = radToDeg(lemmings[enemyInc].en_idleTarget);
	// printf("%f\n", lemmings[enemyInc].coord.x);

	bool takingBreather = havingBreather(enemyInc);

	// Time to change direction?
	if (timer(&lemmings[enemyInc].en_lastDirTime, lemmings[enemyInc].en_nextDirTime)) {
		// Start a breather.
		if(chance(BREATHER_CHANCE) && !takingBreather) {
			lemmings[enemyInc].en_lastBreather = clock();
		}
		lemmings[enemyInc].en_nextDirTime = randomMq(MIN_DIR_CHANGE, MAX_DIR_CHANGE);
		lemmings[enemyInc].en_idleTarget = randomEnemyAngle();
	}

	// stop the breather
	if(takingBreather && isDue(clock(), lemmings[enemyInc].en_lastBreather, randomMq(250, 750))) {
		lemmings[enemyInc].en_lastBreather = 0;
	}
	// take a breather
	else if(takingBreather) {
		return;
	}

	int border = 10;

	// border avoidance (might be nice to restore onScreen once reliable approach found)
	if(lemmings[enemyInc].coord.x > screenBounds.x-border) 		lemmings[enemyInc].en_idleTarget = avoidRight();
	else if(lemmings[enemyInc].coord.x < border) 				lemmings[enemyInc].en_idleTarget = avoidLeft();
	else if(lemmings[enemyInc].coord.y > screenBounds.y-border) 	lemmings[enemyInc].en_idleTarget = avoidBottom();
	else if(lemmings[enemyInc].coord.y < border) 				lemmings[enemyInc].en_idleTarget = avoidTop();

	// Walk towards homing direction
	Coord homeStep = getAngleStep(lemmings[enemyInc].en_idleTarget, ENEMY_SPEED, false);
	lemmings[enemyInc].coord.x += homeStep.x;
	lemmings[enemyInc].coord.y += homeStep.y;

	// printf("%f - %f\n", lemmings[0].en_idleTarget.x, lemmings[0].en_idleTarget.y);
}
