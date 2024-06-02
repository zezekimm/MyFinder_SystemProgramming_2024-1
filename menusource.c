#include <stdio.h>
#include <curses.h>
#include <sys/stat.h>
#include "source.h"

char intString[30];
int iSsize=0;
int recodeMode(int mode_arr[]) {
	int tmpmode=0; //S_IRUSR
	if(mode_arr[1]==1) tmpmode |= S_IRUSR;
	if(mode_arr[2]==1) tmpmode |= S_IWUSR;
	if(mode_arr[3]==1) tmpmode |= S_IXUSR;
	if(mode_arr[4]==1) tmpmode |= S_IRGRP;
	if(mode_arr[5]==1) tmpmode |= S_IWGRP;
	if(mode_arr[6]==1) tmpmode |= S_IXGRP;
	if(mode_arr[7]==1) tmpmode |= S_IROTH;
	if(mode_arr[8]==1) tmpmode |= S_IWOTH;
	if(mode_arr[9]==1) tmpmode |= S_IXOTH;
	return tmpmode;
}
void cursor_permission(int permission_row, int permission_col, int permission_cursor) {
	switch(permission_cursor) {
		      //addstr("|    R W X      R W X      R W X     |");
		case 1:      
			addstr("|    ^                               |"); break;
		case 2:
			addstr("|      ^                             |"); break;
		case 3:
			addstr("|        ^                           |"); break;
		case 4:
			addstr("|               ^                    |"); break;
		case 5:
			addstr("|                 ^                  |"); break;
		case 6:
			addstr("|                   ^                |"); break;
		case 7:
			addstr("|                          ^         |"); break;
		case 8:
			addstr("|                            ^       |"); break;
		case 9:
			addstr("|                              ^     |"); break;
	}
}
void print_permission(int permission_row, int permission_col, int mode_arr[], int permission_cursor) {
	move(permission_row, permission_col);
	addstr("                                      "); move(permission_row++, permission_col);
	addstr(" ____________________________________ "); 
	move(permission_row++, permission_col);
	addstr("|                                    |"); move(permission_row++, permission_col);
	addstr("|            <Permission>            |"); move(permission_row++, permission_col);
	addstr("|    <User>    <Group>    <Other>    |"); move(permission_row++, permission_col);
	addstr("|    R W X      R W X      R W X     |"); move(permission_row++, permission_col);
	addstr("|    ");
/*ox*/	for(int i=1; i<=9; i++) {
		char prtmp;
		if(mode_arr[i]==1) prtmp='o';
		else prtmp='x';
		addch(prtmp);
		addch(' ');
		if(i==3 || i==6) addstr("     ");
		if(i==9) {
			addstr("    |");
			move(permission_row++, permission_col);
		}
	}
	//addstr("|                                    |"); 
	//move(permission_row++, permission_col);
	cursor_permission(permission_row+5, permission_col, permission_cursor);
	move(permission_row++, permission_col); 
/*^*/	addstr("|____________________________________|"); move(permission_row++, permission_col);
	refresh();
	//addstr("                                      ");
}
void decodeMode(int tmpmode, int mode_arr[]) {
	//usr 1-3
	if(tmpmode & S_IRUSR) mode_arr[1]=1;
	else mode_arr[1]=-1;
	if(tmpmode & S_IWUSR) mode_arr[2]=1;
	else mode_arr[2]=-1;
	if(tmpmode & S_IXUSR) mode_arr[3]=1;
	else mode_arr[3]=-1;
	//grp 4-6
	if(tmpmode & S_IRGRP) mode_arr[4]=1;
	else mode_arr[4]=-1;
	if(tmpmode & S_IWGRP) mode_arr[5]=1;
	else mode_arr[5]=-1;
	if(tmpmode & S_IXGRP) mode_arr[6]=1;
	else mode_arr[6]=-1;
	//oth 7-9
	if(tmpmode & S_IROTH) mode_arr[7]=1;
	else mode_arr[7]=-1;
	if(tmpmode & S_IWOTH) mode_arr[8]=1;
	else mode_arr[8]=-1;
	if(tmpmode & S_IXOTH) mode_arr[9]=1;
	else mode_arr[9]=-1;
}


//_________________________________________________
//property source

void printfile_property(int row, int col) {
	//p_row=cal_row(row); p_col=cal_col(col); //cal defined
	move(row, col); 
	for(int i=0; i<5; i++) {
		addstr(file[i]);
		move(row+i+1, col);
	}
	
}
void printdir_property(int row, int col) {
	//p_row=cal_row(row); p_col=cal_col(col); //cal defined
	move(row, col); 
	for(int i=0; i<5; i++) {
		addstr(dir[i]);
		move(row+i+1, col);
	}
}

void printstr_property(char *printstr, int row, int col) {
	char firstline[50]={0}, secondline[50]={0};
	int slen=25; // max 50 ...
	//double line
	move(row, col);
	if(strlen(printstr)>slen && strlen(printstr)<=slen*2){
		strncpy(firstline, printstr, slen);
		firstline[slen]='\0';
		addstr(firstline);
		move(row+1, col);
		addstr((printstr+slen));
	}
	else if(strlen(printstr)>2*slen) {
		strncpy(firstline, printstr, slen);
		firstline[slen]='\0';
		addstr(firstline);
		move(row+1, col);
		strncpy(secondline, printstr+slen, slen);
		secondline[slen]='\0';
		addstr(secondline);
		addstr("...");
	}
	else {
		addstr(printstr);
	}
}
void intToString(long long filesize) {
	int underten;
	char t;
	if(filesize==0) {
		intString[0]='0';
		intString[1]='\0';
		return ;
	}
	iSsize=0;
	while(true) {
		underten=filesize%10;
		filesize/=10;
		intString[iSsize++]=underten+'0';
		if(filesize==0) break;
	}
	intString[iSsize]='\0';
	for(int i=0; i<=(iSsize-1)/2; i++) {
		t=intString[i];
		intString[i]=intString[iSsize-1-i];
		intString[iSsize-1-i]=t;
	}
}
