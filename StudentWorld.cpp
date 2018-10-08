#include "Actor.h"
#include "GameConstants.h"
#include "GameWorld.h"
#include "GraphObject.h"
#include "StudentWorld.h"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

/////////////////
// Constructor //
/////////////////

StudentWorld::StudentWorld(string assetDir)
	: GameWorld(assetDir)
{
	m_oil = 0;
	m_numProtesters = 0;
	m_ticks = 0;
}

////////////////
// Destructor //
////////////////

StudentWorld::~StudentWorld()
{
	// Clean up oil field

	for (size_t x = 0; x < VIEW_WIDTH; x++)
	{
		for (size_t y = 0; y < VIEW_HEIGHT; y++)
		{
			delete m_field[x][y];
		}
	}

	// Clean up tunnelman

	delete m_tunnelman;

	// Clean up actors

	vector<Actor*>::iterator it = m_actors.begin();
	while (it != m_actors.end())
	{
		delete *it;
		it = m_actors.erase(it);
	}
}

////////////////////////////
// init() Implementations //
////////////////////////////

int StudentWorld::init()
{
	// Set up field

	for (size_t x = 0; x < VIEW_WIDTH; x++)
	{
		for (size_t y = 0; y < VIEW_HEIGHT; y++)
		{
			m_field[x][y] = nullptr;
		}
	}

	// Create oil field
	for (size_t i = 0; i < VIEW_WIDTH; i++)
	{
		for (size_t j = 0; j < 60; j++)
		{
			m_field[i][j] = new Earth(this, i, j);
		}
	}

	// Create tunnel

	for (size_t k = 30; k < 34; k++)
	{
		for (size_t l = 4; l < 60; l++)
		{
			removeEarth(k, l);
		}
	}

	// Create Tunnelman

	m_tunnelman = new Tunnelman(this);

	// Generating boulders

	int B = min(static_cast<int>(getLevel()) / 2 + 2, 9);

	for (size_t i = 0; i < B; i++)
	{
		int x = 0;
		int y = 0;
		randomCoordinates(x, y, Actor::boulder);
		Boulder* newBoulder = new Boulder(this, x, y);
		m_actors.push_back(newBoulder);
		clearEarth(x, y, newBoulder->getType());
	}

	// Generate gold nuggets

	int G = max(5 - static_cast<int>(getLevel()) / 2, 2);

	for (size_t j = 0; j < G; j++)
	{
		int x = 0;
		int y = 0;
		randomCoordinates(x, y, Actor::item);
		Gold* newGold = new Gold(this, Item::permanent, x, y);
		m_actors.push_back(newGold);
	}

	// Generating oil barrels

	int L = min(2 + static_cast<int>(getLevel()), 21);

	for (size_t k = 0; k < L; k++)
	{
		int x = 0;
		int y = 0;
		randomCoordinates(x, y, Actor::item);
		Oil* newOil = new Oil(this, x, y);
		m_actors.push_back(newOil);
		addOil();
	}

	// Generate new protester at the very first tick

	addProtester();

	return GWSTATUS_CONTINUE_GAME;
}

////////////////////////////
// move() Implementations //
////////////////////////////

int StudentWorld::move()
{
	// Update display text

	setDisplayText();

	// Give each actor a chance to do something

	vector<Actor*>::iterator i = m_actors.begin();
	while (i != m_actors.end())
	{
		if (m_tunnelman->isAlive())
		{
			(*i)->doSomething();

			if (!m_tunnelman->isAlive())
			{
				decLives();
				return GWSTATUS_PLAYER_DIED;
			}
			else if (getOil() == 0)
			{
				playSound(SOUND_FINISHED_LEVEL);
				return GWSTATUS_FINISHED_LEVEL;
			}
		}

		i++;
	}

	// Give tunnelman a chance to do something

	m_tunnelman->doSomething();

	// Adding protesters to the oil field

	int T = max(25, 200 - static_cast<int>(getLevel()));
	int P = min(15, static_cast<int>(2 + static_cast<int>(getLevel()) * 1.5));

	if (getTicks() >= T && getNumProtesters() < P)
	{
		addProtester();
		setTicks(0);
	}

	// Adding new water pools or sonar kits to the oil field

	int G = static_cast<int>(getLevel()) * 25 + 300;
	int G_chance = rand() % G;

	if (G_chance == 0)
	{
		int Sonar_chance = rand() % 5;
		int Time = max(100, 300 - 10 * static_cast<int>(getLevel()));

		if (Sonar_chance == 0)
		{
			bool isPossible = true;

			if (!noItem(0, 60))
			{
				isPossible = false;
			}

			if (isPossible)
			{
				Sonar* newSonar = new Sonar(this, Time);
				m_actors.push_back(newSonar);
			}
		}
		else
		{
			int x = 0;
			int y = 0;
			randomCoordinates(x, y);
			Water* newWater = new Water(this, Time, x, y);
			m_actors.push_back(newWater);
		}
	}

	// Remove newly-dead actors after each tick

	vector<Actor*>::iterator k = m_actors.begin();
	while (k != m_actors.end())
	{
		if (!(*k)->isAlive())
		{
			delete *k;
			k = m_actors.erase(k);
		}
		else
		{
			k++;
		}
	}

	// Return the proper result

	if (!(m_tunnelman->isAlive()))
	{
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}

	// If the player has collected all of the barrels on the level, then
	// return the result that the player finished the level

	if (getOil() == 0)
	{
		playSound(SOUND_FINISHED_LEVEL);
		return GWSTATUS_FINISHED_LEVEL;
	}

	addTicks();

	if (getTicks() >= 10000)
	{
		setTicks(200);
	}

	// The player hasn't completed the current level and hasn't died
	// let them continue playing the current level

	return GWSTATUS_CONTINUE_GAME;
}

///////////////////////////////
// cleanUp() Implementations //
///////////////////////////////

void StudentWorld::cleanUp()
{
	// Set oil barrels to zero

	resetOil();

	// Set number of protesters to zero

	resetNumProtesters();

	// Set ticks to zero

	setTicks(0);

	// Clean up oil field

	for (size_t x = 0; x < VIEW_WIDTH; x++)
	{
		for (size_t y = 0; y < VIEW_HEIGHT; y++)
		{
			delete m_field[x][y];
		}
	}

	// Clean up tunnelman

	delete m_tunnelman;

	// Clean up actors

	vector<Actor*>::iterator it = m_actors.begin();
	while (it != m_actors.end())
	{
		delete *it;
		it = m_actors.erase(it);
	}

	return;
}

//////////////////////
// Helper Functions //
//////////////////////

void StudentWorld::setDisplayText()
{
	int level = getLevel();
	int lives = getLives();
	int health = m_tunnelman->getHealth() * 10;
	int squirts = m_tunnelman->getSquirt();
	int gold = m_tunnelman->getGold();
	int sonar = m_tunnelman->getSonar();
	int barrelsLeft = getOil();
	int score = getScore();

	ostringstream oss;

	oss << "Lvl: " << setw(2) << level << "  ";
	oss << "Lives: " << lives << "  ";
	oss << "Hlth: " << setw(3) << health << "%  ";
	oss << "Wtr: " << setw(2) << squirts << "  ";
	oss << "Gld: " << setw(2) << gold << "  ";
	oss << "Oil Left: " << setw(2) << barrelsLeft << "  ";
	oss << "Sonar: " << setw(2) << sonar << "  ";
	oss << "Scr: " << setfill('0') << setw(6) << score;

	setGameStatText(oss.str());
}

void StudentWorld::removeEarth(const int x, const int y)
{
	if (m_field[x][y] != nullptr)
	{
		delete m_field[x][y];
		m_field[x][y] = nullptr;
	}

	return;
}

void StudentWorld::clearEarth(const int x, const int y, Actor::IDType type)
{
	if ((x < 0 || x > 60) || (y < 0 || y > 60))
	{
		return;
	}

	bool removedOnce = false;

	for (size_t i = x; i < (x + 4); i++)
	{
		for (size_t j = y; j < (y + 4); j++)
		{
			if (m_field[i][j] != nullptr)
			{
				removeEarth(i, j);
				removedOnce = true;
			}
		}
	}

	if (removedOnce && type == Actor::tunnelman)
	{
		playSound(SOUND_DIG);
	}
}

bool StudentWorld::checkEarthBelow(const int x, const int y)
{
	if ((x < 0 || x > 60) || (y < 0 || y > 60))
	{
		return false;
	}

	for (size_t i = x; i < (x + 4); i++)
	{
		if (m_field[i][y] != nullptr)
		{
			return true;
		}
	}

	return false;
}

double StudentWorld::radius(const double x1, const double y1, const double x2, const double y2) const
{
	return sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
}

bool StudentWorld::withinRadiusOfTunnelman(const int x, const int y, double r)
{
	if (radius(x, y, tunnelmanGetX(), tunnelmanGetY()) <= r)
	{
		return true;
	}
	else
	{
		return false;
	}
}

Tunnelman* StudentWorld::searchForTunnelman(const Actor* a, const double r)
{
	if (withinRadiusOfTunnelman(a->getX(), a->getY(), r))
	{
		return m_tunnelman;
	}
	else
	{
		return nullptr;
	}
}

bool StudentWorld::searchForProtester(const Actor* a)
{
	vector<Actor*>::iterator it = m_actors.begin();
	while (it != m_actors.end())
	{
		if ((*it)->getType() == Actor::protester && (*it)->getState() == Actor::stable)
		{
			if (radius(a->getX(), a->getY(), (*it)->getX(), (*it)->getY()) <= 3.0)
			{
				Protester* protester = dynamic_cast<Protester*>((*it));
				protester->pickGold();
				return true;
			}
		}

		it++;
	}

	return false;
}

bool StudentWorld::noEarth(const int x, const int y)
{
	for (size_t i = x; i < (x + 4); i++)
	{
		for (size_t j = y; j < (y + 4); j++)
		{
			if (m_field[i][j] != nullptr)
			{
				return false;
			}
		}
	}

	return true;
}

bool StudentWorld::noBoulder(const int x, const int y, const Actor* actor = nullptr)
{
	vector<Actor*>::iterator it = m_actors.begin();
	while (it != m_actors.end())
	{
		if (*it != actor && (*it)->getType() == Actor::boulder)
		{
			if (actor != nullptr)
			{
				if (actor->getType() == Actor::protester && actor->getState() == Actor::leaving && (*it)->getState() == Actor::falling)
				{
					it++;
					continue;
				}
			}

			if (radius(x, y, (*it)->getX(), (*it)->getY()) < 4.0 || radius(x, y, (*it)->getX(), (*it)->getY()) == radius(0, 0, 3, 3))
			{
				return false;
			}
		}

		it++;
	}

	return true;
}

bool StudentWorld::noItem(const int x, const int y)
{
	vector<Actor*>::iterator it = m_actors.begin();
	while (it != m_actors.end())
	{
		if ((*it)->getType() == Actor::item)
		{
			if (radius(x, y, (*it)->getX(), (*it)->getY()) < 4.0 || radius(x, y, (*it)->getX(), (*it)->getY()) == radius(0, 0, 3, 3))
			{
				return false;
			}
		}

		it++;
	}

	return true;
}

bool StudentWorld::noOverlap(const int x, const int y, const Actor* actor = nullptr)
{
	if ((x < 0 || x > 60) || (y < 0 || y > 60))
	{
		return false;
	}

	if (actor == nullptr)
	{
		if (!noEarth(x, y))
		{
			return false;
		}
	}
	else
	{
		if (actor->getType() != Actor::tunnelman)
		{
			if (!noEarth(x, y))
			{
				return false;
			}
		}
	}

	if (!noBoulder(x, y, actor))
	{
		return false;
	}

	return true;
}

GraphObject::Direction StudentWorld::withinLineOfSight(const Actor* actor)
{
	if ((actor->getX() == tunnelmanGetX()) && (actor->getY() == tunnelmanGetY()))
	{
		return GraphObject::none;
	}

	if (actor->getX() == tunnelmanGetX())
	{
		if (actor->getY() < tunnelmanGetY())
		{
			for (int y = actor->getY(); y <= tunnelmanGetY(); y++)
			{
				if (!noOverlap(actor->getX(), y, actor))
				{
					return GraphObject::none;
				}
			}

			return GraphObject::up;
		}
		else
		{
			for (int y = actor->getY(); y >= tunnelmanGetY(); y--)
			{
				if (!noOverlap(actor->getX(), y, actor))
				{
					return GraphObject::none;
				}
			}

			return GraphObject::down;
		}
	}
	else if (actor->getY() == tunnelmanGetY())
	{
		if (actor->getX() < tunnelmanGetX())
		{
			for (int x = actor->getX(); x <= tunnelmanGetX(); x++)
			{
				if (!noOverlap(x, actor->getY(), actor))
				{
					return GraphObject::none;
				}
			}

			return GraphObject::right;
		}
		else
		{
			for (int x = actor->getX(); x >= tunnelmanGetX(); x--)
			{
				if (!noOverlap(x, actor->getY(), actor))
				{
					return GraphObject::none;
				}
			}

			return GraphObject::left;
		}
	}

	return GraphObject::none;
}

void StudentWorld::randomCoordinates(int& x, int& y, const Actor::IDType type)
{
	bool isPossible;

	do
	{
		isPossible = true;

		x = rand() % 61;

		if (type == Actor::boulder)
		{
			y = rand() % 37 + 20;
		}
		else
		{
			y = rand() % 57;
		}

		if ((x >= 27 && x <= 33) && (y >= 1 && y <= 59))
		{
			isPossible = false;
			continue;
		}

		vector<Actor*>::iterator it = m_actors.begin();
		while (it != m_actors.end())
		{
			if (radius(x, y, (*it)->getX(), (*it)->getY()) <= 6.0)
			{
				isPossible = false;
				break;
			}

			it++;
		}
	} while (isPossible == false);

	return;
}

void StudentWorld::randomCoordinates(int& x, int& y)
{
	bool isPossible;

	do
	{
		isPossible = true;

		x = rand() % 61;
		y = rand() % 61;

		if (!noOverlap(x, y))
		{
			isPossible = false;
			continue;
		}

		if (radius(x, y, 0, 60) < 4.0 || radius(x, y, 0, 60) == radius(0, 0, 3, 3))
		{
			isPossible = false;
			continue;
		}

		if (!noItem(x, y))
		{
			isPossible = false;
			continue;
		}
	} while (isPossible == false);
}

bool StudentWorld::dropGold()
{
	bool isPossible = true;

	if (!noItem(tunnelmanGetX(), tunnelmanGetY()))
	{
		isPossible = false;
	}

	if (isPossible)
	{
		Gold* tempGold = new Gold(this, Actor::temporary, tunnelmanGetX(), tunnelmanGetY());
		m_actors.push_back(tempGold);
		return true;
	}
	else
	{
		return false;
	}
}

void StudentWorld::reveal()
{
	vector<Actor*>::iterator it = m_actors.begin();
	while (it != m_actors.end())
	{
		if (!(*it)->isVisible())
		{
			if (withinRadiusOfTunnelman((*it)->getX(), (*it)->getY(), 12.0))
			{
				(*it)->isDiscovered();
			}
		}

		it++;
	}
}

void StudentWorld::createSquirt(const int x, const int y, GraphObject::Direction dir)
{
	Squirt* newSquirt = new Squirt(this, x, y, dir);

	if (noOverlap(newSquirt->getX(), newSquirt->getY(), newSquirt))
	{
		m_actors.push_back(newSquirt);
	}
	else
	{
		delete newSquirt;
	}

	return;
}

void StudentWorld::addProtester()
{
	int probabilityOfHardcore = min(90, static_cast<int>(getLevel()) * 10 + 30);
	int randNum = (rand() % 100) + 1;
	int ticksToWaitBetweenMoves = max(0, 3 - static_cast<int>(getLevel()) / 4);

	if (randNum >= probabilityOfHardcore)
	{
		Protester* newProtester = new Protester(this, ticksToWaitBetweenMoves);
		m_actors.push_back(newProtester);
	}
	else
	{
		HardcoreProtester* newHardcoreProtester = new HardcoreProtester(this, ticksToWaitBetweenMoves);
		m_actors.push_back(newHardcoreProtester);
	}

	addNumProtesters();
	return;
}

void StudentWorld::hitProtesters(const Actor* actor)
{
	vector<Actor*>::iterator it = m_actors.begin();
	while (it != m_actors.end())
	{
		if ((*it)->getType() == Actor::protester && (*it)->getState() == Actor::stable)
		{
			if (radius(actor->getX(), actor->getY(), (*it)->getX(), (*it)->getY()) < 4.0 || radius(actor->getX(), actor->getY(), (*it)->getX(), (*it)->getY()) == radius(0, 0, 3, 3))
			{
				Protester* protester = dynamic_cast<Protester*>(*it);
				protester->annoy(100);
			}
		}

		it++;
	}

	return;
}

bool StudentWorld::squirtProtesters(const Actor* actor)
{
	bool squirtedOnce = false;

	vector<Actor*>::iterator it = m_actors.begin();
	while (it != m_actors.end())
	{
		if ((*it)->getType() == Actor::protester && (*it)->getState() == Actor::stable)
		{
			if (radius(actor->getX(), actor->getY(), (*it)->getX(), (*it)->getY()) <= 3.0)
			{
				Protester* protester = dynamic_cast<Protester*>(*it);
				protester->annoy(2);
				squirtedOnce = true;
			}
		}

		it++;
	}

	return squirtedOnce;
}

int StudentWorld::shortestPath(Actor* actor, const int startX, const int startY, const int endX, const int endY)
{
	if (!noOverlap(startX, startY, actor))
	{
		return 3600;
	}

	bool visited[64][64];

	for (size_t x = 0; x < 64; x++)
	{
		for (size_t y = 0; y < 64; y++)
		{
			visited[x][y] = false;
		}
	}

	visited[startX][startY] = true;
	queue<Point> queuePoints;
	Point start(startX, startY);
	queuePoints.push(start);

	while (!queuePoints.empty())
	{
		Point current = queuePoints.front();

		if (current.m_x == endX && current.m_y == endY)
		{
			return current.m_distance;
		}

		queuePoints.pop();

		if ((noOverlap(current.m_x, current.m_y + 1, actor)) && (visited[current.m_x][current.m_y + 1] == false)) // UP
		{
			visited[current.m_x][current.m_y + 1] = true;
			Point next(current.m_x, current.m_y + 1, current.m_distance + 1);
			queuePoints.push(next);
		}

		if ((noOverlap(current.m_x, current.m_y - 1, actor)) && (visited[current.m_x][current.m_y - 1] == false)) // DOWN
		{
			visited[current.m_x][current.m_y - 1] = true;
			Point next(current.m_x, current.m_y - 1, current.m_distance + 1);
			queuePoints.push(next);
		}

		if ((noOverlap(current.m_x + 1, current.m_y, actor)) && (visited[current.m_x + 1][current.m_y] == false)) // RIGHT
		{
			visited[current.m_x + 1][current.m_y] = true;
			Point next(current.m_x + 1, current.m_y, current.m_distance + 1);
			queuePoints.push(next);
		}

		if ((noOverlap(current.m_x - 1, current.m_y, actor)) && (visited[current.m_x - 1][current.m_y] == false)) // LEFT
		{
			visited[current.m_x - 1][current.m_y] = true;
			Point next(current.m_x - 1, current.m_y, current.m_distance + 1);
			queuePoints.push(next);
		}
	}

	return 3600;
}

int StudentWorld::tunnelmanGetX() const
{
	return m_tunnelman->getX();
}

int StudentWorld::tunnelmanGetY() const
{
	return m_tunnelman->getY();
}