#pragma once

#include <cstring>
#include <stdio.h>
#include <chrono>
#include <cstdlib>
#include <ncurses/ncurses.h>
#include <unistd.h>
#include <cmath>
using _clock            = std::chrono::steady_clock;
using _elapsed          = std::chrono::duration<double>;
using _time             = std::chrono::time_point<_clock, _elapsed>;

using namespace std;

#define WIDTH  80
#define HEIGHT 20

class Game
{
public:
    // Set when the game begins
    _time started = _clock::now();
    // Set when update() is called
    _time lastUpdated = _clock::now();

    double elapsed() { return (lastUpdated - started).count(); }

    bool spacePressed = false;

    // You can set this to false in update() to make the scene end.
    bool shouldContinue = true;

    // Keeps track of how many times update() has been called.
    unsigned long frame = 0;

    // Frames per second: how many times the scene should update per second.
    // Lower values means fewer updates per second, higher means more.
    // You can set this variable in init( to change the runtime speed.
    double FPS = 24.0;

    // FPS implicitly defines seconds per frame as the inverse
    double SPF() { return 1.0 / FPS; }

    // Makes the program sleep by the amount of seconds passed in.
    // This gets called in update() to make the scene display more smoothly.
    void sleepFor(double seconds)
    {
        // Convert seconds to microseconds for usleep
        unsigned int ns = (unsigned int)(1000000.0 * seconds);
        usleep(ns);
    }

    // Sets up the scene before it runs
    virtual void init()
    {
        shouldContinue = true;
        started = _clock::now();
    }

    // Called to update the scene by a fixed amount of time. 
    virtual void update()
    {
        frame++;

        int key = ERR;
        bool spacePressedOnThisFrame = false;
        if ((key = getch()) != ERR)
        {
            spacePressedOnThisFrame |= key == ' ';
        }
        spacePressed = spacePressedOnThisFrame;

        mvprintw(20, 0, "space pressed: %s", spacePressed ? "true " : "false");

        lastUpdated = _clock::now();
    }

    void end()
    {
        shouldContinue = false;
    }

    // The function that sets up and runs the scene. Very concise, but powerful.
    void run()
    {
        init();                     // initialize the scene
        while (shouldContinue)      // start the scene in a loop
        {
            update();               // call update once per loop
            refresh();
            sleepFor(SPF());        // then sleep the thread
        }

        sleepFor(2.0);
    }
};
