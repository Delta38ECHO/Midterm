#pragma once

#include "game.h"
#include "Screen.h"

#include <algorithm>

struct vec2  //++++++		May be responsible for start post..
{
    float x = 0;
    float y = 0;
};

struct ivec2
{
    int x = 0;
    int y = 0;
};

// A wall with a hole to fly through  //Has no collision elements attributed -> needs some from 
// //isColliding() and screenChar.
struct obstacle
{
    int x = WIDTH;
    int topCount = 4;
    int botCount = 4;
    _time sinceMoved = _clock::now();

    void display(Screen & screen)
    {
        for(int i = 0; i < topCount; i++)
        {
            //mvprintw(i, x, "0");
            screen[i][x] = '0';
        }
        for(int i = 0; i < botCount; i++)
        {
            //mvprintw(HEIGHT - i, x, "0");
            screen[HEIGHT - 1 - i][x] = '0';
        }
    }

    void reset()
    {
        x = WIDTH;

        int holeSize = 4 + (rand() % 7);

        topCount = rand() % (HEIGHT - holeSize);
        botCount = HEIGHT - holeSize - topCount;
    }
};

class FlappyRunner : public Game
{
    unsigned int score = 0;
    // Whether the runner is currently flapping
    bool isFlapping = false;

    // When the runner last flapped its wings
    _time lastFlapped = _clock::now();

    // How long the runner flaps for when space is pressed
    constexpr static double flapDuration = 0.1;

    // How fast the bird is moving horizontally
    constexpr static double forwardSpeed = 0.020;   //Originally 0.016

    // Altitude loss.
    constexpr static float gravity = 0.2f;

    // How much the bird moves up on a flap
    constexpr static float flapForce = -2.0f;   //++++		Could be a sense as to why char moves down.   ->  Success -  inverting gravity makes bird go UP.

    // Mutable data

    // This contains the characters to be drawn to the screen. Using this as a
    // drawing buffer does 2 things: it reduces display flicker when using 
    // ncurses, and it serves as storage for the current map to do collision
    // detection
    Screen screen;

    // Current position of the bird as a float. x doesn't change during gameplay
    // but y does.
    vec2 position;

    // Position rounded to nearest integer for display and collision detection
    // purposes.
    ivec2 roundedPosition;

    // The current obstacle for the game. Represents a vertical wall with a 
    // hole to fly through. Automatically refreshes to a new obstacle after
    // the player has passed through it.
    obstacle wall;


    // Static data and functions

    // The player is drawn as a 4x4 set of characters  ---->  6x8
    constexpr static int displaySize = 4; //Originally 4

    // But for collision detection, we just consider a 2x2 square to make
    // the player feel good when they avoid near-misses. 
    constexpr static int playerSize = 2;  //Originally 2
    // As a result, only the x-marked positions will be checked for collision
    // dddd    
    // dxxd    
    // dxxd
    // dddd
	
	//Revamp:
		/*
			Display Size = rows available
			PlayerSize = spaces in rows to be counted for collision::
			SO-
			Display size = 6x8 with 9 spaces inside.
			-=- Only going to count 4 spaces inside.
			
			//ddddddddd
			//ddxxxxddd
			//ddxxxxddd
			//ddxxxxddd
			//ddxxxxddd
			//ddddddddd
		*/
//+++++		ASCII Art to be updated.		
    const char * flap[displaySize] = {
        "-=.>",
        " \\/v",
        "    ",
        "    "
		
		/*
		"         ",
		"(  _('>  ",
		"(_(//= ) ",  //FAIL -> / & \ are ESC chars and null full bird view.
		"  //-= \\",	//This is so dumb... Must make simpler bird...
		"         ",
		"         "
		*/
		
		/*	
			Del on completion: Concept for ASCII bird:: 6x6 ; Bird is 4x4 so space is 6x6, so collision should be 4x4. 
		
			" (\  }/ " 
			"(  \_('>"  
			"(__(=_) "
			"   -=   "
			  
			"	      "
			"(  _('>  "
		    "(__(/= ) "
		    "  (/-= }\"
			         
		*/
    };
    const char * unflap[displaySize] = {
        "    ",
        " /\\^",
        "-=.>",
        "    "
		
		/*
		"        ",
		"        ",
		"(\\  }//",
		"( \\_('>",
		"(__(=_) ",
		"   -=   "
		*/
    };

    // Copies the bird to the screen
    void displayBird(Screen & screen)
    {
        const auto bird = getBird();

        for(int i = 0; i < 4; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                char c= bird[i][j];
                if (c != ' ')
                {
                    screen[roundedPosition.y + i][roundedPosition.x + j] = c;
                }
            }
        }
    }

    void displayBounds()
    {
        for(int i = 0; i < WIDTH; i++)
        {
            screen[0][i] = '0';
            screen[HEIGHT-1][i] = '0';
        }
    }

    const char * const* getBird() const
    {
        return isFlapping ? flap : unflap;
    }
public:
    // Called once to initialize the game state
    virtual void init()
    {
        Game::init();

        position = {4, HEIGHT - 1};
    }

    // True if collision detection is working
    bool isColliding()
    {
        // ncurses: overwrites part of the status bar
        mvprintw(22, 0, "                     ");
        
        for(int y = 1; y < playerSize; y++)
        {
            for(int x = 1; x < playerSize; x++)
            {
                // See what's on the screen
                char screenChar = screen[roundedPosition.y + y][roundedPosition.x + x];
                if (screenChar != ' ')
                {
                    mvprintw(22, 0, "oh no, colliding");
                    return true;
                }
            }
        }

        return false;
    }

    // Called each frame to update the game state
    virtual void update()
    {
        Game::update();

        _time nowish = _clock::now();

        double secondsSinceFlap = 0;

        // Update bird position
        if (isFlapping)
        {
            secondsSinceFlap = (nowish - lastFlapped).count();
            if (secondsSinceFlap >= flapDuration)
            {
                isFlapping = false;
            }
        }
        else if (spacePressed)
        {
            isFlapping = true;
            lastFlapped = nowish;
            position.y += flapForce;
        }

        position.y += gravity;

        // Update rounded position
        roundedPosition.x = (int)round(position.x);
        roundedPosition.y = (int)round(position.y);

        // Update obstacle
        double sinceMoved = (nowish - wall.sinceMoved).count();
        if (sinceMoved >= forwardSpeed)
        {
            wall.x--;
            wall.sinceMoved = nowish;
            if (wall.x < 0)
            {
                wall.reset();
                score++;
            }
        }

        // Update screen
        screen.clear();

        // Draw bounds and obstacle into screen
        displayBounds();
        wall.display(screen);

        // We want to check for collisions after
        isColliding();

        // Draw bird at position
        displayBird(screen);
        
        // Display game screen on terminal
        for(int y = 0; y < HEIGHT; y++)
        {
            const char * screenRow = &(screen[y][0]);
            mvprintw(y, 0, "%.*s", WIDTH, screenRow);
        }

        // Display some information about the game state
        mvprintw(20, 40, "height: %.2f", (float)HEIGHT - position.y);
        mvprintw(21, 0, "flap cooldown: %.2f", max(0.0, flapDuration - secondsSinceFlap));
        mvprintw(21, 40, "score: %d. elapsed time: %.2f", score, elapsed());
    }
};
