#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "Actor.h"
#include "GameWorld.h"
#include "GraphObject.h"
#include <string>
#include <vector>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
	// Constructor
	StudentWorld(std::string assetDir);

	// Mutator(s)
	virtual int init();
	virtual int move();
	virtual void cleanUp();

	void reduceNumProtesters() { m_numProtesters--; }
	void reduceOil()
	{
		if (getOil() > 0)
		{
			m_oil--;
		}
	}

	// Destructor
	virtual ~StudentWorld();

	//////////////////////
	// Helper Functions //
	//////////////////////

	// Removes any overlapping 4x4 Earth objects from (x,y)
	void clearEarth(const int x, const int y, Actor::IDType type);

	// Checks if there are any Earth objects immediately below the boulder's position
	bool checkEarthBelow(const int x, const int y);

	// Checks if tunnelman is within radius r of an actor
	bool withinRadiusOfTunnelman(const int x, const int y, const double r);

	// Searches for a tunnelman within a radius r from an actor a
	Tunnelman* searchForTunnelman(const Actor* a, const double r);

	// Searches for a regular or hardcore protester within a radius of 3 to pick up a gold nugget
	bool searchForProtester(const Actor* a);

	// Checks if (x,y) overlaps any 4x4 Earth objects or a Boulder
	bool noOverlap(const int x, const int y, const Actor* actor);

	// Checks if player is in a straight horizontal or vertical line of sight to Tunnelman
	GraphObject::Direction withinLineOfSight(const Actor* actor);

	// Drop temporary gold nugget at tunnelman's position in the oil field
	bool dropGold();

	// All hidden game objects that are within a radius of 12 will be revealed to the player
	void reveal();

	// Creates a squirt object in the oil field
	void createSquirt(const int x, const int y, GraphObject::Direction dir);

	// Hit protesters when a boulder is within a radius of 3.0 of it
	void hitProtesters(const Actor* actor);

	// Squirt water at protesters when a boulder is within a radius of 3.0 of it
	bool squirtProtesters(const Actor* actor);

	// Returns the distance of the shortest path from (startX, startY) to a position (endX, endY)
	int shortestPath(Actor* actor, const int startX, const int startY, const int endX, const int endY);

	// Returns the x-coordinate of tunnelman
	int tunnelmanGetX() const;

	// Returns the y-coordinate of tunnelman
	int tunnelmanGetY() const;

private:
	int getOil() const { return m_oil; }
	int getNumProtesters() const { return m_numProtesters; }
	int getTicks() const { return m_ticks; }
	void addOil() { m_oil++; }
	void resetOil() { m_oil = 0; }
	void addNumProtesters() { m_numProtesters++; }
	void resetNumProtesters() { m_numProtesters = 0; }
	void addTicks() { m_ticks++; }
	void setTicks(int ticks) { m_ticks = ticks; }

	// Displays the necessary text above the game
	void setDisplayText();

	// Removes Earth at (x, y)
	void removeEarth(const int x, const int y);

	// Calculates the Euclidian distance between two coordinates
	double radius(const double x1, const double y1, const double x2, const double y2) const;

	// Checks if (x,y) overlaps any 4x4 Earh objects
	bool noEarth(const int x, const int y);

	// Checks if (x,y) overlaps any Boulder objects
	bool noBoulder(const int x, const int y, const Actor* actor);

	// Checks if (x,y) overlaps any Goodies (e.g. Water Pool, Gold Nuggets, etc.)
	bool noItem(const int x, const int y);

	// Generates random coordinates for an actor
	void randomCoordinates(int& x, int& y, Actor::IDType type);

	// Generates random coordinates for a water pool (function overloading)
	void randomCoordinates(int& x, int& y);

	// Adds a regular or hardcore protester in the oil field
	void addProtester();

	int m_oil;
	int m_numProtesters;
	int m_ticks;
	Earth* m_field[64][64];
	Tunnelman* m_tunnelman;
	std::vector<Actor*> m_actors;

	struct Point
	{
		Point(int x, int y, int distance = 0)
		{
			m_x = x;
			m_y = y;
			m_distance = distance;
		}

		int m_x;
		int m_y;
		int m_distance;
	};
};

#endif // STUDENT_WORLD_H
