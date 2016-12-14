#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <curses.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>

#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80

#define PAUSE 112
#define ESC 27

#define default_ch 'X'
#define HEAD '@'
#define BLANK ' '
#include "bounce.h"

void bound(),set_cr_noecho_mode(), wrap_up(), set_up(),move_snake(int);
int set_ticker(int);
int bounce_or_lose(struct ppball*);
void title();

static int dir =LEFT;
static int length=5;
int cur_x;
int cur_y;
int moved = 0;
int score = 0;
struct ppball snake[100];
int x,y;
int speed = 100;
char food;
int main(){
	int ch;
	title();
	bound();
	fflush(stdin);	
	set_up();
	
//	signal(SIGALRM, move_snake);
//	set_ticker(200);

	while((ch = getchar()) != 'Q'){
	
	
		if(  (dir == LEFT && ch !='f') || (dir == RIGHT && ch != 's') || (dir == DOWN && ch != 'e') || (dir == UP && ch != 'd') )
		{
			if(ch=='s')
				dir=LEFT;
			if(ch=='f')
				dir=RIGHT;
			if(ch=='e')
				dir=UP;
			if(ch=='d')
				dir=DOWN;
		}
		moved=1;
			//ch = 0;
	
	
	}
	wrap_up();

	return 0;
}

void title(){
	bound();

	set_cr_noecho_mode();
	mvaddstr(11, 31, "THE SNAKE GAME");
	mvaddstr(12, 30,"PRESS ANY BUTTON");
	mvaddstr(15, 33,"TEAM:10");	
	move(BOT_ROW+1, 0);
	refresh();

	getch();
	endwin();	
}

void make_food()
{
	int i;
	while(1)
	{
		srand((unsigned)time(NULL));
		x=rand() % (MAP_X-2) + 3;
		y=rand()% (MAP_Y-2) + 3;
		food=rand()% 26 + 97;
	
		for(i=0;i<length ;i++)	
			if(x == snake[i].x_pos && y == snake[i].y_pos)
				break;
		if(i==length)
		{
			mvprintw(y,x,"%c",food);
			return;
		}

	}

}


void game_over()
{
	initscr();
	move(11,31);
	addstr("***game over***");
	mvaddstr(12, 32, "score: ");
	move(12, 37);
	mvprintw(12,41,"%d", score);
	move(BOT_ROW+1, 0);
	refresh();
	sleep(3);
	endwin();
	exit(0);
}

void plus_score()
{
	score += food-96;
	mvprintw(MAP_YINIT-1,MAP_XINIT+MAP_X-6,"%d",score);
}

void move_snake(int signum){
	
	int i;
//	signal(SIGALRM, SIG_IGN);
	
	if(snake[0].x_pos == x && snake[0].y_pos == y)
        {
                make_food();
		refresh();
                ++length;
		plus_score();
		
		if(food == 's'||food == 'h' || food == 'c' || food == 'j' || food =='i')
			speed+=6;
		else
			speed-=2;
		
		set_ticker(speed);
                snake[length-1].x_pos = snake[length-2].x_pos;
                snake[length-1].y_pos = snake[length-2].y_pos;
        }
	
	for(i=length-1;i>0; i--)
              if(snake[0].x_pos == snake[i].x_pos && snake[0].y_pos == snake[i].y_pos)
                      game_over();
	

	mvaddch(snake[length-1].y_pos, snake[length-1].x_pos, BLANK);
	for(i = length-1; i>0; i--){
		snake[i].x_pos = snake[i-1].x_pos;
		snake[i].y_pos = snake[i-1].y_pos;
	}
	if(moved){
		mvaddch(snake[0].y_pos, snake[0].x_pos, DFL_SYMBOL);
		if(dir == LEFT) snake[0].x_pos--;
		if(dir == RIGHT) snake[0].x_pos++;
		if(dir == UP) snake[0].y_pos--;
		if(dir == DOWN) snake[0].y_pos++;
	
	  	mvaddch(snake[0].y_pos, snake[0].x_pos, HEAD);	
		if(snake[0].x_pos == 2 || snake[0].x_pos == 2+MAP_X || snake[0].y_pos == 2 || snake[0].y_pos == 2+MAP_Y)
			game_over();
		refresh();
	}
	
//	signal(SIGALRM, move_snake);
}

void bound(){
	int i, j;
	initscr();
	clear();
	set_cr_noecho_mode();
	for(i = 0; i < MAP_Y; i++){
		mvaddch(MAP_YINIT + i, MAP_XINIT, default_ch);
		mvaddch(MAP_YINIT + i, MAP_XINIT+MAP_X, default_ch);
	}

	
	for(j = 0; j <= MAP_X; j++){
		mvaddch(MAP_YINIT, MAP_XINIT+j, default_ch);
		mvaddch(MAP_YINIT + MAP_Y, MAP_XINIT+j, default_ch);
	}
	
	move(MAP_YINIT -1, MAP_XINIT+MAP_X-15);
	addstr("score: ");
	move(MAP_YINIT -1, MAP_XINIT+MAP_X-6);
	addstr("0");
	move(MAP_YINIT + MAP_Y+1, MAP_XINIT + MAP_X+1);
	refresh();

}

void set_up(){
	int i;
 	void move_snake(int);

	for(i = 0; i < length; i++){
	snake[i].x_pos = X_INIT+i;
	snake[i].y_pos = Y_INIT;
		if(i == 0) snake[i].symbol = HEAD;
		else snake[i].symbol = DFL_SYMBOL;
	snake[i].dir = LEFT;
	
	}
	
	initscr();

        set_cr_noecho_mode();

        signal(SIGINT, SIG_IGN);
        make_food();
	for(i = 0; i < length; i++){
		mvaddch(snake[i].y_pos, snake[i].x_pos, snake[i].symbol);
	}

    	 refresh();
	set_ticker(speed);
	moved = 1;
       	signal(SIGALRM, move_snake);
	
}

void wrap_up(){
        set_ticker(0);
        endwin();
}

void set_cr_noecho_mode(){

        struct termios ttystate;

        tcgetattr(0, &ttystate);
        ttystate.c_lflag &= ~ICANON;
        ttystate.c_lflag &= ~ECHO;
        ttystate.c_cc[VMIN] = 1;
        tcsetattr(0, TCSANOW, &ttystate);

}


