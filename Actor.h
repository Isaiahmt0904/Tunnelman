#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <algorithm> 
#include "StudentWorld.h"
#include "GameWorld.h"
class StudentWorld;
class Tunnelman;
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

//For simplicity, I'm calling a leaving protester "dead". 


enum ActorState {
	idle, movingRight, movingLeft, movingUp, MovingDown, protesting, present, gone
};


class Actor : public GraphObject {
public:
	Actor(StudentWorld* world, int imageID, int startX, int startY, Direction dir = right, double size = 1.0, unsigned int depth = 0, ActorState state = idle)
		: GraphObject(imageID, startX, startY, dir, size, depth), m_world(world), m_state(state) {
		setVisible(true);
	}
	virtual ~Actor();
	ActorState getState() const { return m_state; }
	void makeVisible(bool isVisible) { GraphObject::setVisible(isVisible); }
	virtual void isAnnoyed(int hp) {}
	StudentWorld* getWorld() const { return m_world; }
	virtual bool isBoulder() const { return false; }
	void goTo(int x, int y);
	virtual void doSomething() = 0;

protected:
	ActorState m_state;
	
private:
	StudentWorld* m_world;
};

//Class for non-living objects: Sonar Kits, Gold, Boulders etc. 
class Object : public Actor {
public:
	Object(StudentWorld* world, int imageID, int x, int y, double size = 1.0, unsigned int depth = 2, ActorState state = present)
		: Actor(world, imageID, x, y, right, size, depth, state), m_pickedUp(false), m_ticks(0) {}
	virtual ~Object();
	bool PickedUp(Tunnelman& player);
	bool isPickedUp()
	{
		return m_pickedUp; 
	}
	void setPickedUp(bool pickedUp)
	{
		m_pickedUp = pickedUp; 

	}
	virtual void isAnnoyed(int hp) {}
	virtual void doSomething() override = 0; 
	virtual void setVisibility(bool visible) = 0; 
private:
	bool m_pickedUp;
	int m_ticks;
};


class LivingActor : public Actor {
public:
	LivingActor(StudentWorld* world, int x, int y, int imageID, ActorState state = movingLeft, Direction dir = right, int health = 5, bool alive = true)
		: Actor(world, imageID, x, y, dir, 1.0, 0, state), m_health(health), m_alive(alive) {}
	virtual ~LivingActor();
	int getHealth() const { return m_health; }
	void setHealth(int health) { m_health = health; }
	bool isCurrentlyAlive() const { return m_alive; }
	bool isAlive();
	void setAlive(bool alive)
	{
		m_alive = alive;

	}
	void decHP(int DecHp) { m_health -= DecHp; }
	void die() { m_alive = false;  }
	virtual void doSomething() = 0;
	virtual void isAnnoyed(int hp) override = 0;
	virtual void moveInDirection(Direction d);
	virtual void annoy() = 0; //Both protesters and Tunnelman can annoy (shout vs. squirt) 
protected:
	int m_health;
	bool m_alive;

};


class Tunnelman : public LivingActor
{
public:
	Tunnelman(StudentWorld* world, int startX, int startY, int numSquirts, int numSonar, int numGold, int imageID = TID_PLAYER)
		: LivingActor(world, startX, startY, imageID, idle, right, 10, true),
		m_numSquirts(numSquirts), m_numSonar(numSonar), m_numGold(numGold) {
		setVisible(true);
	}
	virtual ~Tunnelman() {

	}
	virtual void doSomething() override;

	//void dig(StudentWorld* world); 

	int getSquirts() const
	{
		return m_numSquirts;
	}
	int getSonar() const
	{
		return m_numSonar;
	}
	int getGold() const
	{
		return m_numGold;
	}
	//void shoot(); 

	virtual void isAnnoyed(int hp) override;

	virtual void annoy() override; 
	void shoot(); 

	void addSquirts(int squirts)
	{
		m_numSquirts += squirts; 
	}
	void addSonar(int sonar)
	{
		m_numSonar += sonar;
	}
	void addGold(int gold)
	{
		m_numGold += gold;
	}
	int getLives() const
	{
		return m_lives;
	}
private:
	int m_numSquirts;
	int m_numSonar;
	int m_numGold;
	int m_lives;

};


class RegularProtester : public LivingActor //Depth and size already initialized via actor
{
public:
	RegularProtester(StudentWorld* world, int startX, int startY, int imageID = TID_PROTESTER, int tickstoWait = 0)
		: LivingActor(world, startX, startY, imageID, movingLeft, left, 5, true),
		numSquaresToMoveInCurrentDirection(chooseRandomMoveDistance()),
		ticksToWaitBetweenMoves(tickstoWait), m_lastX(-1), m_lastY(-1), m_stuckTicks(0) 
	{
		setVisible(true);
		
	}
	virtual ~RegularProtester() {}
	virtual void doSomething() override; //Function used to shout, get bribed, do nothing etc.
	//Don't forget to implement shout cooldown logic
	virtual void moveInDirection(Direction d) override; 
	//Helper functions for different actions
	void getBribed();
	void stun();
	void annoy() override; 
	virtual void isAnnoyed(int hp) override; 
	void randomNumToMove(); 
	//Movement functions
	bool facingTunnelman();
	GraphObject::Direction randomDirection();
	bool straightPath(Direction d);
	bool atIintersection();
	void optimizedPathToTurn();
	GraphObject::Direction directionToPlayer(); 
	void changeDirection(); //Function implemented to stop protesters from getting stuck 
	bool leaving() const
	{
		return m_leave; 
	}
	bool isBeingAnnoyed()
	{
		return m_beingAnnoyed; 
	}
	void setBeingAnnoyed(bool annoyed)
	{
		m_beingAnnoyed = annoyed; 
	}

protected:
	bool m_beingAnnoyed; 

private:
	int numSquaresToMoveInCurrentDirection;
	int chooseRandomMoveDistance() //Returns random number
	{
		return 8 + rand() % 53; //Generates random number between 8 and 60 including the two

	}
	int ticksToWaitBetweenMoves;
	int m_numToMove;
	int m_tickNoYell; //Needed for cooldown
	int m_tickSinceLastTurn;
	int m_ticksRest; 
	bool m_leave; 
	int m_lastX, m_lastY;  
	int m_stuckTicks;    
	
	
};

class HardCoreProtester : public RegularProtester
{
public:
	HardCoreProtester(StudentWorld* world, int startX, int startY, int ticksToWait = 50)
		: RegularProtester(world, startX, startY, TID_HARD_CORE_PROTESTER, ticksToWait) {
		setHealth(20);
		setVisible(true);
	}
	virtual ~HardCoreProtester() {}
};




class Gold : public Object {

public:
	Gold(StudentWorld* world, int x, int y, int numGold)
	: Object(world, TID_GOLD, x, y, 1.0, 2), m_g(numGold), m_ticks(0) {}
	virtual ~Gold() { }
	Gold& operator=(const Gold& other);
	virtual void doSomething() override; 
	int getNumGold() const
	{
		return m_g; 
	}
	virtual void setVisibility(bool visible) override 
	{
		if (visible == false)
		{
			setVisible(false); 
		}
		else
		{
			setVisible(true); 
		}
	}
	int getTicks() const
	{
		return m_ticks; 
	}
	void setAlive(bool alive)
	{
		m_alive = alive;
	}
	bool isAlive() const 
	{
		return m_alive; 
	}
private:
	int m_g; //Amount of gold nuggets
	int m_ticks; 
	bool m_alive; 
};



class Water : public Object {

public:
	Water(StudentWorld* world, int x, int y)
		: Object(world, TID_WATER_POOL, x, y, 1.0, 2) {
	} //Might need to change creation tick 
	virtual void doSomething() override;
	int getNumWater() const
	{
		return m_num_water; 
	}
	int getTicksAlive() const
	{
		return m_ticks_alive; 
	}
	bool getAlive() const
	{
		return m_alive; 
	}
	void die() //Setter function to kill the water 
	{
			m_alive = false;
	}
	virtual void setVisibility(bool visible) override
	{
		if (visible == false)
		{
			setVisible(false);
		}
		else
		{
			setVisible(true);
		}
	}

	void setCreationTick(int tick); // Setter for creation tick
	int getCreationTick() const;
	

private:
	int m_num_water;
	int m_ticks_alive; 
	bool m_alive; 
	int m_creationTick;
};

class Sonar : public Object {

public:
	Sonar(StudentWorld* world, int x, int y)
		: Object(world, TID_SONAR, x, y, 1.0, 2) {
		setVisible(true);
	}
	virtual void doSomething() override; //Use the sonar
	virtual void setVisibility(bool visible) override
	{
		if (visible == false)
		{
			setVisible(false);
		}
		else
		{
			setVisible(true);
		}
	}
	void setAlive(bool alive)
	{
		m_alive = alive; 
	}
	bool isAlive() const 
	{
		return m_alive;  
	}

private:
	bool m_alive; 
};



class Squirt : public Actor
{
public:
	Squirt(StudentWorld* world, int startX, int startY, ActorState state, Direction dir)
		: Actor(world, TID_WATER_SPURT, startX, startY, dir, 1.0, 1, state) {
		setVisible(true);
		setAlive(true); 
	}
	virtual void doSomething() override;
	bool annoyProtester();
	virtual void setVisibility(bool visible) 
	{
		if (visible == false)
		{
			setVisible(false);
		}
		else
		{
			setVisible(true);
		}
	}
	void setAlive(bool alive)
	{
		m_alive = alive;
	}
	bool isAlive()
	{
		return m_alive;
	}
	bool annoyProtesters();

private:
	int m_travelDistance;
	bool m_alive;
};


class Boulder : public Object
{
public:
	Boulder(StudentWorld* world, int x, int y, int numBoulder)
		: Object(world, TID_BOULDER, x, y, 1.0, 1), m_numBoulders(numBoulder) {
		setVisible(true);
		m_alive = true; 
		m_isFalling = false; 
		m_ticks = 0; 
	}
	virtual void doSomething() override; 
	virtual bool isBoulder() const override
	{
		return true;
	}
	virtual void setVisibility(bool visible) override
	{
		if (visible == false)
		{
			setVisible(false);
		}
		else
		{
			setVisible(true);
		}
	}
	bool isFalling()
	{
		return m_isFalling; 
	}
	void setFalling(bool falling)
	{
		m_isFalling = falling; 
	}
	bool isAlive()
	{
		return m_alive; 
	}
	void die()
	{
		m_alive = false; 
		
	}

private:
	int m_numBoulders;
	int m_ticks;
	bool m_alive; 
	bool m_isFalling; 
};


class Earth : public Object {

public:
	Earth(StudentWorld* world, int x, int y)
		: Object(world, TID_EARTH, x, y, 0.25, 3) {
		setVisible(true);
	}
	virtual void doSomething() override;
	virtual void setVisibility(bool visible) override
	{
		if (visible == false)
		{
			setVisible(false);
		}
		else
		{
			setVisible(true);
		}
	}
};


class Barrel : public Object {

public:
	Barrel(StudentWorld* world, int numOil, int x, int y)
		: Object(world, TID_BARREL, x, y, 1.0, 2), m_o(numOil) {
		setVisible(true);
	}
	virtual ~Barrel() { }
	virtual void doSomething() override;

	virtual void setVisibility(bool visible) override
	{
		if (visible == false)
		{
			setVisible(false);
		}
		else
		{
			setVisible(true);
		}
	}

private:
	int m_o; //Amount of oil barrels 
};

#endif //Actor_H
