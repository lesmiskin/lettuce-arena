#include "enemy.h"
#include "time.h"
#include "enemy.h"
#include "player.h"
#include "renderer.h"
#include "ai.h"
#include "input.h"

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

const double MIN_DIR_CHANGE = 500;
const double MAX_DIR_CHANGE = 3000;

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
	Coord usPos = enemies[enemyInc].coord;

	// What enemy are we closest to?
	for(int i=0; i < MAX_ENEMY; i++) {
		if(i == enemyInc) continue;				// don't shoot ourselves! :p
		if(enemies[i].coord.x == 0) continue;
		if(enemies[i].dead) continue;			// don't shoot corpses

		// find out which enemy we're closest to
		Coord themPos = enemies[i].coord;
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

void aiSmartFrame(int enemyInc) {

	// shoot opponent, with a little wait between each shot.
	if(canShoot(enemyInc) && isDue(clock(), enemies[enemyInc].lastShot, SHOT_RELOAD * randomMq(1, 3))) {
		shootAtOpponent(enemyInc);
	}

	// double angle = radToDeg(enemies[enemyInc].idleTarget);
	// printf("%f\n", enemies[enemyInc].coord.x);

	bool takingBreather = havingBreather(enemyInc);

	// Time to change direction?
	if (timer(&enemies[enemyInc].lastDirTime, enemies[enemyInc].nextDirTime)) {
		// Start a breather.
		if(chance(BREATHER_CHANCE) && !takingBreather) {
			enemies[enemyInc].lastBreather = clock();
		}
		enemies[enemyInc].nextDirTime = randomMq(MIN_DIR_CHANGE, MAX_DIR_CHANGE);
		enemies[enemyInc].idleTarget = randomEnemyAngle();
	}

	// stop the breather
	if(takingBreather && isDue(clock(), enemies[enemyInc].lastBreather, randomMq(250, 750))) {
		enemies[enemyInc].lastBreather = 0;
	}
	// take a breather
	else if(takingBreather) {
		return;
	}

	int border = 10;

	// border avoidance (might be nice to restore onScreen once reliable approach found)
	if(enemies[enemyInc].coord.x > screenBounds.x-border) 		enemies[enemyInc].idleTarget = avoidRight();
	else if(enemies[enemyInc].coord.x < border) 				enemies[enemyInc].idleTarget = avoidLeft();
	else if(enemies[enemyInc].coord.y > screenBounds.y-border) 	enemies[enemyInc].idleTarget = avoidBottom();
	else if(enemies[enemyInc].coord.y < border) 				enemies[enemyInc].idleTarget = avoidTop();

	// else if(!onScreen(enemies[enemyInc].coo
	// Walk towards homing direction
	Coord homeStep = getAngleStep(enemies[enemyInc].idleTarget, ENEMY_SPEED, false);
	enemies[enemyInc].coord.x += homeStep.x;
	enemies[enemyInc].coord.y += homeStep.y;

	// printf("%f - %f\n", enemies[0].idleTarget.x, enemies[0].idleTarget.y);
}
