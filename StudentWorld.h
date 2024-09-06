#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <vector>

class Tunnelman;
class Object;
class Actor;
class RegularProtester;
class LivingActor;
class Earth;
class Boulder;
class Gold;
class Barrel;

class StudentWorld : public GameWorld
{
public:


    StudentWorld(std::string assetDir)
        : GameWorld(assetDir), m_firstTick(true), m_tickSinceLast(0), m_protesters(0), m_player(nullptr), m_barrelsLeft(0), m_pLeaving(false)
    {
    }

    virtual ~StudentWorld() {
        cleanUp();
    }

    virtual int init();

    virtual int move();
    virtual void cleanUp();
    void updateDisplay();

    std::vector<Barrel*> getBarrels();
    std::vector<Gold*> getGolds();
    std::vector<Boulder*> getBoulders() const;
    std::string displayText(int score, int level, int lives, int health, int squirts, int gold, int sonar, int barrels);
    Earth* getEarth() const { return m_earth[64][64]; }
    Tunnelman* getPlayer() const { return m_player; }
    int getNumActors() const { return m_num_actors; }
    Actor* getActorAt(int x, int y) const;
    void detectNearActor(int x, int y, int radius);
    bool inRadius(int x1, int y1, int x2, int y2, int radius) const;
    bool ActorWithinRadius(int x, int y, int radius);
    bool isPlayerinRadius(Actor* a, int radius) const;
    void addActor(Actor* actor);
    void addProtester();
    void decProtester()
    {
        m_protesters--; 
    }
    bool dig(int x, int y);
    bool isBlocked(int x, int y, GraphObject::Direction d);
    GraphObject::Direction sensePlayer(LivingActor* p, int W);
    void generateRandomCoordinates(int& x, int& y, char  o); //Coordinates dependent on the input (object type) 
    void addObject(int num, char object);
    //unsigned int getLevel() const { return m_level; }
    bool isThereBoulder(int x, int y, int radius); 
    bool isThereEarth1(int x, int y, int radius); //For digging
    bool isThereEarth2(int x, int y, int radius); //For spawning water objects
    bool isThereEarth3(int x, int y); //For restricting protester movements (no radius) 
    bool aboveEarth(int x, int y); 
    bool BoulderAbovePlayer(int x, int y); 
    void GRCForWATER(int& x, int& y, int depth = 0); 
    void markAsDug(int x, int y); 
    void setBarrelsLeft(int barrels)
    {
        m_barrelsLeft = barrels; 
    }
    void addGoodies(); 
    void leave(RegularProtester* pr); 
    bool canMoveInDirection(int x, int y, GraphObject::Direction direction);
    LivingActor* detectNearProtester(Actor*actor, int radius); 
private:
    bool m_firstTick;
    int m_tickSinceLast;
    int m_protesters;
    int m_barrelsLeft;
    Earth* m_earth[64][64];
    Tunnelman* m_player;
    std::vector<Actor*> m_actors;
    bool m_pLeaving; 
    std::vector<LivingActor*> m_alive_actors;
    std::vector<Object*> m_objects;
    //int m_level = GameWorld::getLevel();
    int m_num_actors = 0;

    int m_maze[64][64]; 
    struct Grid {
        int x;
        int y;
        Grid(int a, int b) : x(a), y(b) {}
    };
};

#endif // STUDENTWORLD_H_
