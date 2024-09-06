#include "StudentWorld.h"
#include <string>
#include <cmath>  // For sqrt
#include <algorithm> 
#include<vector>
#include<sstream>
#include <iomanip>
#include <queue>
using namespace std;

GameWorld* createStudentWorld(std::string assetDir) {
    return new StudentWorld(assetDir);
}

std::string StudentWorld::displayText(int score, int level, int lives, int health, int squirts, int gold, int sonar, int barrels)
{
    stringstream str;

    str.fill('0');
    str << "Scr: ";
    str << setw(6) << score;

    str.fill(' ');
    str << " Lvl: ";
    str << setw(2) << level;

    str << " Lives: ";
    str << setw(1) << lives;

    str << "  Hlth: ";
    str << setw(3) << health * 10;
    str << '%';

    str << "  Wtr: ";
    str << setw(2) << squirts;

    str << "  Gld: ";
    str << setw(2) << gold;

    str << "  Sonar: ";
    str << setw(2) << sonar;

    str << "  Oil Left: ";
    str << setw(2) << barrels;

    return str.str();
}

void StudentWorld::updateDisplay()
{
    int score = getScore(); 
    int level = getLevel(); 
    int lives = getLives(); 
    int health = m_player->getHealth(); 
    int gold = m_player->getGold(); 
    int squirts = m_player->getSquirts(); 
    int sonar = m_player->getSonar(); 

    string s = displayText(score, level,lives, health, squirts, gold, sonar, m_barrelsLeft); 
    setGameStatText(s); 

    
}

void StudentWorld::markAsDug(int x, int y) {
    if (x>0 && x < 63 && y > 0 && y < 63) {
        m_earth[x][y] = nullptr;  // Mark as empty
    }
}

std::vector<Boulder*> StudentWorld::getBoulders() const
{
    vector<Boulder*> boulders; // Vector to store boulders
    vector<Actor*>::const_iterator it; // Iterator for the objects vector

    for (it = m_actors.begin(); it != m_actors.end(); ++it) // Iterate through m_objects
    {
        Boulder* boulder = dynamic_cast<Boulder*>(*it); // Attempt to cast each object to a Boulder*
        if (boulder) // If the cast is successful
        {
            boulders.push_back(boulder); // Add to boulders vector
        }
    }
    return boulders; // Return the vector of boulders
}

std::vector<Gold*> StudentWorld::getGolds()
{
    vector<Gold*> golds; // Vector to store gold
    vector<Actor*>::const_iterator it; // Iterator for the objects vector

    for (it = m_actors.begin(); it != m_actors.end(); ++it) // Iterate through m_objects
    {
        Gold* gold = dynamic_cast<Gold*>(*it); // Try to cast each object to Gold*
        if (gold) // If the cast is successful
        {
            golds.push_back(gold); // Add to golds vector
        }
    }
    return golds; // Return the vector of golds
}

std::vector<Barrel*> StudentWorld::getBarrels()
{
    vector<Actor*>::const_iterator it; // Iterator for objects vector
    vector<Barrel*> barrels; // Vector to store barrels

    for (it = m_actors.begin(); it != m_actors.end(); ++it)
    {
        Barrel* barrel = dynamic_cast<Barrel*>(*it);
        if (barrel) // If successful cast
        {
            barrels.push_back(barrel);
        }
    }
    return barrels; // Return the vector of barrels found
}

void StudentWorld::addActor(Actor* actor)
{
    m_actors.push_back(actor);

}

void StudentWorld::addProtester()
{
    int RticksToWaitBetweenMoves = std::max(50, 100 - ((int)getLevel()) * 10); //Not these two 
    int HticksToWaitBetweenMoves = std::max(0, 3 - ((int)getLevel()) / 4); 
    int T = max(25, 200 - (int)getLevel());
    int P = fmin(15, 2 + getLevel() * 1.5);	
    int probabilityOfHardcore = min(90, (int)getLevel() * 10 + 30); //probability of adding a hardcore protestser 
    
    
        //std::cout << "ticksToWaitBetweenMoves: " << HticksToWaitBetweenMoves << std::endl;
    if (m_firstTick || (m_tickSinceLast > T && m_protesters < P)) {
        if (rand() % 100 + 1 < probabilityOfHardcore)
        {
            HardCoreProtester* hp = new HardCoreProtester(this, 60, 60, HticksToWaitBetweenMoves);
            m_alive_actors.push_back(hp);
        }
        else
        {
            RegularProtester* rp = new RegularProtester(this, 60, 60, TID_PROTESTER, RticksToWaitBetweenMoves);
            m_alive_actors.push_back(rp);
        }
        m_tickSinceLast = 0;
        m_protesters++; //Add to the number of protesters 
        m_firstTick = false;

    }
    
    m_tickSinceLast++; //Increment this count 

}

bool StudentWorld::isThereEarth1(int x, int y, int radius)
{
    // Define boundaries for object within earth 
    int left = std::max(x - radius, 0);
    int right = std::min(x + radius, 63);
    int bottom = std::max(y - radius, 0);
    int top = std::min(y + radius, 63);

    // Check if there is earth within the defined boundaries
    for (int i = left; i <= right; i++)
    {
        for (int j = bottom; j <= top; j++)
        {
            if (m_earth[i][j] != nullptr)
            {
                return true;
            }
        }
    }
    return false;
}

bool StudentWorld::isThereEarth2(int x, int y, int radius)
{
    // Define boundaries for object within earth 
    int left = std::max(x - radius, 0);
    int right = std::max(x + radius, 63);
    int bottom = std::max(y - radius, 0);
    int top = std::max(y + radius, 63);

    // Check if there is earth within the defined boundaries
    for (int i = left; i <= right; i++)
    {
        for (int j = bottom; j <= top; j++)
        {
            if (m_earth[i][j] != nullptr)
            {
                return true;
            }
        }
    }
    return false;
}

bool StudentWorld::isThereEarth3(int x, int y) //Check for earth WITHOUT a radius 
{
    for (int i = x; i < x + 4; i++) {

        for (int j = y; j < y + 4; j++) {

            if (m_earth[i][j] != nullptr)
                return true;
        }
    }
    return false;
}


bool StudentWorld::aboveEarth(int x, int y)
{
    for (int i = x; i < x + 4; i++) //width of image is 3 array elements 
    {
        if (m_earth[i][y] != nullptr)
        {
            return true;
        }


    }
    return false; 
}

bool StudentWorld::BoulderAbovePlayer(int x, int y)
{
    for (vector<Object*>::iterator it = m_objects.begin(); it != m_objects.end(); it++)
    {
        Boulder* boulder = dynamic_cast<Boulder*>(*it); 
        if (boulder)
        {
            if (boulder->getX() >= x && boulder->getX() <= x + 3
                && boulder->getY() > m_player->getY()
                && boulder->getY() <= m_player->getY() + 3)
            {
                return true; 
           }
        }
    }
    return false;
}

void StudentWorld::detectNearActor(int x, int y, int radius) //To check if protesters are within radius or objects to close together or Sonar implementation 
{
    int w, z;
    vector<Actor*> ::iterator it;
    for (it = m_actors.begin(); it != m_actors.end(); it++) //Iterate through all actors in world 
    {
        if ((*it)->getID() == TID_BARREL || (*it)->getID() == TID_GOLD) //Only add barrel or gold if theres the proper space 
        {
            w = (*it)->getX();
            z = (*it)->getY();
            int ObjectRadiusSQR = (((x - w) * (x - w)) + ((y - z) * (y - z))); //Work with squared radius 

            if (ObjectRadiusSQR <= radius * radius)
            {
                (*it)->setVisible(true);
            }
        }


    }
}

bool StudentWorld::inRadius(int x1, int y1, int x2, int y2, int radius) const {
    int delta_x = abs(x1 - x2);
    int delta_y = abs(y1 - y2);
    return delta_x * delta_x + delta_y * delta_y <= radius * radius;
}


bool StudentWorld::ActorWithinRadius(int x, int y, int radius)
{
    vector<Actor*> ::iterator it;

    it = m_actors.begin();
    while (it != m_actors.end())
    {
        if (inRadius(x, y, (*it)->getX(), (*it)->getY(), radius))
        {
            return true;
        }
        it++;
    }
    return false;
}



void StudentWorld::cleanUp()
{
    delete m_player;
    m_player = nullptr; //Avoid memory leak (dangling pointer) 

    for (auto actor : m_actors) {
        delete actor;
    }
    m_actors.clear();

    for (auto alive_actor : m_alive_actors) {
        delete alive_actor;
    }
    m_alive_actors.clear();


    for (auto objects : m_objects)
    {
        delete objects;
    }
    m_objects.clear();


    for (int x = 0; x < 64; x++) {
        for (int y = 0; y < 60; y++) {
            delete m_earth[x][y];
            m_earth[x][y] = nullptr;
        }
    }
}

bool StudentWorld::dig(int x, int y)
{
    bool dug = false;
    for (int i = x; i < x + 4; i++) //4 matches the size of the player object (4 x 4 grid element) 
    {
        for (int j = y; j < y + 4; j++)
        {
            if (m_earth[i][j] != nullptr)
            {
                delete m_earth[i][j];
                m_earth[i][j] = nullptr;
                dug = true;

            }

        }
    }
    return dug;
}
void StudentWorld::addGoodies()
{
    //static int tickCounter = 0;
    //static int nextWaterSpawnTick = rand() % 100 + 50;
    //int waterTicksAlive = std::max(100, static_cast<int>(300 - (10 * getLevel())));
    int x, y;
    int G = (int)getLevel() + 300;
    if (int(rand() % G) + 1 == 1) {
        if (int(rand() % 5) + 1 == 1) {
            addActor(new Sonar(this, 0, 60));
        }
        else {
            do {
                x = rand() % 60 + 1;
                y = rand() % 60 + 1;
            } while (isThereEarth2(x, y, 0));
            addActor(new Water(this, x, y));
        }
    }
}

void StudentWorld::leave(RegularProtester* pr)
{
    m_pLeaving = true; 
    //Intialize maze 
    for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 64; j++)
        {
            m_maze[i][j] = 0; 
        }
    }

    int w = pr->getX(); 
    int z = pr->getY(); 
    queue<Grid> q;
    q.push(Grid(60, 60));
    m_maze[60][60] = 1;
    while (!q.empty()) 
    {
        Grid c = q.front(); //Queue to keep track of movements 
        q.pop();
        int x = c.x;
        int y = c.y;

        //left
        if (canMoveInDirection(x, y, GraphObject::left) && m_maze[x - 1][y] == 0) {
            q.push(Grid(x - 1, y));
            m_maze[x - 1][y] = m_maze[x][y] + 1;
        }
        //right
        if (canMoveInDirection(x, y, GraphObject::right) && m_maze[x + 1][y] == 0) {
            q.push(Grid(x + 1, y));
            m_maze[x + 1][y] = m_maze[x][y] + 1;
        }
        //up 
        if (canMoveInDirection(x, y, GraphObject::up) && m_maze[x][y + 1] == 0) {
            q.push(Grid(x, y + 1));
            m_maze[x][y + 1] = m_maze[x][y] + 1;
        }
        // down
        if (canMoveInDirection(x, y, GraphObject::down) && m_maze[x][y - 1] == 0) {
            q.push(Grid(x, y - 1));
            m_maze[x][y - 1] = m_maze[x][y] + 1;
        }
    }

    //Recursive calls for repeat 
    if (canMoveInDirection(w, z, GraphObject::left) && m_maze[w - 1][z] < m_maze[w][z])
        pr->moveInDirection(GraphObject::left);
    if (canMoveInDirection(w, z, GraphObject::right) && m_maze[w + 1][z] < m_maze[w][z])
        pr->moveInDirection(GraphObject::right);
    if (canMoveInDirection(w, z, GraphObject::up) && m_maze[w][z + 1] < m_maze[w][z])
        pr->moveInDirection(GraphObject::up);
    if (canMoveInDirection(w, z, GraphObject::down) && m_maze[w][z - 1] < m_maze[w][z])
        pr->moveInDirection(GraphObject::down);
    return;
}

bool StudentWorld::canMoveInDirection(int x, int y, GraphObject::Direction direction)
{
   
    if (!m_pLeaving)
    {
        switch (direction) {
        case GraphObject::left:
            return (x != 0 && !isThereEarth3(x - 1, y) && !isThereBoulder(x, y, 0));
        case GraphObject::right:
            return (x != 60 && !isThereEarth3(x + 1, y) && !isThereBoulder(x + 1, y, 0));
        case GraphObject::up:
            return (y != 60 && !isThereEarth3(x, y + 1) && !isThereBoulder(x, y + 1, 0));
        case GraphObject::down:
            return (y != 0 && !isThereEarth3(x, y - 1) && !isThereBoulder(x, y - 1, 0)); //Might need to change this radius 
        case GraphObject::none:
            return false;
        }
        return false;
    }

    else {
        switch (direction) {
        case GraphObject::left:
            return (x != -1 && !isThereEarth3(x - 1, y) && !isThereBoulder(x, y, 0));
        case GraphObject::right:
            return (x != 61 && !isThereEarth3(x + 1, y) && !isThereBoulder(x + 1, y, 0));
        case GraphObject::up:
            return (y != 64 && !isThereEarth3(x, y + 1) && !isThereBoulder(x, y + 1, 0));
        case GraphObject::down:
            return (y != 64 && !isThereEarth3(x, y - 1) && !isThereBoulder(x, y - 1, 0)); //Might need to change this radius 
        case GraphObject::none:
            return false;
        }
        return false;


    }
}

LivingActor* StudentWorld::detectNearProtester(Actor* actor, int radius)
{
    vector<LivingActor*>::iterator it;
    for (it = m_alive_actors.begin(); it != m_alive_actors.end(); it++) {
        if ((*it)->getID() == TID_PROTESTER || (*it)->getID() == TID_HARD_CORE_PROTESTER) {
            if (inRadius(actor->getX(), actor->getY(), (*it)->getX(), (*it)->getY(), radius)) {
                // Check if the actor is a RegularProtester or HardCoreProtester
                LivingActor* livingActor = dynamic_cast<LivingActor*>(*it);
                if (livingActor != nullptr) {
                    return livingActor;
                }
            }
        }
    }
    return nullptr;
}


GraphObject::Direction StudentWorld::sensePlayer(LivingActor* p, int W)
{
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            m_maze[i][j] = 0;
        }
    }
    int a = p->getX();
    int b = p->getY();
    queue<Grid> q;
    q.push(Grid(getPlayer()->getX(), getPlayer()->getY()));
    m_maze[getPlayer()->getX()][getPlayer()->getY()] = 1;
    while (!q.empty()) {
        Grid c = q.front();
        q.pop();
        int x = c.x;
        int y = c.y;

        //left
        if (canMoveInDirection(x, y, GraphObject::left) && m_maze[x - 1][y] == 0) {
            q.push(Grid(x - 1, y));
            m_maze[x - 1][y] = m_maze[x][y] + 1;
        }
        //right
        if (canMoveInDirection(x, y, GraphObject::right) && m_maze[x + 1][y] == 0) {
            q.push(Grid(x + 1, y));
            m_maze[x + 1][y] = m_maze[x][y] + 1;
        }
        //up
        if (canMoveInDirection(x, y, GraphObject::up) && m_maze[x][y + 1] == 0) {
            q.push(Grid(x, y + 1));
            m_maze[x][y + 1] = m_maze[x][y] + 1;
        }
        // down
        if (canMoveInDirection(x, y, GraphObject::down) && m_maze[x][y - 1] == 0) {
            q.push(Grid(x, y - 1));
            m_maze[x][y - 1] = m_maze[x][y] + 1;
        }
    }
    if (m_maze[a][b] <= W + 1) {
        if (canMoveInDirection(a, b, GraphObject::left) && m_maze[a - 1][b] < m_maze[a][b])
            return GraphObject::left;
        if (canMoveInDirection(a, b, GraphObject::right) && m_maze[a + 1][b] < m_maze[a][b])
            return GraphObject::right;
        if (canMoveInDirection(a, b, GraphObject::up) && m_maze[a][b + 1] < m_maze[a][b])
            return GraphObject::up;
        if (canMoveInDirection(a, b, GraphObject::down) && m_maze[a][b - 1] < m_maze[a][b])
            return GraphObject::down;
    }
    return GraphObject::none;

}

void StudentWorld::generateRandomCoordinates(int& x, int& y, char o)
{
    //Randomizer range from website 
    do {
        x = rand() % 60 + 1; //random x from 1 to 60

        if (o == 'B')
        {
            y = rand() % 36 + 21; //random y from 21 to 56 for boulders
        }
        else
        {
            y = rand() % 56 + 1; //random y from 1 to 56 for other objects
        }
    } while (ActorWithinRadius(x, y, 6) || (x>26 && x < 34 && y >0)); // Make sure not too clumped together 
}

void StudentWorld::GRCForWATER(int& x, int& y, int depth) //The only way I could get this to work is with a buffer using depth as a stopping point 
{
    const int MAX_DEPTH = 100; 
    if (depth > MAX_DEPTH)
    {
        x = rand() % 4 + 30; //Spawn it in an initialized empty space
        y = rand() % (60 - 4) + 4; 
        return;
    }

    x = rand() % 60 + 1; // Random x from 1 to 60
    y = rand() % 60 + 1; // Random y from 1 to 60
     
    if (isThereEarth2(x, y, 4)) //If no coordinates found 
    {
        GRCForWATER(x, y, depth + 1); //Recursive call with incremented depth 
    }
}

bool StudentWorld::isPlayerinRadius(Actor* a,  int radius) const //Helper function to check if player is within radius of object/living actor 
{
    return(inRadius(getPlayer()->getX(), getPlayer()->getY(), a->getX(), a->getY(), radius));

}


void StudentWorld::addObject(int num, char o) // 'B' for boulder, 'G' for gold, 'O' for Oil Barrel 
{
    bool generated = false;
    int x, y;
    int attempts = 0; // Add a limit on the number of attempts to prevent infinite recursion

    while (num > 0 && attempts < 10) // Limit the number of attempts to prevent infinite recursion
    {
        generateRandomCoordinates(x, y, o); // Ensure coordinates are valid

        if (!ActorWithinRadius(x, y, 10))
        {
            switch (o)
            {
            case 'B':
            {
                vector<Boulder*> boulders = getBoulders();
                int numBoulder = std::min(static_cast<int>(getLevel() / 2) + 2, 9);

                if (boulders.size() < numBoulder)
                {
                    m_objects.push_back(new Boulder(this, x, y, 1));
                    for (int i1 = x; i1 < x + 4; i1++)
                    {
                        for (int i2 = y; i2 < y + 4; i2++)
                        {
                            if (i1 < 64 && i2 < 60) // Ensure within bounds
                            {
                                delete m_earth[i1][i2];
                                m_earth[i1][i2] = nullptr;
                            }
                        }
                    }
                    generated = true;
                }
            }
            break;
            case 'G':
            {
                vector<Gold*> golds = getGolds();
                int numGold = std::max(5 - static_cast<int>(getLevel() / 2), 2);

                if (golds.size() < numGold)
                {
                    Gold* gold = new Gold(this, x, y, 1);
                    gold->setVisible(false);
                    m_actors.push_back(gold);
                    generated = true;
                }
            }
            break;
            case 'O':
            {
                vector<Barrel*> oilBarrels = getBarrels();
                int numOil = std::min(static_cast<int>(2 + getLevel()), 21);

                if (oilBarrels.size() < numOil)
                {
                    Barrel* barrel = new Barrel(this, x, y, 1);
                    barrel->setVisible(false);
                    m_actors.push_back(barrel);
                    generated = true;
                }
            }
            break;
            }

            if (generated)
            {
                num--; // Decrease the number of objects to be added
            }
        }

        attempts++; // Increment the number of attempts
    }

    // If not enough objects were generated, call addObject again with the same parameters
    if (num > 0 && attempts >= 10)
    {
        //addObject(num, attempts); 
    }
}








Actor* StudentWorld::getActorAt(int x, int y) const
{
    for (Actor* actor : m_actors) {
        if (actor->getX() == x && actor->getY() == y) {
            return actor;
        }
    }
    return nullptr;
}

int StudentWorld::init()
{
    m_firstTick = true;
    m_tickSinceLast = 0;
    m_protesters = 0;
    m_barrelsLeft = 0;

    for (int x = 0; x < 64; x++) {
        for (int y = 0; y < 60; y++) {
            if (x < 30 || x > 33 || y < 4) {
                m_earth[x][y] = new Earth(this, x, y);
            }
            else {
                m_earth[x][y] = nullptr;
            }
        }
    }
    m_player = new Tunnelman(this, 30, 60, 5, 1, 0);

    int numBoulder = std::min((getLevel() / 2) + 2, static_cast<unsigned int>(9));
    int numGold = std::max(5 - getLevel() / 2, static_cast<unsigned int>(2));
    int numOil = std::min(2 + getLevel(), static_cast<unsigned int>(21));

    addObject(numBoulder, 'B');
    

    addObject(numGold, 'G');

    addObject(numOil, 'O'); 
    setBarrelsLeft(numOil); 

    return GWSTATUS_CONTINUE_GAME;




}

bool StudentWorld::isThereBoulder(int x, int y, int radius)
{
    //Check for boulders in the m_objects vector
    for (vector<Object*>::iterator it = m_objects.begin(); it != m_objects.end(); ++it)
    {
        if ((*it)->getID() == TID_BOULDER && inRadius(x, y, (*it)->getX(), (*it)->getY(), radius))
        {
            return true;
        }
    }

    //Check for boulders in the m_actors vector
    for (vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); ++it)
    {
        Boulder* boulder = dynamic_cast<Boulder*>(*it);
        if (boulder && inRadius(x, y, boulder->getX(), boulder->getY(), radius))
        {
            return true;
        }
    }

    return false;
}
 


bool StudentWorld::isBlocked(int x, int y, GraphObject::Direction d)
{
    if (x < 0 || x > 60 || y < 0 || y > 60) {
        return true;
    }

    int nextPlayerX = m_player->getX();
    int nextPlayerY = m_player->getY();
    int nextX = x;
    int nextY = y;
    switch (d) {
    case GraphObject::left:
        nextX -= 1;
        nextPlayerX -= 1;
        break;
    case GraphObject::right:
        nextX += 1;
        nextPlayerX += 1;
        break;
    case GraphObject::down:
        nextY -= 1;
        nextPlayerY -= 1;
        break;
    case GraphObject::up:
        nextY += 1;
        nextPlayerY += 1;
        break;
    default:
        break;
    }

    if (nextX < -1 || nextX > 61 || nextY < -1 || nextY > 61) {
        return true;
    }

    Actor* actor = getActorAt(nextX, nextY);
    if (actor != nullptr) 
    {
        if (dynamic_cast<Boulder*>(actor) != nullptr) {
            return true; // Blocked by Boulder
        }
        return false;
    }

    // No actor at the next position, so check for Boulder in the path
    if (isThereBoulder(nextX, nextY, 2)) {
        return true; // Blocked by Boulder
    }


    return false;
}

int StudentWorld::move()
{
    updateDisplay(); 
   

    if (m_barrelsLeft == 0)
    {
        return GWSTATUS_FINISHED_LEVEL; 
        advanceToNextLevel(); 
    }

   // tickCounter++; //Each time water object created, increment counter 
    if (m_player->getHealth() <= 0)
    {
        m_player->setAlive(false); 
    }

    addGoodies(); 
    addProtester();
    

    vector <Actor*> toRemove; //List of items that are collected 
    m_player->doSomething();
    if (!getPlayer()->isAlive()) {
        decLives();
        return GWSTATUS_PLAYER_DIED;
        playSound(SOUND_PLAYER_GIVE_UP); 
    }

    for (auto LIT = m_alive_actors.begin(); LIT != m_alive_actors.end(); ++LIT) {
        if ((*LIT)->isCurrentlyAlive()) {
            (*LIT)->doSomething();
        }
    }

    for (auto it = m_objects.begin(); it != m_objects.end();) {
        if (!(*it)->PickedUp(*getPlayer())) {
            (*it)->doSomething();
        }
        Boulder* b = dynamic_cast<Boulder*>(*it); 
        if (b && !b->isAlive()) //If boulder found dead 
        {
            toRemove.push_back(*it);
            it = m_objects.erase(it); //remove from vector 
        }
        else {
            ++it; 
        }
    }

    

    Actor* tempPlayer = getActorAt(m_player->getX(), m_player->getY());

    if (Earth* e = dynamic_cast<Earth*>(tempPlayer))
    {
        dig(m_player->getX(), m_player->getY());
        playSound(SOUND_DIG);

    }

    vector<Actor*> ::iterator it3; 
    
   
    vector<Actor*> pickedUpObjects;
    for (it3 = m_actors.begin(); it3 != m_actors.end(); it3++)
    {
        
        Object* obj = dynamic_cast<Object*>(*it3);
        Barrel* barrel = dynamic_cast<Barrel*>(*it3); //Check for barrel 
        Gold* gold = dynamic_cast<Gold*>(*it3); //Check for gold 
        Water* water = dynamic_cast<Water*>(*it3); //Check for water 
        Sonar* sonar = dynamic_cast<Sonar*>(*it3); //Check for sonar 
        Squirt* squirt = dynamic_cast<Squirt*>(*it3); //Check for squirt 
            
            if (squirt)
            {
            (*it3)->doSomething();
            }

            if (obj && isPlayerinRadius(obj, 5))
            {
                if (!obj->isVisible())
                {
                    obj->setVisibility(true);
                }

            }

            if (obj && isPlayerinRadius(obj, 2) && !obj->isPickedUp()) {
                if (barrel)
                {
                    playSound(SOUND_FOUND_OIL); 
                    increaseScore(1000); 
                    setBarrelsLeft(m_barrelsLeft - 1); 
                }
                
                else {
                    if (water)
                    {
                        increaseScore(100); 
                        m_player->addSquirts(5); 
                    }
                    if (gold)
                    {
                        if (!gold->isPickedUp())
                        {
                            increaseScore(10);
                            m_player->addGold(1);
                        }
                       
                    }
                    if (sonar)
                    {
                        increaseScore(75); 
                        m_player->addSonar(1); 
                    }



                    playSound(SOUND_GOT_GOODIE);
                }
                toRemove.push_back(obj);
            }
            if (gold && gold->isPickedUp() && gold->isAlive()) {

                gold->doSomething();
                if (!gold->isAlive())
                {
                    toRemove.push_back(gold);
                }
            }

    }
    

    for (std::vector<Actor*>::iterator removeIter = toRemove.begin(); removeIter != toRemove.end(); ++removeIter)
    {
        Actor* obj = *removeIter;
        m_actors.erase(std::remove(m_actors.begin(), m_actors.end(), obj), m_actors.end());
        delete obj;
    }

   
    return GWSTATUS_CONTINUE_GAME;
}