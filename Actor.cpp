#include "Actor.h"
#include "StudentWorld.h"
//OG ONE 


// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

bool LivingActor::isAlive() {
    // Check if alive based on health
    if (m_health <= 0) {
        m_alive = false;
        // For example, you might want to notify the game world or play a sound
    }
    return m_alive;
}





void Barrel::doSomething()
{
    

}

void Actor::goTo(int x, int y)
{
    if (x < 0)
    {
        x = 0;
    }
    if (x > 60)
    {
        x = 60;
    }
    if (y < 0)
    {
        y = 0;
    }
    if (y > 60)
    {
        y = 60;
    }
    GraphObject::moveTo(x, y);
}



bool Object::PickedUp(Tunnelman& player)
{
    const int pickupRadius = 0; //Modify this as needed 

    double dx = this->getX() - player.getX();
    double dy = this->getY() - player.getY();

    //calculate distance using pythagoream theorem 

    double distance = sqrt(dx * dx + dy * dy);

    if (distance <= pickupRadius)
    {
        return true; //Item picked up
    }
    return false; //Item not picked up 
}

void LivingActor::moveInDirection(Direction d)
{
    switch (d)
    {
    case(left):
        if (getDirection() == left)
        {
            if (!(getWorld()->isBlocked(getX() - 1, getY(), left))) //If we can move there, move
            {
                moveTo(getX() - 1, getY());
            }
            else
            {
                moveTo(getX(), getY()); //Otherwise stay still 
            }
        }
        else setDirection(left);
        break;
    case(right):
        if (getDirection() == right)
        {
            if (!(getWorld()->isBlocked(getX() + 1, getY(), right))) //If we can move there, move
            {
                moveTo(getX() + 1, getY());
            }
            else
            {
                moveTo(getX(), getY()); //Otherwise stay still 
            }
        }
        else setDirection(right);
        break;
    case(down):
        if (getDirection() == down)
        {
            if (!(getWorld()->isBlocked(getX(), getY() - 1, down))) //If we can move there, move
            {
                moveTo(getX(), getY() - 1);
            }
            else
            {
                moveTo(getX(), getY()); //Otherwise stay still 
            }
        }
        else setDirection(down);
        break;
    case(up):
        if (getDirection() == up)
        {
            if (!(getWorld()->isBlocked(getX(), getY() + 1, up))) //If we can move there, move
            {
                moveTo(getX(), getY() + 1);
            }
            else
            {
                moveTo(getX(), getY()); //Otherwise stay still 
            }
        }
        else setDirection(up);
        break;

    default: case(none):
        return;
    }

}

void Boulder::doSomething()
{
    if (!isAlive())
    {
        return;
    }

    if (!isFalling())
    {
        if (getWorld()->aboveEarth(getX(), getY() - 1))
        {
            return;


        }

    }

    if (m_ticks == 30)
    {
        setFalling(true);
        getWorld()->playSound(SOUND_FALLING_ROCK);
    }
    m_ticks++;
    if (isFalling())
    {
        // Check if the boulder should stop falling
        if (getWorld()->aboveEarth(getX(), getY() - 1) ||
            getWorld()->isThereBoulder(getX(), getY() - 4, 0) ||
            getY() == 0)
        {
            die();
            setVisibility(false); //Hide the boulder 

        }
        else if (getWorld()->BoulderAbovePlayer(getWorld()->getPlayer()->getX(), getWorld()->getPlayer()->getY()))
        {
            getWorld()->getPlayer()->setAlive(false); 
            die();
            setVisibility(false);
            getWorld()->playSound(SOUND_PLAYER_GIVE_UP); 
        }
        else
        {
            moveTo(getX(), getY() - 1); // Move the boulder down
        }
        RegularProtester* p = dynamic_cast<RegularProtester*> (getWorld()->detectNearProtester(this, 3));
        HardCoreProtester* hp = dynamic_cast<HardCoreProtester*>(getWorld()->detectNearProtester(this, 3));
        if (p != nullptr)
        {
            p->isAnnoyed(100); 
        }
        if (hp != nullptr)
        {
            hp->isAnnoyed(100); 
        }

    }
}

GraphObject::Direction RegularProtester::directionToPlayer()
{
    int playerX = getWorld()->getPlayer()->getX();
    int playerY = getWorld()->getPlayer()->getY();
    if (getY() == playerY && getX() == playerX)
    {
        return getDirection();
    }
    if (getX() == playerX) {
        if (getY() < playerY)
            return up;
        if (getY() > playerY)
            return down;
    }
    if (getY() == playerY) {
        if (getX() > playerX)
            return left;
        if (getY() < playerY)
            return right;
    }

    return none;   // didnt see player in straight line
}

void RegularProtester::changeDirection()
{ 
    // Check if the protester is stuck by comparing current and last positions
    if (getX() == m_lastX && getY() == m_lastY)
    {
        m_stuckTicks++; // Only add stuck ticks if actually stuck
    }
    else {
        m_stuckTicks = 0; // Reset the stuck ticks
    }

    // Update the last known position
    m_lastX = getX();
    m_lastY = getY();

    // If not stuck for long enough, do not change direction
    if (m_stuckTicks < 1)
    {
        return;
    }

    // Check if the player is within proximity
    GraphObject::Direction playerDirection = getWorld()->sensePlayer(this, /*W=*/5); // Assuming W=5 as proximity range

    if (playerDirection != GraphObject::none)
    {
        // If the player is within proximity, use the optimized path to turn function
        optimizedPathToTurn();
    }
    else
    {
        // Otherwise, select a random direction
        bool foundPath = false;
        int randDir = rand() % 4; // Generates random int 0-3 corresponding to a direction
        GraphObject::Direction newDirection;
        int attempts = 0; // Prevent infinite loop

        while (!foundPath && attempts < 4) // Attempt to find a valid direction
        {
            switch (randDir)
            {
            case 0:
                newDirection = left;
                break;
            case 1:
                newDirection = right;
                break;
            case 2:
                newDirection = up;
                break;
            case 3:
                newDirection = down;
                break;
            }

            if (getWorld()->canMoveInDirection(getX(), getY(), newDirection))
            {
                foundPath = true;
            }

            attempts++;
            randDir = (randDir + 1) % 4; // Cycle through directions
        }

        if (foundPath)
        {
            setDirection(newDirection);
            moveInDirection(newDirection);
        }
    }

    // Reset ticks since the last direction change
    m_tickSinceLastTurn = 0;

}


void Sonar::doSomething()
{


}
void Earth::doSomething()
{
}

Actor::~Actor() {

}

Object::~Object() {

}

void Water::setCreationTick(int tick) 
{
    m_creationTick = tick;
}

int Water::getCreationTick() const 
{
    return m_creationTick;
}



void Water::doSomething()
{

    if (getAlive() == false)
    {
        return; //Water dead :( 
    }


    else
    {
        m_ticks_alive--;

        if (getWorld()->isPlayerinRadius(this, 3))
        {
            die();
            getWorld()->playSound(SOUND_GOT_GOODIE);
            getWorld()->getPlayer()->addSquirts(5);
            
        }
        if (m_ticks_alive == 0)
        {
            die(); 
        }


    }

}

LivingActor::~LivingActor() {

}

void Tunnelman::doSomething()
{
    if (getWorld()->dig(getX(), getY()))
    {
        getWorld()->playSound(SOUND_DIG);

    }

    if (!isAlive()) //Can't do anything if you're dead 
    {
        return;

    }
    int input;

    if (getWorld()->getKey(input) == true)
    {
        switch (input)
        {
        case KEY_PRESS_ESCAPE:
            setAlive(false); // Set the Tunnelman to "not alive"
            getWorld()->playSound(SOUND_PLAYER_GIVE_UP); // Play sound for giving up
            break;
        case KEY_PRESS_LEFT: moveInDirection(left); break;

        case KEY_PRESS_RIGHT: moveInDirection(right); break;

        case KEY_PRESS_UP: moveInDirection(up); break;

        case KEY_PRESS_DOWN: moveInDirection(down); break;

        case 'z': 
        case'Z': 
            if (m_numSonar > 0)
            {
                m_numSonar--;
                getWorld()->detectNearActor(getX(), getY(), 12); 
                getWorld()->playSound(SOUND_SONAR); 
            }
            break; 
        case KEY_PRESS_TAB: 
            if (m_numGold > 0)
            {
                Gold * gold = new Gold(getWorld(), getX(), getY(), 1);
                gold->setPickedUp(true);
                gold->setAlive(true); 
                getWorld()->addActor(gold); 
                m_numGold--; //Decrease by 1 
            }
            break; 
        case KEY_PRESS_SPACE: 
            if (m_numSquirts > 0)
            {
                m_numSquirts--; 
                shoot(); 
            }
            break; 
        }
    }

}


void Tunnelman::shoot()
{
    if (m_numSquirts > 0)
    {
        switch (getDirection()) //Switch statement to control the shooting in directions. 
        {

        case left: 
            if (!getWorld()->isBlocked(getX() - 4, getY(), getDirection()) &&
                !getWorld()->isThereEarth1(getX() - 4, getY(), 0))
            {
                getWorld()->addActor(new Squirt(getWorld(), getX() - 4, getY(), movingLeft, left)); 
            }
            break; 

        case right: 
            if (!getWorld()->isBlocked(getX() + 4, getY(), getDirection()) &&
                !getWorld()->isThereEarth1(getX() + 4, getY(), 0))
            {
                getWorld()->addActor(new Squirt(getWorld(), getX() + 4, getY(), movingRight, right));
            }
            break; 
        case down: 
            if (!getWorld()->isBlocked(getX(), getY() - 4, getDirection()) &&
                !getWorld()->isThereEarth1(getX(), getY() - 4, 0))
            {
                getWorld()->addActor(new Squirt(getWorld(), getX(), getY() - 4, MovingDown, down));
            }
            break; 
        case up: 
            if (!getWorld()->isBlocked(getX(), getY() - 4, getDirection()) &&
                !getWorld()->isThereEarth1(getX(), getY() - 4, 0))
            {
                getWorld()->addActor(new Squirt(getWorld(), getX(), getY() + 4, movingUp, left));
            }
            break; 
        case none: return; 
           
        }
        getWorld()->playSound(SOUND_PLAYER_SQUIRT); //Play the shooting sound 
    }

}

void Tunnelman::annoy()
{


}

void Tunnelman::isAnnoyed(int hp)
{
    decHP(hp);
    if (getHealth() <= 0)
    {
        setAlive(false);

        getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
        //Don't forget to add sound 
    }

}

void Squirt::doSomething()
{
    if (!isAlive())
    {
        setVisibility(false);
        return;
    }
    if (annoyProtesters()) {
        setAlive(false); 
        return;
    }

    // Stop if the squirt has traveled its maximum distance
    if (m_travelDistance >= 4)
    {
        setAlive(false);
        return;
    }

    // Determine the next position and check for earth
    int nextX = getX();
    int nextY = getY();

    switch (getState())
    {
    case movingLeft:
        setDirection(left);
        nextX--;
        
        break;
    case movingRight:
        setDirection(right);
        nextX++;
       
        break;
    case movingUp:
        setDirection(up);
        nextY++;
        break;
    case MovingDown:
        setDirection(down);
        nextY--;
        break;
    case none:
        return;

    }
   
    // Check if the next position is blocked by earth
    if (!getWorld()->canMoveInDirection(nextX, nextY, getDirection()))
    {
        setAlive(false);
        return;
    }
    else
    {
        moveTo(nextX, nextY);
        m_travelDistance++;
    }
}

bool Squirt::annoyProtesters()
{ 
    RegularProtester* p = dynamic_cast<RegularProtester*> (getWorld()->detectNearProtester(this, 3));
    HardCoreProtester* hp = dynamic_cast<HardCoreProtester*>(getWorld()->detectNearProtester(this, 3));
    if (p != nullptr) {
        p->isAnnoyed(1);
        return true;
        if (p->isAlive())
        {
            p->setBeingAnnoyed(true); 
            getWorld()->playSound(SOUND_PROTESTER_ANNOYED); 
        }
    }
    if (hp != nullptr) {
        hp->isAnnoyed(1);
        return true;
        if (hp->isAlive())
        {
            hp->setBeingAnnoyed(true); 
            getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
        }
    }
    return false;
}

GraphObject::Direction RegularProtester::randomDirection()
{
    int r;
    r = rand() % 4;
    switch (r) {
    case 0: return left;
    case 1: return  right;
    case 2: return up;
    case 3: return down;
    }
    return none;
}
void RegularProtester::getBribed()
{
       getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
        if (getID() == TID_PROTESTER) {
            getWorld()->increaseScore(25);
            m_leave = true;
        }
        else {
            getWorld()->increaseScore(50);
            m_ticksRest = std::max(50, 100 - int(getWorld()->getLevel()) * 10);
        }
    
}
void RegularProtester::stun()
{
    m_ticksRest = std::max(50, 100 - (int)getWorld()->getLevel() * 10);
}
void RegularProtester::annoy()
{


}

void RegularProtester::isAnnoyed(int hp)
{
    if (m_leave) return; 
    decHP(hp); 
    getWorld()->playSound(SOUND_PROTESTER_ANNOYED); 
    stun(); 


    if (hp == 100) getWorld()->increaseScore(500); // hit by boulder

    else if (getID() == TID_PROTESTER) getWorld()->increaseScore(100);

    else getWorld()->increaseScore(250);

}

bool RegularProtester::facingTunnelman()
{
    switch (getDirection()) 
    {
    case left:
        return getWorld()->getPlayer()->getX() <= getX();

    case right:
        return getWorld()->getPlayer()->getX() >= getX();

    case up:
        return getWorld()->getPlayer()->getY() >= getY();

    case down:
        return getWorld()->getPlayer()->getY() <= getY();

    case none:
        return false;
    }

    return false;
}


bool RegularProtester::straightPath(Direction d)
{
    int playerX = getWorld()->getPlayer()->getX();
    int playerY = getWorld()->getPlayer()->getX();
    switch (d) {
    case left:
        for (int i = getX(); i >= playerX; i--) {
            if (getWorld()->isThereEarth3(i, getY()) || getWorld()->isThereBoulder(i, getY(), 0))
                return false;
        }
        return true;
        break;
    case right:
        for (int i = getX(); i <= playerX; i++) {
            if (getWorld()->isThereEarth3(i, getY()) || getWorld()->isThereBoulder(i, getY(), 0))
                return false;
        }
        return true;
        break;
    case up:
        for (int j = getY(); j <= playerY; j++) {
            if (getWorld()->isThereEarth3(getX(), j) || getWorld()->isThereBoulder(getX(), j, 0))
                return false;
        }
        return true;
        break;
    case down:
        for (int j = getY(); j >= playerY; j--) {
            if (getWorld()->isThereEarth3(getX(), j) || getWorld()->isThereBoulder(getX(), j, 0))
                return false;
        }
        return true;
        break;
    case none:
        return false;
    }
}

bool RegularProtester::atIintersection()
{
    if (getDirection() == up || getDirection() == down) 
    {
        return (getWorld()->canMoveInDirection(getX(), getY(), left) || getWorld()->canMoveInDirection(getX(), getY(), right));
    }
    else
        return (getWorld()->canMoveInDirection(getX(), getY(), up) || getWorld()->canMoveInDirection(getX(), getY(), down));
}

void RegularProtester::optimizedPathToTurn()
{
    if (getDirection() == up || getDirection() == down) {
        if (!getWorld()->canMoveInDirection(getX(), getY(), left)) setDirection(right);

        else if (!getWorld()->canMoveInDirection(getX(), getY(), right)) setDirection(left);

        else {

            switch (rand() % 2) {
            case 0: setDirection(left);
            case 1: setDirection(right);

            }
        }
    }
    else {
        if (!getWorld()->canMoveInDirection(getX(), getY(), up)) setDirection(down);

        else if (!getWorld()->canMoveInDirection(getX(), getY(), down)) setDirection(up);

        else {

            switch (rand() % 2) {

            case 0: setDirection(up);
            case 1: setDirection(down);

            }
        }
    }

    if (!getWorld()->canMoveInDirection(getX(), getY(), getDirection())) {
        optimizedPathToTurn(); // Reattempt direction change
    }

}

void RegularProtester::moveInDirection(Direction d)
{
    switch (d) {
    case left:
        if (getDirection() == left && getWorld()->canMoveInDirection(getX() - 1, getY(), left)) {
            moveTo(getX() - 1, getY());
        }
        else {
            moveTo(getX(), getY());
        }
        setDirection(left);
        break;
    case right:
        if (getDirection() == right && getWorld()->canMoveInDirection(getX() + 1, getY(), right)) {
            moveTo(getX() + 1, getY());
        }
        else {
            moveTo(getX(), getY());
        }
        setDirection(right);
        break;
    case up:
        if (getDirection() == up && getWorld()->canMoveInDirection(getX(), getY() + 1, up)) {
            moveTo(getX(), getY() + 1);
        }
        else {
            moveTo(getX(), getY());
        }
        setDirection(up);
        break;
    case down:
        if (getDirection() == down && getWorld()->canMoveInDirection(getX(), getY() - 1, down)) {
            moveTo(getX(), getY() - 1);
        }
        else {
            moveTo(getX(), getY());
        }
        setDirection(down);
        break;
    }


}

void RegularProtester::randomNumToMove()
{
    m_numToMove = rand() % 53 + 8;
}

void RegularProtester::doSomething()
{
    //If dead, can't do anything 
    if (!isAlive())
    {
        getWorld()->playSound(SOUND_PROTESTER_GIVE_UP); 
        getWorld()->leave(this); 
        getWorld()->decProtester();
        return; 
    }
    /*
    if (isAnnoyed(2))
    {

    }
    */ 
    //Implement cooldown 
    if (m_ticksRest > 0) {
        m_ticksRest--;
        return;
    }
    else {
        m_ticksRest = std::max(0, 3 - (int)getWorld()->getLevel() / 4);
        m_tickSinceLastTurn++;
        m_tickNoYell++;
    }

    //Handly leaving logic 
    if (m_leave)
    {
        if (getX() == 60 && getY() == 60)
        {
            die(); 
            setAlive(false); 
            
        }
        //If not already dead, leave 
        getWorld()->leave(this); 
        return; 
    }

    if (getWorld()->isPlayerinRadius(this, 4) && facingTunnelman()) {
        if (m_tickNoYell > 15) {
            getWorld()->getPlayer()->isAnnoyed(2);
            getWorld()->playSound(SOUND_PROTESTER_YELL);
            m_tickNoYell = 0;
            return;
        }
        return;
    }

    //Implement hardcore protester logic 
    if (getID() == TID_HARD_CORE_PROTESTER)
    {
        int M = 16 + int(getWorld()->getLevel()); //Sense signal from 16 tiles away 
        Direction s = getWorld()->sensePlayer(this, M);
        if (s != none) {
            moveInDirection(s);
            return;
        }

    }
    
    //If player is in the line of sight, move towards them. 
    Direction d = directionToPlayer();
    if (d != none && straightPath(d) && (!getWorld()->isPlayerinRadius(this, 4))) {
        setDirection(d);
        moveInDirection(d);
        m_numToMove = 0;
        return;
    }

    m_numToMove--;
    if (m_numToMove <= 0) {
        Direction k = none;
        k = randomDirection();
        while (true) {
            k = randomDirection();
            if (getWorld()->canMoveInDirection(getX(), getY(), k)) break;
        }
        setDirection(k);
        randomNumToMove();
    }
    //Handle intersections 
    else if (atIintersection() && m_tickSinceLastTurn > 200) {
        optimizedPathToTurn();
        m_tickSinceLastTurn = 0;
        randomNumToMove();
    }
    moveInDirection(getDirection());

    if (!getWorld()->canMoveInDirection(getX(), getY(), getDirection())) 
    {
        m_numToMove = 0;
    }
    else {
        //std::cout << "This function is being ran" << std::endl;
        changeDirection(); //Only should run if protesters been stuck for a while
    }
   
    

}





void Gold::doSomething()
{
    std::cout << "Gold::doSomething() called" << std::endl;
    if (isPickedUp())
    {
        if (m_ticks > 100)
        {
            setVisibility(false);
            setAlive(false);
        }

        LivingActor* detectedActor = getWorld()->detectNearProtester(this, 3);
        if (detectedActor) {
            //std::cout << "Detected Actor called" << std::endl;
            RegularProtester* regularProtester = dynamic_cast<RegularProtester*>(detectedActor);
            HardCoreProtester* hardCoreProtester = dynamic_cast<HardCoreProtester*>(detectedActor);
            if (regularProtester) {
                // Handle RegularProtester
                regularProtester->getBribed();
                setAlive(false); 
                setVisibility(false); 
                return; 
            }
            else if (hardCoreProtester) {
                    // Handle HardCoreProtester
                    hardCoreProtester->getBribed();
                    setAlive(false);
                    setVisibility(false);
                    return;
                }
            
        }

    }
    m_ticks++;
    

   

}