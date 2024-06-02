
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <curses.h>
#include <time.h>
#include "core.h"
#include "source.h"

int cut_or_copy=0; //1=cut, 2=copy, 0=not copied yet
struct stat file_stat;//for check the file is directory or not
char filepath_copy[1000]; //original file path for cut, copy
char copy_file_name[256]; //name of original file
int inputflag=0;
int menu_size;
char *units[10]={"bytes", "KB", "MB", "GB", "TB"};
void intomenu() {
	char c;
	add_cursor=0;
	menu_cursor_row=para_row+2;
	menu_cursor_col=para_col+12;
	menu_cursor();
	menu_size=6;
	while(true) {
		set_screen_size();
		if(change_screen) {
			dir_explore();
			clear();
			diropen();
			printmessagebox(mainline1, mainline2);
			control_cursor(0);
			return ;
		}
		
		move(menu_cursor_row, menu_cursor_col);
		addstr("  ");
		menu_cursor_row=para_row+2+add_cursor;
		menu_cursor();  //for cursor
		
		c=getinput(); // 1 up 2 down 3 right 4 left
		if(c>='1' && c<='2') {
			if(c=='1') {
				if(add_cursor==0) add_cursor=menu_size;
				else add_cursor--;
			}
			else if(c=='2') {
				if(add_cursor==menu_size) add_cursor=0;
				else add_cursor++;
			}
			
		}
		if(c==' ') {
			if(add_cursor==0) { //cut
				menu_cut();
				print_message_flash(flashingTime, "Cut Complete!!", "");
				return ;
			}	
			else if(add_cursor==1) { //copy
				menu_copy();
				print_message_flash(flashingTime, "Copy Complete!!", "");
				return ;

			}	
			else if(add_cursor==2) { //rename
				menu_rename();
				print_message_flash(flashingTime, "Rename Complete!!", "");
				return ;
			}	
			else if(add_cursor==3) { // permission
				menu_permission();
				print_message_flash(flashingTime, "Change permission Complete!!", "");
				return ;
			}
			else if(add_cursor==4) { // delete
				menu_delete();
				print_message_flash(flashingTime, "Delete Complete!!", "");
				return ;
			}
			/*else if(add_cursor==5) { // shortcut
				//menu_shortcut();
			}*/	
			else if(add_cursor==5) { // property
				menu_property();
				return ;
			}
			else if(add_cursor==6) { // paste
				menu_paste();
				print_message_flash(flashingTime, "Paste Complete!!", "");
				return ;
			}
		}
		if(c==Backspace) {
			//printf("go back");
			clear();
			return ;
		}
		
	}
}
void intofilemenu() {
	char c;
	add_cursor=0;
	menu_cursor_row=para_row+2;
	menu_cursor_col=para_col+12;
	menu_cursor();
	menu_size=5;
	while(true) {
		//arrow=0;
		move(menu_cursor_row, menu_cursor_col);
		addstr("  ");
		menu_cursor_row=para_row+2+add_cursor;
		menu_cursor();  //for cursor
		
		c=getinput(); // 1 up 2 down 3 right 4 left
		if(c>='1' && c<='2') {
			if(c=='1'){
				if(add_cursor==0) add_cursor=menu_size;
				else add_cursor--;
			}
			else if(c=='2') {
				if(add_cursor==menu_size) add_cursor=0;
				else add_cursor++;
			}
			
		}
		if(c==' ') {
			if(add_cursor==0) { //cut
				menu_cut();
				return ;
			}	
			else if(add_cursor==1) { //copy
				menu_copy();
				return ;

			}	
			else if(add_cursor==2) { //rename
				//
				menu_rename();
				return ;
			}	
			else if(add_cursor==3) { // permission
				//
				menu_permission();
				return ;
			}
			else if(add_cursor==4) { // delete
				menu_delete();
				return ;
			}
			/*else if(add_cursor==5) { // shortcut
				//menu_shortcut();
			}*/	
			else if(add_cursor==5) { // property
				menu_property(0);
				return ;
			}/*
			else if(add_cursor==6) { // paste
				menu_paste();
				return ;
			}*/
		}/*
		if(c=='x') {
			//cursor_now_row, cursor_now_col
			printmenu();
			intomenu();
		}*/
		if(c==Backspace) {
			//printf("go back");
			clear();
			return ;
		}
		
	}
}



//___________________________________________________________________
int property_file_count;
void menu_property() {
	struct dirent *tmpfile=dir_arr[cursor_num];
	int property_row=cal_row(1), property_col=cal_col(1);
	int tmp_row, tmp_col;
	char tmpstr[500], c;
	move(property_row++, property_col);
	addstr(" ______________________________________________ "); move(property_row++, property_col);
	tmp_row=property_row; tmp_col=property_col;
	addstr("|                                              |"); move(property_row++, property_col);
	addstr("|                                              |"); move(property_row++, property_col);
	addstr("|                                              |"); move(property_row++, property_col);
	addstr("|                                              |"); move(property_row++, property_col);
	addstr("|                                              |"); move(property_row++, property_col);
	addstr("|                                              |"); move(property_row++, property_col);
	addstr("|                                              |"); move(property_row++, property_col);
	addstr("|                                              |"); move(property_row++, property_col);
	addstr("|                                              |"); move(property_row++, property_col);
	addstr("|                                              |"); move(property_row++, property_col);
	addstr("|                                              |"); move(property_row++, property_col);
	addstr("|                                              |"); move(property_row++, property_col);
	addstr("|                                              |"); move(property_row++, property_col);
	addstr("|                                              |"); move(property_row++, property_col);
	addstr("|                                              |"); move(property_row++, property_col);
	addstr("|                                              |"); move(property_row++, property_col);		
	addstr("|                                              |"); move(property_row++, property_col);
	addstr("|______________________________________________|"); move(property_row++, property_col);
	property_row=tmp_row; property_col=tmp_col;
	if(tmpfile->d_type==4) printdir_property(property_row, property_col+19);
	else printfile_property(property_row, property_col+19);
	property_col+=2; property_row+=6;
	move(property_row++, property_col); 
	//Name: 
	addstr("         Name: ");
	printstr_property(tmpfile->d_name, property_row-1, property_col+15);
	move(++property_row, property_col);
	//Contents: filenumber, sizeofdirectory
	//addstr("Contents"); Size
	makefilepath();
	stat(filepath, &file_stat);
	
	if(tmpfile->d_type==4) {
		addstr("     Contents: ");
		property_file_count=0;
		long long int lltmp=dirSize(tmpfile->d_name);
		int unitcnt=0;
		int floatingpoint=0;
		intToString(property_file_count);
		addstr(intString);
		addstr(" items, ");
		while(1) {
			if(lltmp<1000) {
				break;
			}
			else {
				floatingpoint=lltmp%1000;
				lltmp/=1000;
				unitcnt++;
			}
		}
		intToString(lltmp);
		addstr(intString);
		addch('.');
		intToString(floatingpoint);
		addstr(intString);
		addch(' ');
		addstr(units[unitcnt]);
		
	}
	else {
		addstr("         Size: ");
		long long int lltmp=file_stat.st_size;
		int unitcnt=0;
		int floatingpoint=0;
		while(1) {
			if(lltmp<1000) {
				break;
			}
			else {
				floatingpoint=lltmp%1000;
				lltmp/=1000;
				unitcnt++;
			}
		}
		intToString(lltmp);
		addstr(intString);
		addch('.');
		intToString(floatingpoint);
		addstr(intString);
		addch(' ');
		addstr(units[unitcnt]);
	}
	property_row+=2;
	move(property_row, property_col);
	
	//Parent folder: 
	addstr("Parent folder: ");
	makefilepath();
	filepath[strlen(filepath)-strlen(tmpfile->d_name)-1]='\0';
	for(int i=strlen(filepath); i>=0; i--) {
		if(filepath[i]=='/') {
			filepath[i]='\0';
			break;
		}
	}
	printstr_property(cur, property_row, property_col+15);
	
	//tmpmode=file_stat.st_mode;
	//file_stat.
	
	//addstr("Parent folder: ");
	property_row+=2;
	move(property_row++, property_col);
	//time: 
	addstr("     Accessed: ");
	strcpy(tmpstr, ctime(&file_stat.st_atime));
	tmpstr[strlen(tmpstr)-1]='\0';
	addstr(tmpstr);
	move(property_row++, property_col);
	
	addstr("     Modified: ");
	strcpy(tmpstr, ctime(&file_stat.st_mtime));
	tmpstr[strlen(tmpstr)-1]='\0';
	addstr(tmpstr);
	move(property_row++, property_col);
	
	addstr("      Created: ");
	strcpy(tmpstr, ctime(&file_stat.st_ctime));
	tmpstr[strlen(tmpstr)-1]='\0';
	addstr(tmpstr);
	
	refresh();
	while(true) {
		c=getinput();
		if(c==' ' || c==Enter || c==Backspace) {
			clear();
			return ;
		}
	}
}

//________________________________________________________________________
void menu_permission() {
	int permission_row=cal_row(1), permission_col=cal_col(1);
	int tmpmode, mode_arr[10]={0}, permission_cursor=1;
	char c;
	makefilepath();
	stat(filepath, &file_stat);
	tmpmode=file_stat.st_mode;
	decodeMode(tmpmode, mode_arr);
	while(true) {
		print_permission(permission_row, permission_col, mode_arr, permission_cursor);
		c=getinput(); // 1 up 2 down 3 right 4 left
		if(c>='3' && c<='4') {
			if(c=='4') { // left
				permission_cursor--;
				if(permission_cursor==0) permission_cursor=9;
			}
			else { //right
				permission_cursor++;
				if(permission_cursor==10) permission_cursor=1;	
			}
		}
		if(c==' ') {
			mode_arr[permission_cursor]*=-1;
		}
		if(c==Backspace) {
			//printf("go back");
			clear();
			return ;
		}
		if(c==Enter) {
			tmpmode=recodeMode(mode_arr);
			chmod(filepath, tmpmode);
			clear();
			return ;
		}
	}	
}

void menu_rename() {
	//char newname[500];
	printmessagebox("Enter a new file name to change.(Quit : ctrl+'d')", ":");
	//signal(SIGINT, return;);
	//curs_set(1);	
	move(4, 6);
	addstr(" ");
	
	//reset_mode();
	inputflag=0;
	inputdata();
	
	if(inputflag==1 && !((!strcmp(inputstr, ".") || !strcmp(inputstr, "..") ) )  ) {
		makefilepath();
		char buff[1000];
		sprintf(buff, "%s/%s", cur, inputstr);
		
		if(access(buff, F_OK) != -1){ // if a file with same name exists
                        printmessagebox("** A file with the same name exists in that location. Press any keys... **", "");
                        refresh();
			char c=getinput();
			clear();
			return;
                }
		//printf("%s", inputstr);
		if(strncmp(filepath, filepath_copy, strlen(filepath)) == 0)
                	cut_or_copy = 0;//original file or upper file deleted, then can not copy
		
		if(rename(filepath, buff)==-1) {
			perror("Can't rename");
			//printf("\n%s\n%s\n", filepath, inputstr);
			EXIT(1);
		}
	//	rename();
	}
	//scanf("%s", newname);
	dir_explore();
	diropen();
	refresh();
	//curs_set(0);
}


void menu_cursor() { //para_row, para_col
	move(menu_cursor_row, menu_cursor_col);
	printcursor();
	refresh();
}

void makefilepath() {
	sprintf(filepath, "%s/%s", cur, dir_arr[cursor_num]->d_name);
}

void filecopy(char* old, char* new){
	int pid;
	if((pid = fork()) == -1 ){
		perror("fork");
		exit(1);
	}
	else if(pid > 0){//parent
		wait(NULL);
	}
	else if (pid == 0){
	//child
	execlp("cp", "cp", "-r", "-p", old, new, NULL);
	perror("cp");
	exit(1);
	}
}

void menu_cut() {
	cut_or_copy = 1;
	makefilepath();
	strcpy(filepath_copy, filepath);			//store original file path
	strcpy(copy_file_name, dir_arr[cursor_num]->d_name);	//store original file name
	clear();
	diropen();
	control_cursor(1);
}

void menu_copy() {
	cut_or_copy = 2;
	makefilepath();
	strcpy(filepath_copy, filepath);			//store original file path
	strcpy(copy_file_name, dir_arr[cursor_num]->d_name);	//store original file name
	clear();
	diropen();
	control_cursor(1);
}
//___________________________________________________________________
void delete_function(char *deletefile, char *current) {
	char tmperstr[500]={'\0'}, tch;
	struct dirent *direntp;
	int deletedir_cnt;
	DIR *dir_ptr;
	
	if((dir_ptr=opendir(deletefile))==NULL) {
		fprintf(stderr, "%s : Can't open delete directory!\n", deletefile);
		//getcwd(tmperstr, sizeof(tmperstr));
		//printf("%s\r\n", tmperstr);			
		//sprintf(tmperstr, "%s/%s", tmperstr, deletefile);
		//printf("%s\n\n", tmperstr);
		EXIT(1);
	}
	else { 
		//in deletefile(folder)
		while((direntp=readdir(dir_ptr))!=NULL) {
			if(strcmp(direntp->d_name, "..")==0) {
				continue;
			}
			else if(strcmp(direntp->d_name, ".") ==0) {
				 continue;
			}
			
			if(direntp->d_type==4) {
				//delete folder's file
				chdir(deletefile);
				getcwd(tmperstr, sizeof(tmperstr));
				sprintf(tmperstr, "%s/%s", tmperstr, direntp->d_name);
				
				delete_function(tmperstr, deletefile);
				
				//delete folder
				chdir(current);
				remove(tmperstr);
				
			}
			else {
				chdir(deletefile);
				getcwd(tmperstr, sizeof(tmperstr));
				sprintf(tmperstr, "%s/%s", tmperstr, direntp->d_name);
				
				remove(tmperstr);
			}
		}
	}
	closedir(dir_ptr);
}
void menu_delete() {
	makefilepath();
	if(strcmp(filepath, filepath_copy) == 0)
		cut_or_copy = 0;//original file deleted, then can not copy
	if(dir_arr[cursor_num]->d_type==4) {
		chdir(cur);
		
		delete_function(filepath, cur);
		chdir(cur);
		remove(filepath);
	}
	else {
		remove(dir_arr[cursor_num]->d_name);
	}
	clear();
	dir_explore();
	diropen();
	
	
	control_cursor(1);
	return ;
}


void menu_paste() {
	if(cut_or_copy == 0){//not copied yet
		clear();
        	diropen();
        	control_cursor(1);
		return;
	}
	makefilepath();
	stat(filepath,&file_stat);

	if(S_ISDIR(file_stat.st_mode)){//check the file is directory	
		//make new file path
                sprintf(filepath, "%s/%s", filepath, copy_file_name);
		// if a file with same name exists
		if(access(filepath, F_OK) != -1){
                        printmessagebox("** A file with the same name exists in that location. Press any keys... **", "");
                        refresh();
			char c = getinput();
			clear();
			return;
                }
		if(cut_or_copy == 1){ // cut
			//when paste to same directory
			//if(strcmp(filepath_copy, filepath) == 0){
			//	cut_or_copy = 0;
			//	return;	
			//}
			if(rename(filepath_copy, filepath) == -1){
				perror("cut_paste");
			}
			cut_or_copy = 0;
		}
		else if (cut_or_copy == 2){ //copy
			//when paste to same directory
			//if(strcmp(filepath_copy, filepath) == 0)
			//	sprintf(filepath, "%s_%s", filepath, "copy");
			filecopy(filepath_copy, filepath);
		}
	}
	
	clear();
	diropen();
	control_cursor(1);
}


