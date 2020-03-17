#include "flappyrunner.h"

int main()
{
    // ncurses initialization - do not change
    WINDOW * win = initscr();
    noecho();
    curs_set(FALSE);
    cbreak();
    nodelay(win, TRUE);

    // Game only runs once??? HOW TO FIX?!??? 
    FlappyRunner fr;
    fr.run();
    

    // ncurses shutdown - do not change
    endwin();
    return 0;
}

