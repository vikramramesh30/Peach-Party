#include "StudentWorld.h"
#include "GameConstants.h"
#include "Board.h"
#include "Actor.h"
#include "GameWorld.h"
#include "GameConstants.h"
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath), m_peach(nullptr)
{ }



//loops through actor vector to see if square is at a position
bool StudentWorld::containsSquare(int x, int y)
{
    for(Actor* actor: m_actors)
    {
        if(!actor->isPlayer())
        {
            if(actor->getX() == x && actor->getY() == y) 
            {
                return true;
            }
        }
    }
    
    return false;
}

//loops through actor vector to see if a player is at a position
bool StudentWorld::containsPlayer(int x, int y)
{
    for(Actor* actor: m_actors)
    {
        if(actor->isPlayer())
        {
            if(actor->getX() == x && actor->getY() == y) 
            {
                return true;
            }
        }
    }
    
    return false;
}


//loops through actor vector to see if a square is a position
Actor* StudentWorld::getSquare(int x, int y)
{
    for(Actor* s: m_actors)
    {
        if(s->getX() == x && s->getY() == y && !s->isMovingActor())
        {
            return s;
        }
    }

    return nullptr;
}

//gets the player at a position
Player* StudentWorld::getPlayer(int x, int y)
{
    if(getPeach()->getX() == x && getPeach()->getY() == y)
    {
        return getPeach();
    }

    if(getYoshi()->getX() == x && getYoshi()->getY() == y)
    {
        return getYoshi();
    }

    return nullptr;
}


//destroys a square at a position
void StudentWorld::destroySquare(int x, int y)
{
    for(int i = 0; i < m_actors.size(); i++)
    {
        if(m_actors[i]->getX() == x && m_actors[i]->getY() == y && !m_actors[i]->isMovingActor())
        {
            delete m_actors[i];
            m_actors.erase(m_actors.begin() + i);
        }
    }
}

//adds actor to vector of actors
void StudentWorld::addSquare(Actor* actor)
{
    m_actors.push_back(actor);
}


//returns the baddie that was impacted by a vortex
Actor* StudentWorld::getImpact(int x, int y)
{
    for(Actor* actor: m_actors)
    {
        if(actor->allowImpact())
        {
            if(abs(actor->getX() - x) < SPRITE_WIDTH && abs(actor->getY() - y) < SPRITE_HEIGHT)
            {
                return actor;
            }
        }
    }

    return nullptr;
}

//creates an adds a vortex to actors vector
void StudentWorld::createVortex(int x, int y, int direction)
{
    m_actors.push_back(new Vortex(this, IID_VORTEX, x, y, direction));
}

int StudentWorld::init()
{ 
    startCountdownTimer(99);  // this placeholder causes timeout after 5 seconds

    Board bd;
    string board_file = assetPath() + "board0" + to_string(getBoardNumber()) + ".txt";
    Board::LoadResult result = bd.loadBoard(board_file);
    if (result == Board::load_fail_file_not_found)
    cerr << "Could not find board01.txt data file\n";
    else if (result == Board::load_fail_bad_format)
    cerr << "Your board was improperly formatted\n";
    else if (result == Board::load_success) {
    cerr << "Successfully loaded board\n";

    for(int x = 0; x < 16; x++)
    {
        for(int y = 0; y < 16; y++)
        {
            Board::GridEntry ge = bd.getContentsOf(x, y); 
            switch (ge) {
            case Board::empty:
                cout << "Location " << x << "," << y << " is empty\n";
                break;
            case Board::boo:
                cout << "Location " << x << "," << y << " has a Boo and a blue coin square \n";
                if(x < VIEW_WIDTH && y < VIEW_HEIGHT)
                {
                    int x_new = x * BOARD_WIDTH;
                    int y_new = y * BOARD_HEIGHT;
                    m_actors.push_back(new Boo(this, IID_BOO, x_new, y_new));
                    m_actors.push_back(new CoinSquare(this, IID_BLUE_COIN_SQUARE, x_new, y_new, 3));

                }
                break;
            case Board::bowser:
                cout << "Location " << x << "," << y << " has a Bowser and a blue coin square \n";
                if(x < VIEW_WIDTH && y < VIEW_HEIGHT)
                {
                    int x_new = x * BOARD_WIDTH;
                    int y_new = y * BOARD_HEIGHT;
                    m_actors.push_back(new Bowser(this, IID_BOWSER, x_new, y_new));
                    m_actors.push_back(new CoinSquare(this, IID_BLUE_COIN_SQUARE, x_new, y_new, 3));
                }
                break;
            case Board::player:
                cout << "Location " << x << "," << y << " has a player \n";
                if(x < VIEW_WIDTH && y < VIEW_HEIGHT)
                {
                    int x_new = x * BOARD_WIDTH;
                    int y_new = y * BOARD_HEIGHT;
                    m_peach = new Player(this, IID_PEACH, x_new, y_new, 1);
                    m_yoshi = new Player(this, IID_YOSHI, x_new, y_new, 2);
                    m_actors.push_back(m_peach);
                    m_actors.push_back(m_yoshi);
                    m_actors.push_back(new CoinSquare(this, IID_BLUE_COIN_SQUARE, x_new, y_new, 3));

                }
                break;
            case Board::blue_coin_square:
                cout << "Location " << x << "," << y << " has a blue coin square \n";
                if(x < VIEW_WIDTH && y < VIEW_HEIGHT)
                {
                    int x_new = x * BOARD_WIDTH;
                    int y_new = y * BOARD_HEIGHT;   
                    m_actors.push_back(new CoinSquare(this, IID_BLUE_COIN_SQUARE, x_new, y_new, 3));
                }
                break;
            case Board::red_coin_square:
                cout << "Location " << x << "," << y << " has a red coin square \n";
                if(x < VIEW_WIDTH && y < VIEW_HEIGHT)
                {
                    int x_new = x * BOARD_WIDTH;
                    int y_new = y * BOARD_HEIGHT;   
                    m_actors.push_back(new CoinSquare(this, IID_RED_COIN_SQUARE, x_new, y_new, -3));
                }
                break;
            case Board::event_square:
                cout << "Location " << x << "," << y << " has a event square \n";
                if(x < VIEW_WIDTH && y < VIEW_HEIGHT)
                {
                    int x_new = x * BOARD_WIDTH;
                    int y_new = y * BOARD_HEIGHT;   
                    m_actors.push_back(new EventSquare(this, IID_EVENT_SQUARE, x_new, y_new));
                }
                break;
            case Board::bank_square:
                cout << "Location " << x << "," << y << " has a bank square \n";
                if(x < VIEW_WIDTH && y < VIEW_HEIGHT)
                {
                    int x_new = x * BOARD_WIDTH;
                    int y_new = y * BOARD_HEIGHT;   
                    m_actors.push_back(new BankSquare(this, IID_BANK_SQUARE, x_new, y_new));
                }
                break;
            case Board::star_square:
                cout << "Location " << x << "," << y << " has a star square \n";
                if(x < VIEW_WIDTH && y < VIEW_HEIGHT)
                {
                    int x_new = x * BOARD_WIDTH;
                    int y_new = y * BOARD_HEIGHT;   
                    m_actors.push_back(new StarSquare(this, IID_STAR_SQUARE, x_new, y_new));
                }
                break;                
            case Board::up_dir_square:
                cout << "Location " << x << "," << y << " has a up direction square \n";
                if(x < VIEW_WIDTH && y < VIEW_HEIGHT)
                {
                    int x_new = x * BOARD_WIDTH;
                    int y_new = y * BOARD_HEIGHT;   
                    m_actors.push_back(new DirectionSquare(this, IID_DIR_SQUARE, x_new, y_new, 90));
                }
                break;
            case Board::down_dir_square:
                cout << "Location " << x << "," << y << " has a down direction square \n";
                if(x < VIEW_WIDTH && y < VIEW_HEIGHT)
                {
                    int x_new = x * BOARD_WIDTH;
                    int y_new = y * BOARD_HEIGHT;   
                    m_actors.push_back(new DirectionSquare(this, IID_DIR_SQUARE, x_new, y_new, 270));
                }
                break;
            case Board::right_dir_square:
                cout << "Location " << x << "," << y << " has a right direction square \n";
                if(x < VIEW_WIDTH && y < VIEW_HEIGHT)
                {
                    int x_new = x * BOARD_WIDTH;
                    int y_new = y * BOARD_HEIGHT;   
                    m_actors.push_back(new DirectionSquare(this, IID_DIR_SQUARE, x_new, y_new, 0));
                }
                break;   
            case Board::left_dir_square:
                cout << "Location " << x << "," << y << " has a left direction square \n";
                if(x < VIEW_WIDTH && y < VIEW_HEIGHT)
                {
                    int x_new = x * BOARD_WIDTH;
                    int y_new = y * BOARD_HEIGHT;   
                    m_actors.push_back(new DirectionSquare(this, IID_DIR_SQUARE, x_new, y_new, 180));
                }
                break; 
                }
             }
        }
    }

	


    return GWSTATUS_CONTINUE_GAME;

    
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit ESC.
    // Notice that the return value GWSTATUS_NOT_IMPLEMENTED will cause our framework to end the game.

    //runs the do something method for actors, and deletes dead actors
    for(int i = 0; i < m_actors.size(); i++)
    {
        if(m_actors[i]->getAlive())
        {
            m_actors[i]->doSomething();
        }
        else
        {
            delete m_actors[i];
            m_actors.erase(m_actors.begin() + i);
        }
    }   

    //header stats 
    string p1Stats = "P1 Roll: " + to_string(m_peach->getDiceRoll()) + " Stars: " + to_string(m_peach->getStars()) + " $$: " + to_string(m_peach->getCoins());
    m_peach->hasVortex() ? p1Stats += " VOR | " : p1Stats += " | ";

    string p2Stats = "P2 Roll: " + to_string(m_yoshi->getDiceRoll()) + " Stars: " + to_string(m_yoshi->getStars()) + " $$: " + to_string(m_yoshi->getCoins());
    if (m_yoshi->hasVortex()) p2Stats += " VOR";

    setGameStatText(p1Stats + "Time: " + to_string(timeRemaining()) + " | Bank: " + to_string(getBankBalance()) + " | " + p2Stats);
    
    if (timeRemaining() <= 0)
    {
        setFinalScore(m_peach->getStars(), m_peach->getCoins());
		return GWSTATUS_PEACH_WON;
    }
    
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{   
    //deletes actors and clears vector
    for(Actor* i : m_actors)
    {
       delete i;
    } 

    m_actors.clear();
}
