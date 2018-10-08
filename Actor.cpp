#include "Actor.h"
#include "GameConstants.h"
#include "GraphObject.h"
#include "StudentWorld.h"
#include <algorithm>
#include <vector>
using namespace std;

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

///////////////////////////////////
// Boulder Class Implementations //
///////////////////////////////////

void Boulder::doSomething()
{
	if (!isAlive())
	{
		return;
	}

	if (getState() == stable)
	{
		if (!(getWorld()->checkEarthBelow(getX(), (getY() - 1))))
		{
			setState(waiting);
			return;
		}
	}

	if (getState() == waiting)
	{
		if (getTicks() == 30)
		{
			setState(falling);
			getWorld()->playSound(SOUND_FALLING_ROCK);
			return;
		}
		else
		{
			addTicks();
		}
	}

	if (getState() == falling)
	{
		if (getWorld()->noOverlap(getX(), (getY() - 1), this))
		{
			moveTo(getX(), (getY() - 1));
		}
		else
		{
			setDead();
			return;
		}

		Tunnelman* player = getWorld()->searchForTunnelman(this, 3.0);
		if (player != nullptr)
		{
			player->annoy(100);
			return;
		}

		getWorld()->hitProtesters(this);
	}

	return;
}

//////////////////////////////////
// Squirt Class Implementations //
//////////////////////////////////

void Squirt::doSomething()
{
	if (!isAlive())
	{
		return;
	}

	if (getTravel() <= 0)
	{
		setDead();
		return;
	}

	if (getWorld()->squirtProtesters(this))
	{
		setDead();
		return;
	}

	switch (getDirection())
	{
	case up:
		if (getWorld()->noOverlap(getX(), (getY() + 1), this))
		{
			moveTo(getX(), (getY() + 1));
		}
		else
		{
			setDead();
			return;
		}
		break;
	case down:
		if (getWorld()->noOverlap(getX(), (getY() - 1), this))
		{
			moveTo(getX(), (getY() - 1));
		}
		else
		{
			setDead();
			return;
		}
		break;
	case right:
		if (getWorld()->noOverlap((getX() + 1), getY(), this))
		{
			moveTo((getX() + 1), getY());
		}
		else
		{
			setDead();
			return;
		}
		break;
	case left:
		if (getWorld()->noOverlap((getX() - 1), getY(), this))
		{
			moveTo((getX() - 1), getY());
		}
		else
		{
			setDead();
			return;
		}
		break;
	}

	reduceTravel();
	return;
}

////////////////////////////////
// Item Class Implementations //
////////////////////////////////

void Item::doSomething()
{
	if (!isAlive())
	{
		return;
	}

	doDifferentiatedStuff();

	if (getState() == temporary)
	{
		if (getLifetime() == 0)
		{
			setDead();
		}

		reduceLifetime();
	}

	return;
}

///////////////////////////////
// Oil Class Implementations //
///////////////////////////////

void Oil::doDifferentiatedStuff()
{
	if (!isVisible() && getWorld()->withinRadiusOfTunnelman(getX(), getY(), 4.0))
	{
		isDiscovered();
		return;
	}

	if (isVisible())
	{
		if (getWorld()->withinRadiusOfTunnelman(getX(), getY(), 3.0))
		{
			setDead();
			getWorld()->playSound(SOUND_FOUND_OIL);
			getWorld()->increaseScore(1000);
			getWorld()->reduceOil();
		}
	}

	return;
}

////////////////////////////////
// Gold Class Implementations //
////////////////////////////////

void Gold::doDifferentiatedStuff()
{
	if (!isVisible() && getWorld()->withinRadiusOfTunnelman(getX(), getY(), 4.0))
	{
		isDiscovered();
		return;
	}

	if (isVisible() && getState() == permanent)
	{
		Tunnelman* player = getWorld()->searchForTunnelman(this, 3.0);
		if (player != nullptr)
		{
			setDead();
			getWorld()->playSound(SOUND_GOT_GOODIE);
			getWorld()->increaseScore(10);
			player->addGold();
			return;
		}
	}

	if (isVisible() && getState() == temporary)
	{
		if (getWorld()->searchForProtester(this))
		{
			setDead();
		}
	}

	return;
}

/////////////////////////////////
// Sonar Class Implementations //
/////////////////////////////////

// Mutator(s)
void Sonar::doDifferentiatedStuff()
{
	Tunnelman* player = getWorld()->searchForTunnelman(this, 3.0);
	if (player != nullptr)
	{
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->increaseScore(75);
		player->addSonar();
	}

	return;
}

/////////////////////////////////
// Water Class Implementations //
/////////////////////////////////

// Mutator(s)
void Water::doDifferentiatedStuff()
{
	Tunnelman* player = getWorld()->searchForTunnelman(this, 3.0);
	if (player != nullptr)
	{
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->increaseScore(100);
		player->addSquirt();
	}

	return;
}

/////////////////////////////////////
// Tunnelman Class Implementations //
/////////////////////////////////////

void Tunnelman::doSomething()
{
	if (!isAlive())
	{
		return;
	}

	int ch;

	if (getWorld()->getKey(ch) == true)
	{
		switch (ch)
		{
		case KEY_PRESS_UP:
			if (getDirection() != up)
			{
				setDirection(up);
			}
			else
			{
				if (getWorld()->noOverlap(getX(), (getY() + 1), this))
				{
					moveTo(getX(), (getY() + 1));
					getWorld()->clearEarth(getX(), getY(), getType());
				}
			}

			moveTo(getX(), getY());
			break;

		case KEY_PRESS_DOWN:
			if (getDirection() != down)
			{
				setDirection(down);
			}
			else
			{
				if (getWorld()->noOverlap(getX(), (getY() - 1), this))
				{
					moveTo(getX(), (getY() - 1));
					getWorld()->clearEarth(getX(), getY(), getType());
				}
			}

			moveTo(getX(), getY());
			break;

		case KEY_PRESS_RIGHT:
			if (getDirection() != right)
			{
				setDirection(right);
			}
			else
			{
				if (getWorld()->noOverlap((getX() + 1), getY(), this))
				{
					moveTo((getX() + 1), getY());
					getWorld()->clearEarth(getX(), getY(), getType());
				}
			}

			moveTo(getX(), getY());
			break;

		case KEY_PRESS_LEFT:
			if (getDirection() != left)
			{
				setDirection(left);
			}
			else
			{
				if (getWorld()->noOverlap((getX() - 1), getY(), this))
				{
					moveTo((getX() - 1), getY());
					getWorld()->clearEarth(getX(), getY(), getType());
				}
			}

			moveTo(getX(), getY());
			break;

		case KEY_PRESS_ESCAPE:
			setDead();
			break;

		case KEY_PRESS_TAB:
			if (getGold() > 0)
			{
				if (getWorld()->dropGold())
				{
					reduceGold();
				}
			}

			break;

		case 'Z':
		case 'z':
			if (getSonar() > 0)
			{
				getWorld()->playSound(SOUND_SONAR);
				getWorld()->reveal();
				reduceSonar();
			}

			break;

		case KEY_PRESS_SPACE:
			if (getSquirt() > 0)
			{
				getWorld()->playSound(SOUND_PLAYER_SQUIRT);
				reduceSquirt();

				switch (getDirection())
				{
				case up:
					getWorld()->createSquirt(getX(), (getY() + 4), getDirection());
					break;
				case down:
					getWorld()->createSquirt(getX(), (getY() - 4), getDirection());
					break;
				case right:
					getWorld()->createSquirt((getX() + 4), getY(), getDirection());
					break;
				case left:
					getWorld()->createSquirt((getX() - 4), getY(), getDirection());
					break;
				}
			}

			break;
		}
	}

	return;
}

void Tunnelman::annoy(int damage)
{
	reduceHealth(damage);

	if (getHealth() <= 0)
	{
		getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
		setDead();
	}
}

/////////////////////////////////////
// Protester Class Implementations //
/////////////////////////////////////

void Protester::doSomething()
{
	// Check if protester is currently alive

	if (!isAlive())
	{
		return;
	}

	// If protester is "resting" during the current tick, do nothing

	if (getRestingTicks() > 0)
	{
		reduceRestingTicks();
		return;
	}
	else
	{
		setRestingTicks(getTicksToWaitBetweenMoves());
	}

	// If protester is leaving the oil field, it must find its way back to the exit point

	if (getState() == leaving)
	{
		if (getX() == 60 && getY() == 60)
		{
			setDead();
			getWorld()->reduceNumProtesters();
		}
		else
		{
			int up = getWorld()->shortestPath(this, getX(), getY() + 1, 60, 60);
			int down = getWorld()->shortestPath(this, getX(), getY() - 1, 60, 60);
			int right = getWorld()->shortestPath(this, getX() + 1, getY(), 60, 60);
			int left = getWorld()->shortestPath(this, getX() - 1, getY(), 60, 60);

			vector<int> shortest = { up, down, right, left };
			sort(shortest.begin(), shortest.end());

			if (shortest[0] == up)
			{
				setDirection(GraphObject::up);
				moveTo(getX(), getY() + 1);
			}
			else if (shortest[0] == down)
			{
				setDirection(GraphObject::down);
				moveTo(getX(), getY() - 1);
			}
			else if (shortest[0] == right)
			{
				setDirection(GraphObject::right);
				moveTo(getX() + 1, getY());
			}
			else
			{
				setDirection(GraphObject::left);
				moveTo(getX() - 1, getY());
			}
		}

		return;
	}

	// Checks to see if Tunnelman is within a distance of 4 units of the Tunnelman

	Tunnelman* player = getWorld()->searchForTunnelman(this, 4.0);
	if (player != nullptr)
	{
		bool facingTunnelman = false;

		// Checks to see if protester is currently facing the Tunnelman

		switch (getDirection())
		{
		case up:
			if (getY() < player->getY())
			{
				facingTunnelman = true;
			}
			break;
		case down:
			if (getY() > player->getY())
			{
				facingTunnelman = true;
			}
			break;
		case right:
			if (getX() < player->getX())
			{
				facingTunnelman = true;
			}
			break;
		case left:
			if (getX() > player->getX())
			{
				facingTunnelman = true;
			}
			break;
		}

		// If both cases mentioned above are satisfied, yell at the Tunnelman

		if (getShoutingTicks() <= 0 && facingTunnelman == true)
		{
			getWorld()->playSound(SOUND_PROTESTER_YELL);
			player->annoy(2);
			setShoutingTicks(15);
			reducePerpendicularTicks();
			return;
		}
		else if (getWorld()->withinLineOfSight(this) != none && facingTunnelman == true) // If the protester is directly facing the tunnelman, stay at its position (do nothing)
		{
			reduceShoutingTicks();
			reducePerpendicularTicks();
			moveTo(getX(), getY());
			return;
		}
		else if (facingTunnelman == true) // If the protester is not directly facing the tunnelman, move towards the protester's position
		{
			switch (getDirection())
			{
			case up:
				if (getY() < player->getY())
				{
					moveTo(getX(), (getY() + 1));
				}
				break;
			case down:
				if (getY() > player->getY())
				{
					moveTo(getX(), (getY() - 1));
				}
				break;
			case right:
				if (getX() < player->getX())
				{
					moveTo((getX() + 1), getY());
				}
				break;
			case left:
				if (getX() > player->getX())
				{
					moveTo((getX() - 1), getY());
				}
				break;
			}

			reduceShoutingTicks();
			reducePerpendicularTicks();
			return;
		}
		else // If the protester is in a straight horizontal or vertical line of sight to the Tunnelman, change its direction to face in the direction of the Tunnelman
		{
			Direction dir = getWorld()->withinLineOfSight(this);

			if (dir != none)
			{
				setDirection(dir);
				reduceShoutingTicks();
				reducePerpendicularTicks();
				moveTo(getX(), getY());
				return;
			}
		}
	}

	// Otherwise if the protester is more than 4 units away from the Tunnelman

	if (!(getWorld()->withinRadiusOfTunnelman(getX(), getY(), 4.0)))
	{
		if (doDifferentiatedStuff())
		{
			reduceShoutingTicks();
			reducePerpendicularTicks();

			if (getShoutingTicks() <= -100)
			{
				setShoutingTicks(0);
			}

			if (getPerpendicularTicks() <= -100)
			{
				setPerpendicularTicks(0);
			}

			return;
		}

		// Checks to see if protester is in a straight horizontal or vertical line of sight to the Tunnelman
		// AND could actually move the entire way to the Tunnelman with no Earth or Boulders blocking its path

		Direction dir = getWorld()->withinLineOfSight(this);

		if (dir != none)
		{
			// change its direction to face in the direction of the Tunnelman
			// AND then take one step toward him

			setDirection(dir);

			switch (dir)
			{
			case up:
				moveTo(getX(), (getY() + 1));
				break;
			case down:
				moveTo(getX(), (getY() - 1));
				break;
			case right:
				moveTo((getX() + 1), getY());
				break;
			case left:
				moveTo((getX() - 1), getY());
				break;
			}

			resetNumSquaresToMove();
			reduceShoutingTicks();
			reducePerpendicularTicks();
			return;
		}
	}

	reduceNumSquaresToMove();

	// If the protester has finished walking numSquaresToMove steps in its currently-selected direction

	if (getNumSquaresToMove() <= 0)
	{
		// Protester will pick a random new direction to move
		// IF the random direction is blocked either by Earth or a Boulder such that it can't take even a single step
		// THEN select a different direction and check it for blockage
		// The protester will then change its direction to this new chosen direction

		bool isPossible = true;

		do
		{
			isPossible = true;
			int randomDir = rand() % 4;

			switch (randomDir)
			{
			case 0: // UP
				if (getWorld()->noOverlap(getX(), (getY() + 1), this))
				{
					setDirection(up);
				}
				else
				{
					isPossible = false;
				}

				break;

			case 1: // DOWN
				if (getWorld()->noOverlap(getX(), (getY() - 1), this))
				{
					setDirection(down);
				}
				else
				{
					isPossible = false;
				}

				break;

			case 2: // RIGHT
				if (getWorld()->noOverlap((getX() + 1), getY(), this))
				{
					setDirection(right);
				}
				else
				{
					isPossible = false;
				}

				break;

			case 3: // LEFT
				if (getWorld()->noOverlap((getX() - 1), getY(), this))
				{
					setDirection(left);
				}
				else
				{
					isPossible = false;
				}

				break;
			}
		} while (isPossible == false);

		randomizeNumSquaresToMove();
	}
	else if (getPerpendicularTicks() <= 0) // Checks to see if the protester has not made a perpendicular turn in the last 200 non-resting ticks
	{
		bool changingDirection = true;

		// Checks to see if a protester is sitting at an intersection where it could turn
		// AND move at least one square in a perpendicular direction from its currently facing direction

		switch (getDirection()) // Pick a viable perpendicular direction. If both perpendicular directions are viable, then pick one of the two choices randomly
		{
		case up:
		case down:
			if ((getWorld()->noOverlap((getX() + 1), getY(), this)) && (getWorld()->noOverlap((getX() - 1), getY(), this)))
			{
				int randomDir = rand() % 2;

				if (randomDir == 0)
				{
					setDirection(right); // Sets its direction to the selected perpendicular direction
				}
				else
				{
					setDirection(left); // Sets its direction to the selected perpendicular direction
				}
			}
			else if (getWorld()->noOverlap((getX() + 1), getY(), this))
			{
				setDirection(right); // Sets its direction to the selected perpendicular direction
			}
			else if (getWorld()->noOverlap((getX() - 1), getY(), this))
			{
				setDirection(left); // Sets its direction to the selected perpendicular direction
			}
			else
			{
				changingDirection = false;
			}

			break;

		case right:
		case left:
			if ((getWorld()->noOverlap(getX(), (getY() + 1), this)) && (getWorld()->noOverlap(getX(), (getY() - 1), this)))
			{
				int randomDir = rand() % 2;

				if (randomDir == 0)
				{
					setDirection(up); // Sets its direction to the selected perpendicular direction
				}
				else
				{
					setDirection(down); // Sets its direction to the selected perpendicular direction
				}
			}
			else if (getWorld()->noOverlap(getX(), (getY() + 1), this))
			{
				setDirection(up); // Sets its direction to the selected perpendicular direction
			}
			else if (getWorld()->noOverlap(getX(), (getY() - 1), this))
			{
				setDirection(down); // Sets its direction to the selected perpendicular direction
			}
			else
			{
				changingDirection = false;
			}

			break;
		}

		if (changingDirection)
		{
			randomizeNumSquaresToMove(); // Pick a new value for numSquaresToMove
			setPerpendicularTicks(200);
		}
	}

	bool canMoveHere = false;

	// Protester will then attempt to take one step in its currently facing direction

	switch (getDirection())
	{
	case up:
		if (getWorld()->noOverlap(getX(), (getY() + 1), this))
		{
			moveTo(getX(), (getY() + 1));
			canMoveHere = true;
		}
		break;
	case down:
		if (getWorld()->noOverlap(getX(), (getY() - 1), this))
		{
			moveTo(getX(), (getY() - 1));
			canMoveHere = true;
		}
		break;
	case right:
		if (getWorld()->noOverlap((getX() + 1), getY(), this))
		{
			moveTo((getX() + 1), getY());
			canMoveHere = true;
		}
		break;
	case left:
		if (getWorld()->noOverlap((getX() - 1), getY(), this))
		{
			moveTo((getX() - 1), getY());
			canMoveHere = true;
		}
		break;
	}

	// If the protester is for some reason blocked from taking a step in its currently direction
	// It will set numSquaresToMove to zero, resulting in a new direction being chosing during the protester's next non-resting tick

	if (!canMoveHere)
	{
		resetNumSquaresToMove();
	}

	reduceShoutingTicks();
	reducePerpendicularTicks();

	if (getShoutingTicks() <= -10000)
	{
		setShoutingTicks(0);
	}

	if (getPerpendicularTicks() <= -10000)
	{
		setPerpendicularTicks(0);
	}

	return;
}

void Protester::annoy(int damage)
{
	reduceHealth(damage);

	if (getHealth() > 0)
	{
		getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
		int N = max(50, 100 - static_cast<int>(getWorld()->getLevel()) * 10);
		setRestingTicks(N);
	}
	else
	{
		setState(leaving);
		getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
		setRestingTicks(0);
		getPoints(damage);
	}
}

void Protester::getPoints(int damage)
{
	if (damage == 100)
	{
		getWorld()->increaseScore(500);
	}
	else
	{
		getWorld()->increaseScore(100);
	}
}

void Protester::pickGold()
{
	getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
	getWorld()->increaseScore(25);
	setState(leaving);
}

//////////////////////////////////////////////
// Hardcore Protester Class Implementations //
//////////////////////////////////////////////

bool HardcoreProtester::doDifferentiatedStuff()
{
	int M = 16 + static_cast<int>(getWorld()->getLevel()) * 2;

	int up = getWorld()->shortestPath(this, getX(), getY() + 1, getWorld()->tunnelmanGetX(), getWorld()->tunnelmanGetY());
	int down = getWorld()->shortestPath(this, getX(), getY() - 1, getWorld()->tunnelmanGetX(), getWorld()->tunnelmanGetY());
	int right = getWorld()->shortestPath(this, getX() + 1, getY(), getWorld()->tunnelmanGetX(), getWorld()->tunnelmanGetY());
	int left = getWorld()->shortestPath(this, getX() - 1, getY(), getWorld()->tunnelmanGetX(), getWorld()->tunnelmanGetY());

	vector<int> shortest = { up, down, right, left };
	sort(shortest.begin(), shortest.end());

	if (shortest[0] <= M)
	{
		if (shortest[0] == up)
		{
			setDirection(GraphObject::up);
			moveTo(getX(), getY() + 1);
		}
		else if (shortest[0] == down)
		{
			setDirection(GraphObject::down);
			moveTo(getX(), getY() - 1);
		}
		else if (shortest[0] == right)
		{
			setDirection(GraphObject::right);
			moveTo(getX() + 1, getY());
		}
		else
		{
			setDirection(GraphObject::left);
			moveTo(getX() - 1, getY());
		}

		return true;
	}

	return false;
}

void HardcoreProtester::getPoints(int damage)
{
	if (damage == 100)
	{
		getWorld()->increaseScore(500);
	}
	else
	{
		getWorld()->increaseScore(250);
	}
}

void HardcoreProtester::pickGold()
{
	getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
	getWorld()->increaseScore(50);
	int ticks_to_stare = max(50, 100 - static_cast<int>(getWorld()->getLevel()) * 10);
	setRestingTicks(ticks_to_stare);
}