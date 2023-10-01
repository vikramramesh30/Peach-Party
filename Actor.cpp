#include "Actor.h"
#include "GameWorld.h"
#include "StudentWorld.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

//Swaps attributes of two player objcts 
void Player::swap()
{
    Player* p1 = getWorld()->getPeach();
    Player* p2 = getWorld()->getYoshi();
    int dir = p1->getSDirection();
    int dice_roll = p1->getDiceRoll();
    int state = p1->getState();
    int ticks = p1->getTicks();
    int x = p1->getX();
    int y = p1->getY();
    p1->setState(p2->getState());
    p1->setDiceRoll(p2->getDiceRoll());
    p1->setSDirection(p2->getSDirection());
    p1->setTicks(p2->getTicks());
    p1->moveTo(p2->getX(),p2->getY());
    p2->setTicks(ticks);
    p2->setSDirection(dir);  
    p2->moveTo(x,y);
    p2->setState(state);
    p2->setDiceRoll(dice_roll);
    if(this==p1)
    {
        p1->setActivatedSquare(false);
        p2->setActivatedSquare(true);
    }
     else
    {
        p1->setActivatedSquare(true);
        p2->setActivatedSquare(false);
    }



}

//checks if a moving actor can move in a certain direction
bool MovingActor::canMove(int direction)
{
    switch(direction)
    {
        case right:
            if(getWorld()->containsSquare(getX() + BOARD_WIDTH, getY()))
            {
                return true;
            }
            break;
        case up:
            if(getWorld()->containsSquare(getX(), getY() + BOARD_HEIGHT))
            {
                return true;
            }
            break;
        case down:
            if(getWorld()->containsSquare(getX(), getY() - BOARD_HEIGHT))
            {
                return true;
            }
            break;
        case left:  
            if(getWorld()->containsSquare(getX() - BOARD_WIDTH, getY()))
            {
                return true;
            }
            break;
    }


    return false;
}


//checks if a pathway is a fork
bool MovingActor::isFork(int dir)
{
    int count = 0;
    for(int i = 0; i < 4; i++)
    {
        if(canMove(i*90))
        {
            count++;
        } 
    }

    return count > 2;
}

//teleports a moving actor to a random location on the board
void MovingActor::randomTP()
{
    int x = randInt(0,15) * 16;
    int y = randInt(0,15) * 16;
    while(!getWorld()->containsSquare(x,y))
    {
        x = randInt(0,15) * 16;
        y = randInt(0,15) * 16;
        moveTo(x,y);
    }
}


//adjusts the direction of a moving actor
void MovingActor::changeDir()
{
    if(getSDirection() == right || getSDirection() == left)
    {
        setSDirection(up);
        if(canMove(getSDirection()))
        {
           return;
        }
        setSDirection(down);
        return;
    }

    if(getSDirection() == up || getSDirection() == down)
    {
        setSDirection(right);
        if(canMove(getSDirection()))
        {
            return;
        }
        setSDirection(left);
        return;
    }
}

//generates a random direction that a moving actor can face
int MovingActor::randomDirection(int dir)
{
    int rand_direction = randInt(0,3) * 90;
    while(!canMove(rand_direction) && rand_direction != dir)
    {
        rand_direction = randInt(0,3) * 90;
    }

    return rand_direction;
} 

//main player do something method
void Player::doSomething()
{
    //checks if state is paused
    if(getState() == 0)
    {   
        //adjusts direction if player teleports and is facing an invalid direction
        if(!canMove(getSDirection()) && getJustTeleported())
        {
            setSDirection(randomDirection(getSDirection()));
            if(getSDirection() == left)
            {
                setDirection(180);
            }
            {
                setDirection(0);
            }

        }

        //rolls the die when the player presses roll
        int action = getWorld()->getAction(getPlayer());
        if(action == ACTION_ROLL)
        {   
            setDiceRoll(randInt(1,10));
            setTicks(getDiceRoll() * 8);
            setState(1);

        }   
        //shoots a vortex if a player has a vortex and presses fire
        else if(hasVortex() && action == ACTION_FIRE)
        {
              getWorld()->createVortex(getX(), getY(), getSDirection());
              getWorld()->playSound(SOUND_PLAYER_FIRE);

              setHasVortex(false);
        }
        else
        {
            return;
        }

    }
    //checks if state is walking
    else if(getState() == 1)
    {   
        //if the player is aligned with teh square
        if((getX() % BOARD_WIDTH) == 0 && (getY() % BOARD_HEIGHT) == 0)
        {
            //checks if player already has their direction determined
            if(isOnDirectionSquare())
            {
                setOnDirectionSquare(false);
            }
            //request user input if they are at a fork
            else if(isFork(getSDirection()))
            {
                int dir;
                switch((getWorld()->getAction(getPlayer())))
                {
                    case ACTION_RIGHT:
                        if(canMove(right))
                        {
                            setSDirection(right);
                            setDirection(0);
                        }
                        break;
                    case ACTION_UP:
                        if(canMove(up))
                        {
                            setSDirection(up);
                            setDirection(0);

                        }
                        break;
                    case ACTION_DOWN:
                        if(canMove(down))
                        {
                            setSDirection(down);
                            setDirection(0);

                        }
                        break;
                    case ACTION_LEFT:
                        if(canMove(left))
                        {
                            setSDirection(left);
                            setDirection(left);

                        }
                        break;
                    default:
                        return;
                }


  
            }
            //change direction if player can no longer move forward
            else if(!canMove(getSDirection()))
            {
                changeDir();
                if (getSDirection() == left)  
                { 
				    setDirection(left);
                }
			    else
                {
				    setDirection(right);
                }
            }
 
        }
        //moves forward and adjusts ticks
        moveAtAngle(getSDirection(), 2);

        setTicks(getTicks() - 1);
        if(getTicks() == 0 )
        {
            setState(0);
        }
    }

}

//----Square----

//square base class for all square sub classes

void Square::doSomething()
{
    //checks if square is alive
    if(!getAlive())
    {
        return;
    }

    //checks if a player landed or passed the square, runs a seperate function for each
    if(getWorld()->containsPlayer(getX(), getY()))
    {
        Player* p = getWorld()->getPlayer(getX(), getY());
        if(p->getState() == 0)
        {
            if(!p->getActivatedSquare())
            {
                //flag to activate a square
                p->setActivatedSquare(true);
                landFunction(p);
            }
        }
        else
        {
            passFunction(p);
            p->setActivatedSquare(false);
        }
    }
}

//----Coin Square----

void CoinSquare::landFunction(Player* p)
{
    //gives coins if coin square is blue
    if(getCoinMod() > 0)
    {
        p->setCoins(p->getCoins() + getCoinMod());
        getWorld()->playSound(SOUND_GIVE_COIN);
    }
    //substracts coins if coin square is red
    else
    {
        if(p->getCoins() <= getCoinMod())
        {
            p->setCoins(0);
        }
        else
        {
            p->setCoins(p->getCoins() - getCoinMod());
        }
    }


}


void CoinSquare::passFunction(Player* p){}

//----Bank Square----


//gives bank to player if they land on it
void BankSquare::landFunction(Player *p)
{
    p->setCoins(p->getPlayer() + getWorld()->getBankBalance());
    getWorld()->setBankBalance(0);
}

//takes player's money and adds to bank if they pass by it
void BankSquare::passFunction(Player *p)
{
    int coins = p->getCoins();
    if(p->getCoins() < 5)
    {
        p->setCoins(0);
        getWorld()->setBankBalance(getWorld()->getBankBalance() + coins);
        getWorld()->playSound(SOUND_DEPOSIT_BANK);
    }
    else
    {
        p->setCoins(p->getCoins() - 5);
        getWorld()->setBankBalance(getWorld()->getBankBalance() + 5);
        getWorld()->playSound(SOUND_WITHDRAW_BANK);
    }

    
}


//----Star Square----


//adjusts stars when player lands on square
void StarSquare::landFunction(Player* p)
{
    if(p->getCoins() < 20)
    {   
        return;
    }
    p->setCoins(p->getCoins() - 20);
    p->setStars(p->getStars() + 1);
    getWorld()->playSound(SOUND_GIVE_STAR);
}

void StarSquare::passFunction(Player* p)
{
    landFunction(p);
}



//----Event Square----


//teleports, swaps, or give's player vortex randomly if they land on square
void EventSquare::landFunction(Player *p)
{    
    int event = randInt(1,3);
    switch(event)
    {
        case 1:
        {
            p->randomTP();
            getWorld()->playSound(SOUND_PLAYER_TELEPORT);
            p->setJustTeleported(true);
            break;
        }
         case 2:
             p->swap();
             getWorld()->playSound(SOUND_PLAYER_TELEPORT);
             break;
        case 3:
            if(!p->hasVortex())
            {
                p->setHasVortex(true);
            }
            getWorld()->playSound(SOUND_GIVE_VORTEX);
            break;

    }

}

void EventSquare::passFunction(Player *p){}


//----Direction Square----


//sets players direction to a forced direction
void DirectionSquare::landFunction(Player *p)
{
    p->setSDirection(getForcedDirection());
    p->setOnDirectionSquare(true);
    
}

void DirectionSquare::passFunction(Player *p)
{
    landFunction(p);
}

//----Dropping Square----


//takes a player's coins or stars if a player lands on the square
void DroppingSquare::landFunction(Player *p)
{
    int one_or_two = randInt(1,2);
    switch(one_or_two){
        case 1:
        {
            if(p->getCoins() <= 10)
            {
                p->setCoins(0);
            }
            else
            {
                p->setCoins(p->getCoins() - 10);
            }
            break;
        }
        case 2:
        {
            if(p->getStars() > 0)
            {
                p->setStars(p->getStars() + 1);
            }
        }
    }
    getWorld()->playSound(SOUND_DROPPING_SQUARE_ACTIVATE);
}

void DroppingSquare::passFunction(Player *p){}



//----Boo----


//main boo do something method
void Boo::doSomething()
{
    //checks if boo is in a paused state
    if(getState() == 0)
    {

        //checks if player is on the same square
        if(getWorld()->containsPlayer(getX(), getY()) && getWorld()->getPlayer(getX(), getY())->getState() == 0)
        {

            int one_or_two = randInt(1,2);

            //swamps both players coins or stars
            if(one_or_two == 1)
            {
                int temp = getWorld()->getPeach()->getCoins();
                getWorld()->getPeach()->setCoins(getWorld()->getYoshi()->getCoins());
                getWorld()->getYoshi()->setCoins(temp);
            }
            else
            {
                int temp = getWorld()->getPeach()->getStars();
                getWorld()->getPeach()->setStars(getWorld()->getYoshi()->getStars());
                getWorld()->getYoshi()->setStars(temp);
                

            }

            getWorld()->playSound(SOUND_BOO_ACTIVATE);

        }

        
        //pauses and changes directions
        decreasePauseCounter();

        if(getPauseCounter() == 0)
        {
            int squares_to_move = randInt(1,3);
            setTicks(squares_to_move * 8);
            int random_direction = randomDirection(getSDirection());
            setSDirection(random_direction);
            if(random_direction==left)
            {
                 setDirection(180);
            }
            else
            {
                setDirection(0);
            }
            

            setState(1);


         }


        
    }
    //checks if boo is walking
    else if(getState() == 1)
    {
        //checks if it is aligned
        if((getX() % BOARD_WIDTH) == 0 && (getY() % BOARD_HEIGHT) == 0)
         {
            //checks and adjusts directions at a fork if needed
            if(isFork(getSDirection()))
            {
                int random_direction = randomDirection(getSDirection());
                setSDirection(random_direction);

                if(random_direction== left)
                {
                    setDirection(180);
                }
                else
                {
                    setDirection(0);
                }

             }
        //checks if it can move forward and adjusts direction if needed
        if(!canMove(getSDirection()))
        {
            changeDir();
            if (getSDirection() == left)  
            { 
				setDirection(left);
            }
			else
            {
			    setDirection(right);
             }
         }
             


      } 

        //moves boo and adjusts ticks
        moveAtAngle(getSDirection(), 2);
        setTicks(getTicks() - 1);
        if(getTicks() == 0)
         {
            setState(0);
            setPauseCounter(180);
        }
       
    }
    
}

//----Bowser----

//main bowser do something method
void Bowser::doSomething()  
{
    //checks if bowser is in the paused state
    if(getState() == 0)
    {
        //checks if player is on it
        if(getWorld()->containsPlayer(getX(), getY()) && getWorld()->getPlayer(getX(), getY())->getState() == 0)
        {
            
            int one_or_two = randInt(1,2);
            //resets the players coins 50% of the time
            if(one_or_two == 1)
            {
                getWorld()->getPlayer(getX(),getY())->setCoins(0);
                getWorld()->getPlayer(getX(),getY())->setStars(0);
                getWorld()->playSound(SOUND_BOWSER_ACTIVATE);
            }
            

            getWorld()->playSound(SOUND_BOO_ACTIVATE);

        }

        //pauses and changes to a random direction
        decreasePauseCounter();

        if(getPauseCounter() == 0)
        {
            int squares_to_move = randInt(1,10);
            setTicks(squares_to_move * 8);
            int random_direction = randomDirection(getSDirection());
            setSDirection(random_direction);
            if(random_direction==left)
            {
                 setDirection(180);
            }
            else
            {
                setDirection(0);
            }
            

            setState(1);


         }


        
    }
    //checks if bowser is walking
    else if(getState() == 1)
    {
        //checks if bowser is aligned with the square
        if((getX() % BOARD_WIDTH) == 0 && (getY() % BOARD_HEIGHT) == 0)
         {
            //checks and adjusts at a fork
            if(isFork(getSDirection()))
            {
                int random_direction = randomDirection(getSDirection());
                setSDirection(random_direction);

                if(random_direction== left)
                {
                    setDirection(180);
                }
                else
                {
                    setDirection(0);
                }

             }
        //checks and adjusts if it can't move forward
        if(!canMove(getSDirection()))
        {
            changeDir();
            if (getSDirection() == left)  
            { 
				setDirection(left);
            }
			else
            {
			    setDirection(right);
             }
         }
             


      }    

        //moves and sets tickets to correct amount

        moveAtAngle(getSDirection(), 2);
        setTicks(getTicks() - 1);
        if(getTicks() == 0)
         {
            setState(0);
            setPauseCounter(180);
            int random = randInt(1,4);
            //adds a dropping square below bowser 25% of the time
            if(random == 1)
            {
                getWorld()->getSquare(getX(),getY())->setAlive(false);
                getWorld()->destroySquare(getX(),getY());
                getWorld()->addSquare(new DroppingSquare(getWorld(), IID_DROPPING_SQUARE, getX(), getY()));
                getWorld()->playSound(SOUND_DROPPING_SQUARE_CREATED);

            }
        }

       
    }
}


//----Vortex----

//vortex helper method to move baddie if hit by vortex
void Baddie::runVortex()
{
    randomTP();
    setSDirection(right);
    setState(0);
    setPauseCounter(180);
}

//vortex main do something method
void Vortex::doSomething()
{
    //exits if vector is dead
    if(!getAlive())
    {
        return;
    }

    //moves vector
    moveAtAngle(getVDirection(), 2);

    //checks if vector is in the screen
    if(getX() < 0 || getX() >= VIEW_WIDTH || getY()<0 || getY()>=VIEW_HEIGHT)
    {
        setAlive(false);
        return;
    }

    //runs vortex's helper methods if baddie is hit
    Actor* impact = getWorld()->getImpact(getX(), getY());
    if(impact != nullptr)
    { 
        impact->runVortex();
        setAlive(false);
        getWorld()->playSound(SOUND_HIT_BY_VORTEX);
    }
    


    


};