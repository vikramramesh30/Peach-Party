#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include "Board.h"
#include <string>
#include <vector>


// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class Actor;
class Player;

class StudentWorld : public GameWorld
{
public:
  StudentWorld(std::string assetPath);
  virtual ~StudentWorld();
  virtual int init();
  virtual int move();
  virtual void cleanUp();

  //helper player and square methods
  bool containsSquare(int x, int y);
  bool containsPlayer(int x, int y);
  Player* getPlayer(int x, int y);
  Actor* getSquare(int x, int y); 
  void destroySquare(int x, int y);
  void addSquare(Actor* actor);

  //for bank square
  int getBankBalance() {return m_bank; } 
  void setBankBalance(int balance){m_bank = balance;}

  //player getters
  Player* getPeach(){return m_peach;}
  Player* getYoshi(){return m_yoshi;}

  //helper vector methods
  Actor* getImpact(int x, int y);
  void createVortex(int x, int y, int direction);

private:
    Player* m_peach;
    Player* m_yoshi;
    int m_bank;
    //actor vector
    std::vector<Actor*> m_actors;
};

#endif // STUDENTWORLD_H_
