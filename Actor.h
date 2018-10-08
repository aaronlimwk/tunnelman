#ifndef ACTOR_H_
#define ACTOR_H_

#include "GameConstants.h"
#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class StudentWorld;

/////////////////
// Actor Class //
/////////////////

class Actor : public GraphObject
{
public:

	enum IDType { tunnelman, protester, boulder, squirt, item, earth };
	enum IDState { invalid, stable, waiting, falling, permanent, temporary, leaving };

	// Constructor
	Actor(StudentWorld* world, int imageID, int startX, int startY, IDType type, IDState state, Direction dir, double size, unsigned int depth, bool isVisible) : GraphObject(imageID, startX, startY, dir, size, depth)
	{
		m_type = type;
		m_state = state;
		m_alive = true;
		m_world = world;
		m_isVisible = isVisible;
		setVisible(isVisible);
	}

	// Accessor(s)
	IDType getType() const { return m_type; }
	IDState getState() const { return m_state; }
	bool isAlive() const { return m_alive; }
	bool isVisible() const { return m_isVisible; }
	StudentWorld* getWorld() const { return m_world; }

	// Mutator(s)
	void isDiscovered()
	{
		if (!isVisible())
		{
			m_isVisible = true;
			setVisible(true);
		}
	}

	// Virtual Function(s)
	virtual void doSomething() = 0;

	// Destructor
	virtual ~Actor() { setVisible(false); }

protected:
	void setState(IDState state) { m_state = state; }
	void setDead() { m_alive = false; }

private:
	IDType m_type;
	IDState m_state;
	bool m_alive;
	bool m_isVisible;
	StudentWorld* m_world;
};

/////////////////
// Earth Class //
/////////////////

class Earth : public Actor
{
public:
	// Constructor
	Earth(StudentWorld* world, int startX, int startY) : Actor(world, TID_EARTH, startX, startY, earth, invalid, right, 0.25, 3, true) {}

	// Mutator(s)
	virtual void doSomething() {}

	// Destructor(s)
	virtual ~Earth() {}
};

///////////////////
// Boulder Class //
///////////////////

class Boulder : public Actor
{
public:
	// Constructor
	Boulder(StudentWorld* world, int startX, int startY) : Actor(world, TID_BOULDER, startX, startY, boulder, stable, down, 1.0, 1, true) { m_ticks = 0; }

	// Mutator(s)
	virtual void doSomething();

	// Destructor
	virtual ~Boulder() {}

private:
	int getTicks() const { return m_ticks; }
	void addTicks() { m_ticks++; }

	int m_ticks;
};

//////////////////
// Squirt Class //
//////////////////

class Squirt : public Actor
{
public:
	// Constructor
	Squirt(StudentWorld* world, int startX, int startY, Direction dir) : Actor(world, TID_WATER_SPURT, startX, startY, squirt, invalid, dir, 1.0, 1, true) { m_travel = 4; }

	// Mutator(s)
	virtual void doSomething();

	// Destructor
	virtual ~Squirt() {}

private:
	int getTravel() const { return m_travel; }
	void reduceTravel() { m_travel--; }

	int m_travel;
};

////////////////
// Item Class //
////////////////

class Item : public Actor
{
public:
	// Constructor
	Item(StudentWorld* world, int lifetime, int imageID, int startX, int startY, IDType type, IDState state, bool isVisible) : Actor(world, imageID, startX, startY, type, state, right, 1.0, 2, isVisible)
	{
		m_lifetime = lifetime;

		if (state == temporary)
		{
			setVisible(true);
		}
	}

	// Mutator(s)
	virtual void doSomething();

	// Destructor
	virtual ~Item() {}

private:
	int getLifetime() const { return m_lifetime; }
	void reduceLifetime() { m_lifetime--; }

	// Virtual Function(s)
	virtual void doDifferentiatedStuff() = 0;

	int m_lifetime;
};

///////////////
// Oil Class //
///////////////

class Oil : public Item
{
public:
	// Constructor 
	Oil(StudentWorld* world, int startX, int startY) : Item(world, 0, TID_BARREL, startX, startY, item, permanent, false) {}

	// Destructor
	virtual ~Oil() {}

private:
	virtual void doDifferentiatedStuff();
};

////////////////
// Gold Class //
////////////////

class Gold : public Item
{
public:
	// Constructor
	Gold(StudentWorld* world, IDState state, int startX, int startY) : Item(world, 100, TID_GOLD, startX, startY, item, state, false) {}

	// Destructor
	virtual ~Gold() {}

private:
	virtual void doDifferentiatedStuff();
};

/////////////////
// Sonar Class //
/////////////////

class Sonar : public Item
{
public:
	// Constructor
	Sonar(StudentWorld* world, int lifetime) : Item(world, lifetime, TID_SONAR, 0, 60, item, temporary, true) {}

	// Destructor
	virtual ~Sonar() {}

private:
	virtual void doDifferentiatedStuff();
};

/////////////////
// Water Class //
/////////////////

class Water : public Item
{
public:
	// Constructor
	Water(StudentWorld* world, int lifetime, int startX, int startY) : Item(world, lifetime, TID_WATER_POOL, startX, startY, item, temporary, true) {}

	// Destructor
	virtual ~Water() {}

private:
	virtual void doDifferentiatedStuff();
};

/////////////////
// Human Class //
/////////////////

class Human : public Actor
{
public:
	// Constructor
	Human(StudentWorld* world, int health, int imageID, int startX, int startY, IDType type, IDState state, Direction dir) : Actor(world, imageID, startX, startY, type, state, dir, 1.0, 0, true)
	{
		m_health = health;
	}

	// Accessor(s)
	int getHealth() const { return m_health; }

	// Virtual Function(s)
	virtual void annoy(int damage) = 0;

	// Destructor
	virtual ~Human() {}

protected:
	void reduceHealth(int health) { m_health -= health; }

private:
	int m_health;
};

/////////////////////
// Tunnelman Class //
/////////////////////

class Tunnelman : public Human
{
public:
	// Constructor
	Tunnelman(StudentWorld* world) : Human(world, 10, TID_PLAYER, 30, 60, tunnelman, invalid, right)
	{
		m_squirt = 5;
		m_gold = 0;
		m_sonar = 1;
	}

	// Accessor(s)
	int getSquirt() const { return m_squirt; }
	int getGold() const { return m_gold; }
	int getSonar() const { return m_sonar; }

	// Mutator(s)
	void addSquirt() { m_squirt += 5; }
	void addGold() { m_gold++; }
	void addSonar() { m_sonar++; }

	virtual void doSomething();
	virtual void annoy(int damage);

	// Destructor
	virtual ~Tunnelman() {}

private:
	void reduceSquirt() { m_squirt--; }
	void reduceGold() { m_gold--; }
	void reduceSonar() { m_sonar--; }

	int m_squirt;
	int m_gold;
	int m_sonar;
};

/////////////////////
// Protester Class //
/////////////////////

class Protester : public Human
{
public:
	// Constructor
	Protester(StudentWorld* world, int ticksToWaitBetweenMoves, int health = 5, int imageID = TID_PROTESTER) : Human(world, health, imageID, 60, 60, protester, stable, left)
	{
		m_ticksToWaitBetweenMoves = ticksToWaitBetweenMoves;
		m_restingTicks = ticksToWaitBetweenMoves;
		m_shoutingTicks = 15;
		m_perpendicularTicks = 200;
		m_numSquaresToMove = (rand() % 53) + 8;
	}

	// Mutator(s)
	virtual void doSomething();
	virtual void annoy(int damage);
	virtual void pickGold();

	// Destructor
	virtual ~Protester() {}

protected:
	void setRestingTicks(int ticks) { m_restingTicks = ticks; }

private:
	int getTicksToWaitBetweenMoves() const { return m_ticksToWaitBetweenMoves; }
	int getRestingTicks() const { return m_restingTicks; }
	int getShoutingTicks() const { return m_shoutingTicks; }
	int getPerpendicularTicks() const { return m_perpendicularTicks; }
	int getNumSquaresToMove() const { return m_numSquaresToMove; }
	void reduceRestingTicks() { m_restingTicks--; }
	void reduceShoutingTicks() { m_shoutingTicks--; }
	void setShoutingTicks(int ticks) { m_shoutingTicks = ticks; }
	void reducePerpendicularTicks() { m_perpendicularTicks--; }
	void setPerpendicularTicks(int ticks) { m_perpendicularTicks = ticks; }
	void reduceNumSquaresToMove() { m_numSquaresToMove--; }
	void randomizeNumSquaresToMove() { m_numSquaresToMove = (rand() % 53) + 8; }
	void resetNumSquaresToMove() { m_numSquaresToMove = 0; }
	virtual bool doDifferentiatedStuff() { return false; }
	virtual void getPoints(int damage);

	int m_ticksToWaitBetweenMoves;
	int m_restingTicks;
	int m_shoutingTicks;
	int m_perpendicularTicks;
	int m_numSquaresToMove;
};

//////////////////////////////
// Hardcore Protester Class //
//////////////////////////////

class HardcoreProtester : public Protester
{
public:
	// Constructor
	HardcoreProtester(StudentWorld* world, int ticksToWaitBetweenMoves) : Protester(world, ticksToWaitBetweenMoves, 20, TID_HARD_CORE_PROTESTER) {}

	// Mutator(s)
	virtual void pickGold();

	// Destructor
	virtual ~HardcoreProtester() {}

private:
	virtual bool doDifferentiatedStuff();
	virtual void getPoints(int damage);
};

#endif // ACTOR_H_