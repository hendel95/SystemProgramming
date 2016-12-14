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
int x_ob,y_ob;
int MAX_SCORE;
typedef struct obstacle
{
	int x,y;
}obstacle;

int obs_num;

obstacle location[100];

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

void reset()
{
	int i;
	dir =LEFT;
	length=5;
	moved = 0;
	score = 0;
	length = 5;
	obs_num = 0;
	for(i=0;i<100;i++)
	{
		location[i].x=0;
		location[i].y=0;
	}
	system("cls");
	title();
	bound();
}
void title(){
	bound();

	set_cr_noecho_mode();
	mvaddstr(7, 30, "<<THE SNAKE GAME>>");
	mvaddstr(8, 31,"PRESS ANY BUTTON");
	mvaddstr(11, 30,"up : press 'E' button");
	mvaddstr(12, 30,"down : prees 'D' button");
	mvaddstr(13, 30,"left : press 'S' button");
	mvaddstr(14, 30,"right : press 'F' button");
	mvaddstr(15, 30,"slow : eat s,j,c,h,i");
	mvaddstr(16, 30,"obstacle : X");
	mvaddstr(17, 30,"quit : press 'Q'");
	mvaddstr(18, 30,"score : a = 1, b = 2,...,z = 26");
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
		
		if(food == 'x')
			continue;
			
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

void make_obstacle()
{
	int i;
	while(1)
	{
                //srand(100);
		x_ob=rand() % (MAP_X-2) + 3;
                y_ob=rand()% (MAP_Y-2) + 3;
		
                for(i=0;i<length ;i++){
			if((x_ob == snake[i].x_pos && y_ob == snake[i].y_pos) ||( x == x_ob && y == y_ob))
			{
				i=0;  
			      break;
			}
			
		}
                if(i==length)
                {
			location[obs_num].x = x_ob;
			location[obs_num].y = y_ob;
			obs_num++;
                        mvprintw(y_ob,x_ob,"%c",default_ch);
                        return;
		}
        }

}

void game_over()
{
	if(score > MAX_SCORE)
		MAX_SCORE = score;
	initscr();
	move(11,31);
	addstr("***game over***");
	mvaddstr(12, 32, "Your score: ");
	mvaddstr(13, 32, "High score : ");
	mvaddstr(14, 26,"Restart : wait 5 seconds");
	move(12, 37);
	mvprintw(12,47,"%d", score);
	mvprintw(13,47,"%d", MAX_SCORE);
	move(BOT_ROW+1, 0);
	refresh();
	sleep(5);
	endwin();
	reset();
	set_up();
}

void plus_score()
{
	score += food-96;
	mvprintw(MAP_YINIT-1,MAP_XINIT+MAP_X-6,"%d",score);
}

void move_snake(int signum){
	
	int i;
	
	if(snake[0].x_pos == x && snake[0].y_pos == y)
        {
                make_food();
		make_obstacle();
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
	
	for(i=0;i<obs_num;i++)
		if(location[i].x == snake[0].x_pos && location[i].y == snake[0].y_pos)
			game_over();

	if((snake[0].x_pos == x_ob && snake[0].y_pos == y_ob))
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
		if(snake[0].x_pos == 2 || snake[0].x_pos == 2+MAP_X || snake[0].y_pos == 2 || snake[0].y_pos == 2+MAP_Y )
			game_over();
		refresh();
	}

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
	mvprintw(MAP_YINIT -1 , MAP_XINIT,"HIGH SCORE : %d",MAX_SCORE);
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


