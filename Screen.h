#pragma once

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>

struct ScreenCol
{
    char row[WIDTH];

    char & operator[](int index)
    {
        return row[abs(index) % WIDTH];
    }

    ScreenCol & operator=(const ScreenCol & other)
    {
        if (this != &other)
        {
            memcpy(row, other.row, WIDTH);
        }
        return *this;
    }
};

    // All screen access is made through a struct to control wrap/clamp
    // behavior for out-of-bounds indices
struct Screen
{
    bool flipY = false;

    ScreenCol col[HEIGHT];

    ScreenCol & operator[](int index)
    {
        return col[abs(index) % HEIGHT];
    }

    Screen & operator=(const Screen & other)
    {
        if (this != &other)
        {
            for(int i = 0; i < HEIGHT; i++)
            {
                col[i] = other.col[i];
            }
        }
        return *this;
    }

    operator const char *()
    {
        return &(col[0][0]);
    }

    // Writes the contents of the screen to the display
    void display()
    {
        printf("\x1b[H");
        for(int y = 0; y < HEIGHT; y++)
        {
            int ry = (flipY ? (HEIGHT - 1 - y) : y);
            for(int x = 0; x < WIDTH; x++)
            {
                putchar(col[ry][x]);
            }
            putchar('\n');
        }
    }

    // Puts a blank space in every position on the screen
    void clear(int xstart = 0, int xend = WIDTH, int ystart = 0, 
        int yend = HEIGHT)
    {
        for(int y = ystart; y < yend; y++)
        {
            for(int x = xstart; x < xend; x++)
            {
                col[y][x] = ' ';
            }
        }
    }
};