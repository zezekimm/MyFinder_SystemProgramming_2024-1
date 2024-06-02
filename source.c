#include <stdio.h>
#include <curses.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <signal.h>
#include <sys/ioctl.h>
#include "core.h"
#include "source.h"

#define BLANK "           "

#define ARROW1 27
#define ARROW2 91
#define UP 65
#define DOWN 66
#define RIGHT 67
#define LEFT 68

#define allowSize 9

int p_row, p_col;

char dir[10][20]={
	{" ___      "},
	{"|   \\____ "},
	{"|        |"},
	{"|        |"},
	{"|________|"}
};
char file[10][20]={
	{" ________ "},
	{"|________|"},
	{"|________|"},
	{"|________|"},
	{"|________|"}
};
char blank[10][20]={
	{"          "},
	{"          "},
	{"          "},
	{"          "},
	{"          "}
};
int namelen=8;
char inputstr[500];

void printname(char *dname) {
	char firstline[10], secondline[10];
	//double line
	if(strlen(dname)>namelen && strlen(dname)<=namelen*2){
		strncpy(firstline, dname, namelen);
		firstline[namelen]='\0';
		addstr(firstline);
		addch('\n');
		move(p_row+6, p_col);
		addstr((dname+namelen));
	}
	else if(strlen(dname)>2*namelen) {
		strncpy(firstline, dname, namelen);
		firstline[namelen]='\0';
		addstr(firstline);
		addch('\n');
		move(p_row+6, p_col);
		strncpy(secondline, dname+namelen+1, namelen);
		secondline[namelen]='\0';
		addstr(secondline);
		addstr("...");
	}
	else {
		addstr(dname);
	}
}

void printfile(char *dname, int row, int col) {
	p_row=cal_row(row); p_col=cal_col(col); //cal defined
	move(p_row, p_col); 
	for(int i=0; i<5; i++) {
		addstr(file[i]);
		move(p_row+i+1, p_col);
	}
	addstr(BLANK);
	move(p_row+6, p_col);
	addstr(BLANK);
	move(p_row+5, p_col);
	printname(dname);
	refresh();
}
void printdir(char *dname, int row, int col) {
	p_row=cal_row(row); p_col=cal_col(col); //cal defined
	move(p_row, p_col); 
	for(int i=0; i<5; i++) {
		addstr(dir[i]);
		move(p_row+i+1, p_col);
	}
	addstr(BLANK);
	move(p_row+6, p_col);
	addstr(BLANK);
	move(p_row+5, p_col);
	printname(dname);
	refresh();
}


void set_mode() {
	initscr();
	crmode();
	clear();
	endwin();
	curs_set(0);
	
	struct termios ttystate;
	signal(SIGINT, SIG_IGN);
	tcgetattr(0, &ttystate);
	ttystate.c_lflag &= ~ICANON;
	ttystate.c_lflag &= ~ECHO;
	ttystate.c_cc[VMIN]=1;
	tcsetattr(0, TCSANOW, &ttystate);
}
void reset_mode() {
	struct termios ttystate;
	signal(SIGINT, SIG_DFL);
	tcgetattr(0, &ttystate);
	ttystate.c_lflag |= ICANON;
	ttystate.c_lflag |= ECHO;
	ttystate.c_cc[VMIN]=0;
	tcsetattr(0, TCSANOW, &ttystate);
	curs_set(1);
}
void inputdata() {
	char c=1;
	int len=0;
	int printwhere=6;
	//char inputdata[500];
	while(true) {
		c=getinputnotQuit();
		//printf("%c", c);
		//addch('c);
		if(c==0) continue;
		else if(c==Enter) {
			inputflag=1;
			inputstr[len]='\0';
			break;
		}
		else if(c==ctrlD) {
			inputflag=0;
			return 0;
		}
		else if(c==Backspace) {
			if(printwhere<=6) continue;
			move(4, printwhere);
			addch(' ');
			printwhere--;
			move(4, printwhere);
			len--;
		}
		else {
			if(!(len==0 && c==' ')) {
				printwhere++;
				move(4, printwhere);
				addch(c);			
				inputstr[len]=c;
				len++;
			}
		}
		refresh();
	}
	//printf("%s", inputstr);
}	
char getinput() {
	char c;
	c=getchar();
	if(c=='Q') {
		endwin();
		printf("key 'Q' is pressed...\n");
		EXIT(0);
	}
	if(c=='1' || c=='2' || c=='3' || c=='4') return '0';
	if(c==ARROW1) {
		c=getchar();
		if(c==ARROW2) {
			c=getchar();
			switch(c) {
				case UP :
					c='1';
					break;
				case DOWN :
					c='2';
					break;
				case RIGHT :
					c='3';
					break;
				case LEFT :
					c='4';
					break;
			}
		}
	}
	//addch(arrow);
	//refresh();
	return c;
}
int isallowedword(char c) {
	char *cmpchar={"!@#$%^&*()-=_+ ~`|[]{}?<>,.;:\"\'"};
	int cmpflag=0;
	
	if(c==Enter || c==Backspace || c==ctrlD) return 1;
	
	for(int i=0; i<strlen(cmpchar); i++) {
		if(cmpchar[i]==c) {
			cmpflag=1;
		}
	}
	//printf("%d %d %d\n %d", cmpchar_size, sizeof(char), cmpchar_size, cmpchar_size);
	if(cmpflag)
		return 1;
	else 
		return 0;
}
int getinputnotQuit() {
	char c;
	c=getchar();
	/*if(c=='Q') {
		endwin();
		printf("key 'Q' is pressed...\n");
		EXIT(0);
	}*/
	//if(c=='1' || c=='2' || c=='3' || c=='4') return '0';
	if(c==ARROW1) {
		c=getchar();
		if(c==ARROW2) {
			c=getchar();
			switch(c) {
				case UP :
					c='1';
					break;
				case DOWN :
					c='2';
					break;
				case RIGHT :
					c='3';
					break;
				case LEFT :
					c='4';
					break;
			}
			return 0;
		}
	}
	if(!isallowedword(c) && !isalnum(c)) return 0;
	//addch(arrow);
	//refresh();
	return c;
}
struct winsize wbuf;
int screen_size() {
	if(ioctl(0, TIOCGWINSZ, &wbuf)!=-1) {
		return wbuf.ws_row*1000+wbuf.ws_col;
	}
	printf("Can't calculate screen size\n");
	reset_mode();
	exit(1);
}
int printline() {
	for(int i=0; i<wbuf.ws_col; i++) {
		addch('_');
	}
	addch('\n');
}
int printline_flash(char num, char per) {
	num+='1';
	per+='1';
	for(int i=0; i<wbuf.ws_col-10; i++) {
		addch('_');
	}
	addch('(');
	addch(num);
	addstr(" / ");
	addch(per);
	addch(')');
	for(int i=0; i<3; i++) {
		addch('_');
	}
}
void printcursor() {
	int atts;
	//atts = A_BLINK | A_BOLD;
	atts = A_BOLD;
	attron(atts);
	//attron(
	addstr("<-");
	attroff(atts);
	//bold and blinking
	//printw(BOLD_BLINKING"<-"OFF);
}
void EXIT(int ecode) {
	clear();
	reset_mode();
	//closedir(cur);
	exit(ecode);
}
void printBoldChar(char ch) {
	int atts;
	//atts = A_BOLD | A_UNDERLINE;
	//attron(atts);
	addch(ch);
	//attroff(atts);
}
void printmenustr(char *str) {
	addstr("| ");
	printBoldChar(str[0]);
	addstr(str+1);
}
void printmenuname(char *name) {
	addch('|');
	int atts;
	atts = A_BOLD | A_UNDERLINE;
	attron(atts);
	addch(' ');
	char nametmp[20];
	int longflag=0;
	if(strlen(name)>7) longflag=1;
	strncpy(nametmp, name, 7);
	nametmp[7]='\0';
	if(longflag) {
		nametmp[strlen(nametmp)+3]='\0';
		strncpy(nametmp+7, "...", 3);
	}
	//move(menu_row++, menu_col);
	
	//addstr("            ");
	
	addstr(nametmp);
	for(int i=0; i<14-strlen(nametmp); i++) {
		addch(' ');
	}
	attroff(atts);
	addstr("|");
}
void printmenu(char *name, int menu_row, int menu_col) {
	move(menu_row++, menu_col);
	addstr(" _______________");move(menu_row++, menu_col);
	printmenuname(name);
	//move(menu_row, menu_col+16);
	move(menu_row++, menu_col);  //nameprint
	//      | Copy          |\n
	printmenustr("Cut           |");move(menu_row++, menu_col);
	printmenustr("Copy          |");move(menu_row++, menu_col);
	printmenustr("Rename        |");move(menu_row++, menu_col);
	printmenustr("Permission    |");move(menu_row++, menu_col);
	printmenustr("Delete        |");move(menu_row++, menu_col);
	//printmenustr("Shortcut      |");move(menu_row++, menu_col);
	printmenustr("Property      |");move(menu_row++, menu_col);
	printmenustr("Paste         |");move(menu_row++, menu_col);
	addstr("|_______________|");move(menu_row++, menu_col);
	refresh();
}
void printfilemenu(char *name, int menu_row, int menu_col) {
	move(menu_row++, menu_col);
	addstr(" _______________");move(menu_row++, menu_col);
	printmenuname(name);
	//move(menu_row, menu_col+16);
	move(menu_row++, menu_col);  //nameprint
	//      | Copy          |\n
	printmenustr("Cut           |");move(menu_row++, menu_col);
	printmenustr("Copy          |");move(menu_row++, menu_col);
	printmenustr("Rename        |");move(menu_row++, menu_col);
	printmenustr("Permission    |");move(menu_row++, menu_col);
	printmenustr("Delete        |");move(menu_row++, menu_col);
	//printmenustr("Shortcut      |");move(menu_row++, menu_col);
	printmenustr("Property      |");move(menu_row++, menu_col);
	//printmenustr("Paste         |");move(menu_row++, menu_col);
	addstr("|_______________|");move(menu_row++, menu_col);
	refresh();
}
void printmain() {
	move(1, 5);
	addstr("Current directory : ");
	addstr(cur);
	addstr("\n\n");
	move(2,5);
	addstr("\n     \n    \n");
	move(5, 0);
	printline();
}

void printmessagebox(char *line1, char *line2) {
	char tmpline1[500], tmpline2[500];
	char curline1[1000]="\0", curline2[1000]="\0";
	set_screen_size();
	strcpy(tmpline1, line1);
	strcpy(tmpline2, line2);
	int path_print_size=wbuf.ws_col-27;
	
	move(1, 5);
	addstr("Current directory : ");
	if(strlen(cur)>path_print_size && strlen(cur)<=path_print_size*2){
		strncpy(curline1, cur, path_print_size);
		curline1[path_print_size+1]='\0';
		addstr(curline1);
		//addch('\n');
		move(2, 25);
		addstr((cur+path_print_size));
	}
	else if(strlen(cur)>2*path_print_size) {
		strncpy(curline1, cur+strlen(cur)-1-path_print_size-path_print_size, path_print_size);
		curline1[0]='.'; curline1[1]='.'; curline1[2]='.';
		curline1[path_print_size+1]='\0';
		addstr(curline1);
		//addch('\n');
		move(2, 25);
		strncpy(curline2, cur+strlen(cur)-1-path_print_size, path_print_size);
		curline2[path_print_size+1]='\0';
		addstr(curline2);
	}
	else {
		addstr(cur);
	}
	
	
	addstr("\n\n");
	move(3, 5);
	//addstr("\n     ");
	if(strlen(tmpline1)>screen_col-allowSize) {
		strcpy(tmpline1+screen_col-allowSize, "..."); 
	}
	addstr(tmpline1);
	addstr("\n     ");
	
	if(strlen(tmpline2)>screen_col-allowSize) {
		strcpy(tmpline2+screen_col-allowSize, "..."); 
	}
	addstr(tmpline2);
	
	addstr("\n");
	move(5, 0);
	printline();
	refresh();
}

void printmessagebox_flash(char *line1, char *line2, char num, char per) {
	char tmpline1[500], tmpline2[500];
	set_screen_size();
	strcpy(tmpline1, line1);
	strcpy(tmpline2, line2);
	char curline1[1000]="\0", curline2[1000]="\0";
	set_screen_size();
	int path_print_size=wbuf.ws_col-27;
	
	move(1, 5);
	addstr("Current directory : ");
	if(strlen(cur)>path_print_size && strlen(cur)<=path_print_size*2){
		strncpy(curline1, cur, path_print_size);
		curline1[path_print_size+1]='\0'; 
		addstr(curline1);
		//addch('\n');
		move(2, 25);
		addstr((cur+path_print_size));
	}
	else if(strlen(cur)>2*path_print_size) {
		strncpy(curline1, cur+strlen(cur)-1-path_print_size-path_print_size, path_print_size);
		curline1[0]='.'; curline1[1]='.'; curline1[2]='.';
		curline1[path_print_size+1]='\0';
		addstr(curline1);
		//addch('\n');
		move(2, 25);
		strncpy(curline2, cur+strlen(cur)-1-path_print_size, path_print_size);
		curline2[path_print_size+1]='\0';
		addstr(curline2);
	}
	else {
		addstr(cur);
	}
	
	addstr("\n\n");
	move(3, 5);
	if(strlen(tmpline1)>screen_col-allowSize) {
		strcpy(tmpline1+screen_col-allowSize, "..."); 
	}
	addstr(tmpline1);
	addstr("\n     ");
	
	if(strlen(tmpline2)>screen_col-allowSize) {
		strcpy(tmpline2+screen_col-allowSize, "..."); 
	}
	addstr(tmpline2);
	
	addstr("\n");
	move(5, 0);
	printline_flash(num, per);
	refresh();
}

int set_ticker(int n_msecs) {
	struct itimerval new_timeset;
	long n_sec, n_usecs;
	n_sec=n_msecs/1000;
	n_usecs=(n_msecs%1000)*1000L;
	
	new_timeset.it_interval.tv_sec=n_sec;
	new_timeset.it_interval.tv_usec=n_usecs;
	new_timeset.it_value.tv_sec=n_sec;
	new_timeset.it_value.tv_usec=n_usecs;
	return setitimer(ITIMER_REAL, &new_timeset, NULL);
}
int set_ticker_flash(int n_msecs) {
	struct itimerval new_timeset;
	long n_sec, n_usecs;
	n_sec=n_msecs/1000;
	n_usecs=(n_msecs%1000)*1000L;
	
	new_timeset.it_interval.tv_sec=200;
	new_timeset.it_interval.tv_usec=2000;
	new_timeset.it_value.tv_sec=n_sec;
	new_timeset.it_value.tv_usec=n_usecs;
	return setitimer(ITIMER_REAL, &new_timeset, NULL);
}
static int flash_max=2; // 0~2,, 
int flash_num;
void foldermenu_flash(int signum) {
	flash_num++;
	if(flash_num>flash_max) flash_num=0;
	
	switch(flash_num) {
		case 0:
			printmessagebox_flash("Press \'p\' to paste the selected file", "Press \'n\' to create a new directory", flash_num, flash_max);
			break;
		case 1:
			printmessagebox_flash("Press \'s\' to choose sort type", "Press \'t\' to open the properties of current directory", flash_num, flash_max);
			break;
		case 2:
			if(show_hidden) printmessagebox_flash("Press \'h\' to hide hidden file", "", flash_num, flash_max);
			else printmessagebox_flash("Press \'h\' to show hidden file", "", flash_num, flash_max);
			break;	
	}
}

void flash_message(int signum) {
	set_ticker(0);
	strcpy(mainline1, staticmainline1);
	strcpy(mainline2, staticmainline2);
	printmessagebox(mainline1, mainline2);
	flashing=0;
	signal(SIGALRM, SIG_DFL);
}
void print_message_flash(int flash_time, char* message1, char* message2) {
	set_ticker(flash_time);
	printmessagebox(message1, message2);
	strcpy(mainline1, message1);
	strcpy(mainline2, message2);
	flashing=1;
	signal(SIGALRM, flash_message);
	
}
