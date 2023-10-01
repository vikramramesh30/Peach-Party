#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"

class StudentWorld;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp


//Actor Base Class
class Actor: public GraphObject
{
        public:
        Actor(StudentWorld* studentWorld, int imageID, double startX, double startY, int startDirection, int depth) 
        : GraphObject(imageID, startX, startY, startDirection, depth)
        {
            m_studentworld = studentWorld;
        }   

        StudentWorld* getWorld()
        {
            return m_studentworld;
        }

        ~Actor(){}

        virtual void doSomething() = 0;
        

        //for actors that are alive or dead
        bool getAlive(){return m_alive;}
        void setAlive(bool alive) {m_alive = alive;}

        
        //to determine if actors are players and moving characters
        virtual bool isPlayer(){return false;}
        virtual bool isMovingActor(){return false;}

        //to check if actors can be hit by vortexes
        virtual bool allowImpact(){return false;}

        //so Actor objects can call runVortex
        virtual void runVortex(){}


    
    private:

        StudentWorld* m_studentworld;
        bool m_alive = true;


    
};


//for actors that move
class MovingActor: public Actor
{
    public:
        MovingActor(StudentWorld* studentWorld, int imageID, double startX, double startY, int startDirection = 0, int depth = 0) 
            : Actor(studentWorld, imageID, startX, startY, startDirection, depth){}


        
        bool canMove(int dir); //to check if actor can move in a direction
        bool isFork(int dir); //to check if a actor is at a fork
        void changeDir(); //helper method to change directions if a actor can't move foward
        int randomDirection(int dir); //makes actor face at a random direction

        //getters
        int getSDirection(){return m_direction; } 
        int getState(){return m_state;}
        int getTicks() {return m_tickToMove;}

        //setters
        void setSDirection(int direction){m_direction = direction;}
        void setState(int state){m_state = state;}
        void setTicks(int ticks){m_tickToMove = ticks; }

        
        virtual bool isMovingActor(){return true;} //to determine if actor is a moving actor

        //teleports actor to a random square
        void randomTP();

    private:
        int m_direction = right;
        int m_tickToMove = 0;
        int m_state = 0;
};

//for players peach and yoshi
class Player: public MovingActor
{
    public:
        Player(StudentWorld* studentWorld, int imageID, double startX, double startY, int player, int startDirection = 0, int depth = 0)
        : MovingActor(studentWorld, imageID, startX, startY, startDirection, depth)
        {
            m_player = player;
        }
        ~Player(){}

        void swap(); //swap to player's attributes

        //getters
        virtual void doSomething();
        virtual bool isPlayer() {return true;} 
        int getPlayer(){return m_player;}
        int getCoins(){return m_coins;}
        int getStars(){return m_stars;}
        bool isOnDirectionSquare(){return m_onDirectionSquare;}
        bool getJustTeleported(){return m_justTeleported;}
        bool hasVortex(){return m_hasVortex;}
        int getDiceRoll(){return m_dice_roll;}
        bool getActivatedSquare(){return m_activated;}


        //setters
        void setCoins(int coins){m_coins = coins;}
        void setStars(int stars) {m_stars = stars;}
        void setActivatedSquare(bool activated){m_activated = activated;}
        void setOnDirectionSquare(bool on){m_onDirectionSquare = on;}
        void setJustTeleported(bool t){m_justTeleported = t;}
        void setDiceRoll(int dice){m_dice_roll = dice;}
        void setHasVortex(bool vortex){m_hasVortex = vortex;}




    private:
        int m_coins = 0;
        int m_stars = 0;
        int m_player;
        bool m_activated = false;
        bool m_onDirectionSquare = false;
        bool m_justTeleported = false;
        bool m_hasVortex = false;
        int m_dice_roll = 0;
};

//Baddie helper class for Boo and Bowser
class Baddie : public MovingActor
{
    public:
    Baddie(StudentWorld* studentWorld, int imageID, double startX, double startY, int startDirection = 0, int depth = 0)
    : MovingActor(studentWorld, imageID, startX, startY, startDirection, depth){}

    //getters
    int getPauseCounter(){return pause_counter;}


    //setters
    void setPauseCounter(int pausecounter) {pause_counter = pausecounter;}

    void decreasePauseCounter()
    {
        pause_counter--;
    }

    virtual bool allowImpact(){return true;} //so baddies can be hit by vortex's

    private:
    int pause_counter = 180;

    virtual void runVortex(); //helper method that teleport's baddies once hit
};

//bowser
class Bowser : public Baddie
{
public:

    Bowser(StudentWorld* studentWorld, int imageID, double startX, double startY, int startDirection = 0, int depth = 0)
    : Baddie(studentWorld, imageID, startX, startY, startDirection, depth){}
	virtual void doSomething();

};

//boo
class Boo : public Baddie
{
public:
    Boo(StudentWorld* studentWorld, int imageID, double startX, double startY, int startDirection = 0, int depth = 0)
    : Baddie(studentWorld, imageID, startX, startY, startDirection, depth){}
    virtual void doSomething();

};

//square base class for all squares
class Square: public Actor
{
    public: 
        Square(StudentWorld* studentWorld, int imageID, double startX, double startY, int startDirection, int depth)
        : Actor(studentWorld, imageID, startX, startY, startDirection, depth){}
        ~Square(){} 

    virtual void doSomething();

    //function that runs if a player lands on square
    virtual void landFunction(Player* player) = 0;

    //function that runs if a player passes a square
    virtual void passFunction(Player* player) = 0;
};

//coin square
class CoinSquare: public Square
{
    public: 
        CoinSquare(StudentWorld* studentWorld, int imageID, double startX, double startY, int coinMod, int startDirection = 0, int depth = 1)
        : Square(studentWorld, imageID, startX, startY, startDirection, depth)
        {
            m_coinMod = coinMod;
        }
        ~CoinSquare(){}
    
    virtual void landFunction(Player* player);

    virtual void passFunction(Player* player);

    int getCoinMod() 
    {
        return m_coinMod;
    }

    private:
    
    int m_coinMod; //coin modifier (+ for blue, - for red)

};

//event square
class EventSquare: public Square
{
    public: 
        EventSquare(StudentWorld* studentWorld, int imageID, double startX, double startY, int startDirection = 0, int depth = 1)
        : Square(studentWorld, imageID, startX, startY, startDirection, depth){}
        ~EventSquare(){}
    
    virtual void landFunction(Player* player);

    virtual void passFunction(Player* player);

};

//bank square
class BankSquare: public Square
{
    public: 
        BankSquare(StudentWorld* studentWorld, int imageID, double startX, double startY, int startDirection = 0, int depth = 1)
        : Square(studentWorld, imageID, startX, startY, startDirection, depth){}
        ~BankSquare(){}

    virtual void landFunction(Player* player);

    virtual void passFunction(Player* player);
    

};

//star square
class StarSquare: public Square
{
    public: 
        StarSquare(StudentWorld* studentWorld, int imageID, double startX, double startY, int startDirection = 0, int depth = 1)
        : Square(studentWorld, imageID, startX, startY, startDirection, depth){}
        ~StarSquare(){}
    
    virtual void landFunction(Player* player);

    virtual void passFunction(Player* player);

};

//direcion square
class DirectionSquare: public Square
{
    public: 
        DirectionSquare(StudentWorld* studentWorld, int imageID, double startX, double startY, int startDirection, int depth = 1)
        : Square(studentWorld, imageID, startX, startY, startDirection, depth)
        {
            m_dir = startDirection;
        }
        ~DirectionSquare(){}


        int getForcedDirection()
        {
            return m_dir;
        }

    virtual void landFunction(Player* player);

    virtual void passFunction(Player* player);

    private:
        int m_dir; //forced direction
};

//dropping square
class DroppingSquare: public Square
{
    public: 
        DroppingSquare(StudentWorld* studentWorld, int imageID, double startX, double startY, int startDirection = 0, int depth = 1)
        : Square(studentWorld, imageID, startX, startY, startDirection, depth){}
        ~DroppingSquare(){}
        

    virtual void landFunction(Player* player);

    virtual void passFunction(Player* player);
};


//vortex
class Vortex: public Actor
{
    public:
        Vortex(StudentWorld* studentWorld, int imageID, double startX, double startY, int direction, int depth = 1)
        : Actor(studentWorld, imageID, startX, startY, direction, depth)
        {
            m_direction = direction;
        }

    virtual void doSomething(); 

    int getVDirection()
    {
        return m_direction;
    }

    private:
        int m_direction; //vortex direction

    
};

#endif // ACTOR_H_
