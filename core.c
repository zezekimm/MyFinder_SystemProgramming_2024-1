#include <stdio.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <curses.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <termios.h>
#include <sys/stat.h>
#include "core.h"
#define cursor_st_row 10
#define cursor_st_col 15

#define min_screen_row 15
#define min_screen_col 30

#define tick_time 2000

int ADMIN=0; 

char cur[1000]="/"; //  /home/username/...
char filepath[1000];
//char flash_message1[500], flash_message2[500];
char mainline1[50]={"Press \"Space bar\" to go to the selected directory."};
char mainline2[50]={"Press \"Backspace\" to go to the upper directory."};

int row=1, col=0;
struct dirent *dir_arr[1000], *dir_me, *dir_papa;
int screen_size_data, screen_row, screen_col;
int before_screen_row, before_screen_col, view_col, view_row;
int cursor_num=0; // for cursor control
int cursor_cnt_row, cursor_cnt_col;
int wheel_row=0, filecnt; // 0<=wheel_row<= filecnt/view_col
int file_row; // file_row=filecnt/view_col
int para_col, para_row, menu_cursor_row, menu_cursor_col;
int noFile, flashing; //flag 
int cursor_now_row, cursor_now_col;
void diropen();
long location=0L;
int change_screen=0;
int add_cursor=0;
int sortType=1, show_hidden=0;

int main(int argc, char *argv[]) {
	set_screen_size();
	//view_row=(screen_row);
	set_mode(); //SIGINT ignore, ~ECHO, ~ICANON
	struct dirent *direntp;
	char hostname[256];
	
	while(screen_col < min_screen_col || screen_row < min_screen_row) {
		clear();
		addstr("Screen is too small!!!!\n");
		refresh();  
		set_screen_size();
	}
	
	//gethostname(hostname, 256);
	//sprintf(cur, "%s/%s", cur, hostname);
	getcwd(cur, sizeof(cur));
	
	dir_explore(); // 1~filecnt
	
	diropen();
	
	chdir(cur);
	control_cursor(0);
	char c;
	if(argc==2) {
		if(!strcmp(argv[1], "admin")) {
			ADMIN=1;
		}
	}
	while(true) {
		set_screen_size();
		if(screen_col < min_screen_col || screen_row < min_screen_row) {
			clear();
			addstr("Screen is too small!!!!\n");
			refresh();
			set_screen_size();
			continue;
		}
		c=getinput(); // 1 up 2 down 3 right 4 left
		if(c>='1' && c<='4') {
			clear();
			control_cursor(c);
		}
		else if(c==' ') {
			//printf("dive in");
			if(dir_arr[cursor_num]->d_type==4) {
				//closedir(cur);
				chdir(dir_arr[cursor_num]->d_name);
				getcwd(cur, sizeof(cur));
				dir_explore();
				control_cursor(0);
			}
			else {
				reset_mode();
				openvi();
				set_mode();
				control_cursor(1);
			}
		}
		else if(c==Backspace) {
			//printf("go back");
			chdir(dir_papa->d_name);
			getcwd(cur, sizeof(cur));
			dir_explore();
			control_cursor(0);
		}
		else if(c=='x') {
			//cursor_now_row, cursor_now_col
			//move(cursor_now_row, cursor_now_col);
			para_row=cursor_now_row; para_col=cursor_now_col;
			if(cursor_now_col+16>=screen_col) {
				para_col-=16;
			}
			if(cursor_now_row+9>=screen_row) {
				para_row-=9;
			}
			move(cursor_now_row, cursor_now_col);
			addstr("  "); //delete cursor
			
			if(dir_arr[cursor_num]->d_type==4) {
				printmenu(dir_arr[cursor_num]->d_name, para_row, para_col);
				intomenu(para_row, para_col);
			}
			else {
				printfilemenu(dir_arr[cursor_num]->d_name, para_row, para_col);
				intofilemenu(para_row, para_col);
			}
			control_cursor(1);
		}
		else if(c=='v') { // foldermenu
			//char *line1 = "Press \'p\' to paste the selected file";
			//char *line2 = "Press \' \' to ...";
			printmessagebox_flash("Press \'p\' to paste the selected file", "Press \'n\' to create a new directory", 0, 2);
			set_ticker(tick_time);
			signal(SIGALRM, foldermenu_flash);
			intofoldermenu();
			set_ticker(0);
			signal(SIGALRM, SIG_DFL);
			dir_explore();
			diropen();
			control_cursor(1);
		} 
		set_screen_size();
		if(change_screen) {
			dir_explore();
			clear();
			diropen();
			printmessagebox(mainline1, mainline2);
			control_cursor(0);
			// function
		}
		diropen();
		printmessagebox(mainline1, mainline2);
	}
	endwin();
}

void openvi() {
	int pid, tmp;
	int child_info=-1;
	makefilepath();
	if((pid=fork()) == -1)
		perror("fork");
	else if(pid==0) {
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		execl("/bin/vi", "vi", filepath, NULL);
		perror("cannot execute command");
		EXIT(1);
	}
	else {
		if((tmp=wait(&child_info))==-1)
			perror("wait");
	}
	reset_mode();
	set_mode();
}

void dir_explore() {
	DIR *dir_ptr;
	struct dirent *direntp;
	filecnt=1; cursor_num=1;
	col=0; row=1; cursor_cnt_row=0; cursor_cnt_col=0;
	noFile=1;
	wheel_row=0;
	if((dir_ptr=opendir(cur))==NULL) {
		fprintf(stderr, "%s : Can't open directory!\n", cur);
		EXIT(1);
	}
	else {
		chdir(cur);
		filecnt=1;
		while((direntp=readdir(dir_ptr))!=NULL) {
			if(strcmp(direntp->d_name, "..")==0) {
				dir_papa=direntp; 
				continue;
			}
			else if(strcmp(direntp->d_name, ".") ==0) {
				 dir_me=direntp;
				 continue;
			}
			else if(!show_hidden && direntp->d_name[0]=='.') 
			{
				continue;
			}
			dir_arr[filecnt]=direntp;
			filecnt++;
			noFile=0;
		}
	}
	filecnt--;
	dirSort(sortType);
	file_row=(filecnt)/view_col;
	clear();
}


void diropen() {
	DIR *dir_ptr;
	struct dirent *direntp;
	row=1; col=0;
	char ctmp;
	if(!flashing) {
		printmessagebox(mainline1, mainline2);
	}
	else {
		flashing=0;
	}
	if(noFile) {
		clear();
		printmessagebox(mainline1, mainline2);
		move(cursor_now_row, cursor_now_col);
		addstr("No File\n");
		move(cursor_now_row+1, cursor_now_col-4);
		addstr("Press \"backspace\"");
		refresh();
		while(1) {
			set_screen_size();
			if(change_screen) {
				//dir_explore();
				clear();
				move(cursor_now_row, cursor_now_col);
				addstr("No File\n");
				move(cursor_now_row+1, cursor_now_col-4);
				addstr("Press \"backspace\"");
				printmessagebox(mainline1, mainline2);
				//control_cursor(0);
			// function
			}
			ctmp=getinput();
			if(ctmp==Backspace) {
				//printf("go back");
				chdir(dir_papa->d_name);
				getcwd(cur, sizeof(cur));
				dir_explore();
				diropen();
				control_cursor(0);
				return ;
			}
			else if(ctmp=='v') { // foldermenu
				printmessagebox("Press \'p\' to copy the selected file", "Press \'n\' to create a new directory");
				intofoldermenu();
				dir_explore();
				diropen();
				control_cursor(1);
				if(!noFile) return ;
			} 
			
		}
	}
	for(int i=wheel_row*view_col+1; i<=filecnt; i++) {
		direntp=dir_arr[i];
		col++;
		if(col>view_col) {
			row++; col=1;
		}
		if(row>view_row) continue;
		
		if(direntp->d_type == 4) {
			printdir(direntp->d_name, row, col);
		}
		else {
			printfile(direntp->d_name, row, col);
		}
	}
	refresh();
}


void control_cursor(char c) { // 1 up 2 down 3 right 4 left
	cursor_now_row=cursor_st_row+(cursor_cnt_row*8);
	cursor_now_col=cursor_st_col+(cursor_cnt_col*15);
	
	move(cursor_now_row, cursor_now_col);
	char ctmp;
	
	addstr("  "); //delete cursor
	if(c==0) { //setting cusor
		move(cursor_now_row, cursor_now_col);
		printcursor();
		refresh();
		return ;
	}
	if(noFile) {
		move(cursor_now_row, cursor_now_col);
		addstr("No File\n");
		move(cursor_now_row+1, cursor_now_col-4);
		addstr("Press \"backspace\"");
		refresh();
		while(1) {
			ctmp=getchar();
			if(ctmp==Backspace) return ;
		}
	}
	if(c=='1') { //up
		if(cursor_cnt_row>0) {
			cursor_cnt_row--;
			cursor_num-=view_col;
		}
		else {
			if(wheel_row>0) {
				wheel_row--;
				cursor_num-=view_col;
			}
		}
	}
	else if(c=='2') { //down
		if(cursor_cnt_row<view_row-1) { //!end line?
				 if(cursor_num+view_col <= filecnt) {
					cursor_cnt_row++;
					cursor_num+=view_col;
					
				}
				else {
					cursor_cnt_row++;
					cursor_num-=cursor_cnt_col;
					cursor_cnt_col=(filecnt-1)%view_col;
					cursor_num+=view_col+cursor_cnt_col;	
				}
		}
		else { //end line?
			if(wheel_row<=file_row-view_row) { //wheel
				if(cursor_num+view_col<=filecnt) {
					wheel_row++;
					cursor_num+=view_col;
					
				}
				else if(cursor_num+view_col>filecnt && filecnt%view_col!=0){ //can't scroll //gara complete
					
					wheel_row++;
					cursor_num-=cursor_cnt_col;
					cursor_cnt_col=(filecnt-1)%view_col;
					cursor_num+=view_col+cursor_cnt_col;
				}
			}
			
		}
	}
	else if(c=='3') { // right
		if(cursor_num!=filecnt) {
		if(cursor_cnt_col<view_col-1) { // !end position??
			if(cursor_num<filecnt) {
				cursor_num++;
				cursor_cnt_col++;
			}
		}
		else { //end position
			if(cursor_cnt_row<view_row-1) {
				cursor_cnt_row++;
				cursor_cnt_col=0;
				cursor_num++;
			}
			else if(wheel_row<=file_row-view_row) {
				if(cursor_num<filecnt) {
					cursor_cnt_col=0;
					wheel_row++;
					cursor_num++;
				}
			}
		}
		}
	}
	else if(c=='4') { // left
		if(cursor_num!=1) {
			if(cursor_cnt_col>0) {
				cursor_cnt_col--;
				cursor_num--;
			}
			else {
				if(cursor_cnt_row>0) {
					cursor_cnt_row--;
					cursor_cnt_col=view_col-1;
					cursor_num--;
				}
				else if(wheel_row>0) {
					//cursor_cnt_row++;
					cursor_cnt_col=view_col-1;
					wheel_row--;
					cursor_num--;
				}
			}
		}
	}
	if(cursor_num>filecnt) {
		cursor_num=filecnt;
		cursor_cnt_col=(filecnt-1)%(view_col);
		if(filecnt%view_col==0) {
			cursor_cnt_row=filecnt/view_col-1;
		}
		else cursor_cnt_row=filecnt/view_col;
	}
	cursor_now_row=cursor_st_row+(cursor_cnt_row*8);
	cursor_now_col=cursor_st_col+(cursor_cnt_col*15);
	move(cursor_now_row, cursor_now_col);
	printcursor();
	refresh();
}


void set_screen_size() {
	change_screen=0;
	screen_size_data=screen_size();
	before_screen_col=screen_col;
	before_screen_row=screen_row;
	screen_col=screen_size_data%1000;
	screen_row=screen_size_data/1000; //get screen size
	//if(screen_row<11 && screen_col<20)
	//printf("s_row : %d, s_col : %d", screen_row, screen_col);
	view_col=(screen_col-5)/15;
	view_row=(screen_row-15)/8+1;
	
	if(before_screen_col!=screen_col || before_screen_row!=screen_row) 
		change_screen=1;
	
	return ; 
}

