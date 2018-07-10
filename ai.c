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

double getDistance(Coord a, Coord b) {
	// source: http://www.mathwarehouse.com/algebra/distance_formula/index.php
	return sqrt(
		pow(a.x - b.x, 2) + 
		pow(a.y - b.y, 2)
	);
}

void shootAtOpponent(int enemyInc) {
	Coord target;
	double bestDistance = 1000;
	Coord usPos = enemies[enemyInc].coord;
	for(int i=0; i < MAX_ENEMY; i++) {
		if(enemies[i].coord.x == 0) continue;
		if(i == enemyInc) continue;				// don't shoot ourselves! :p

		// -------------------------------------
		// find out which enemy we're closest to
		// -------------------------------------
		Coord themPos = enemies[i].coord;
		double distance = getDistance(usPos, themPos);

		// we're the shortest so far!
		if(distance < bestDistance) {
			target = themPos;
			bestDistance = distance;
		}
	}

	// give us a chance to target the player too!
	if(getDistance(usPos, pos) < bestDistance) {
		target = pos;
	}

	// shoot at the chosen target
	fireShot(enemyInc, target);
}

double randomAngle() {
	return randomMq(0, 360);
}

Coord randomDir(Coord coord) {
	//Randomly set the target for the enemy, so they walk towards it.
	return deriveCoord(coord, chance(50) ? -100 : 100, chance(50) ? -100 : 100);
}

double avoidRight() {
	return degToRad(180);
}

double avoidLeft() {
	return degToRad(0);
}

double avoidTop() {
	return degToRad(90);
}

double avoidBottom() {
	return degToRad(270);

	// while(radToDeg(try) > 90 && radToDeg(try) < 315) try = randomEnemyAngle();
	// return try;
}

void aiSmartFrame(int enemyInc) {

//	shootAtOpponent(enemyInc);

	// double angle = radToDeg(enemies[enemyInc].idleTarget);
	// printf("%f\n", enemies[enemyInc].coord.x);

	// Time to change direction?
	if (timer(&enemies[enemyInc].lastDirTime, enemies[enemyInc].nextDirTime)) {
		enemies[enemyInc].nextDirTime = 500;
		enemies[enemyInc].idleTarget = randomEnemyAngle();
	}


	int border = 10;

	// border avoidance (might be nice to restore onScreen once reliable approach found)
	if(enemies[enemyInc].coord.x > screenBounds.x-border) 		enemies[enemyInc].idleTarget = avoidRight();
	else if(enemies[enemyInc].coord.x < border) 				enemies[enemyInc].idleTarget = avoidLeft();
	else if(enemies[enemyInc].coord.y > screenBounds.y-border) 	enemies[enemyInc].idleTarget = avoidBottom();
	else if(enemies[enemyInc].coord.y < border) 				enemies[enemyInc].idleTarget = avoidTop();

	// else if(!onScreen(enemies[enemyInc].coord, 10)) {

	// Walk towards homing direction
	Coord homeStep = getAngleStep(enemies[enemyInc].idleTarget, ENEMY_SPEED, false);
	enemies[enemyInc].coord.x += homeStep.x;
	enemies[enemyInc].coord.y += homeStep.y;

	// printf("%f - %f\n", enemies[0].idleTarget.x, enemies[0].idleTarget.y);
}
