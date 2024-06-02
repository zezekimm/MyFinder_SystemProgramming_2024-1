#include <stdio.h>
#include <stdbool.h>
#include <curses.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include "core.h"
#include "source.h"
#define NEWDIRMODE 0775


void intofoldermenu() {
	char c;
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
		
		c=getinput(); // 1 up 2 down 3 right 4 left
		if(c=='p') {
			//menu_paste();
			//printf("%d %s << %s", cut_or_copy, filepath_copy, filepath);
			set_ticker(0);
			out_paste();
			return ;
		}
		else if(c=='n') {
			set_ticker(0);
			make_directory();
			return ;
		}
		else if(c=='s') {
			set_ticker(0);
			choose_sort_type();
			return ;
		}
		else if(c=='t') {
			set_ticker(0);
			foldermenu_property();
			return ;
		}
		else if(c=='h') {
			set_ticker(0);
			show_hidden=!show_hidden;
			return ;
		}
		else if(c==Backspace) {
			set_ticker(0);
			//printf("go back");
			clear();
			return ;
		}
	}
}
void choose_sort_type() {
	char c;
	printmessagebox("Press the number key correspond to sort type.", "1 : Name, 2 : Modified time 3 : Size");
	while(true) {
		c=getinputnotQuit(); // 1 up 2 down 3 right 4 left
		if(c=='1') {
			sortType=1;
			return ;
		}
		else if(c=='2') {
			sortType=2;
			return ;
		}
		else if(c=='3') {
			sortType=3;
			return ;
		}
		else if(c==Backspace) {
			clear();
			return ;
		}
	}
}
void make_directory() {
	printmessagebox("Enter a new directory name.(Quit : ctrl+'d')", ":");
	inputflag=0;
	inputdata();
	if(inputflag==1 && !((!strcmp(inputstr, ".") || !strcmp(inputstr, "..") ) )  ) {
		sprintf(filepath, "%s/%s", cur, inputstr);
		if(access(filepath, F_OK) ) {
			
		}
		mkdir(filepath, NEWDIRMODE);
		chmod(filepath,NEWDIRMODE);
		noFile=0;
	}
	return ;
}
void out_paste(){
	if(cut_or_copy == 0){
		noFile=0;
		return;
	}
	//make new file path
        sprintf(filepath, "%s/%s", cur, copy_file_name);
	
	
	// if a file with same name exists
        if(access(filepath, F_OK) != -1){
		struct stat file_stat1, file_stat2;

		stat(filepath,&file_stat1);//new file
                stat(filepath_copy,&file_stat2);//old file

		//both files are directories or both files are not directories
                if((S_ISDIR(file_stat1.st_mode) && S_ISDIR(file_stat2.st_mode))
			|| (!S_ISDIR(file_stat1.st_mode) && !S_ISDIR(file_stat2.st_mode))){
                	printmessagebox("** A file with the same name exists in this location. Do you want to Replace? **", 
					"** Y / N **");
                                
			refresh();        
			while(true){
				char c = getinput();
				if(c == 'y' || c == 'Y'){
					//when the file is upper or original file
					if(strncmp(filepath, filepath_copy, strlen(filepath)) == 0){
					//flash_message1, "There is file"
					//print_message_flash(500);
					
						printmessagebox("** You cannot replace the file. The file is in the path of the original file. **",  
								"**  Press any keys... **");
						
						refresh();
						char c=getinput();
						clear();
						diropen();
						control_cursor(1);
						return;
					}

					//delete the file
					if(S_ISDIR(file_stat1.st_mode)) {
						chdir(cur);
						delete_function(filepath, cur);
						chdir(cur);
						remove(filepath);
					}
					else {
						remove(filepath);
					}
					break;              
				}
			      	else if( c == 'n' || c == 'N'){
		 			clear();
	 				diropen();
 					control_cursor(1);
					return;
                                }
                        }
                }
                else{ // if file type is different
                        printmessagebox("** A file with the same name exists in that location. **", 
					"**  Press any keys... **");
                        refresh();
                        char c=getinput();
                        clear();
                        diropen();
                        control_cursor(1);
			return;
		}
	}
	if(cut_or_copy == 1){//cut
		rename(filepath_copy, filepath); 
		cut_or_copy = 0;
	}
	else if(cut_or_copy == 2){//copy
		filecopy(filepath_copy, filepath);
	}
	noFile=0;
}
void foldermenu_property() {
	char property_path[500];
	char property_name[500]="\0";
	struct dirent *tmpfile=dir_me;
	struct stat file_statt;
	int property_row=cal_row(1), property_col=cal_col(1);
	int tmp_row, tmp_col, property_name_cnt=0;
	char tmpstr[500], c, ctmp;
	
	strcpy(property_path, cur);
	//printf("hello %s 1\n", property_path);
	for(int i=strlen(property_path)-1; i>=0; i--) {
		if(property_path[i]=='/') break;
		property_name[property_name_cnt++]=property_path[i];
	}
	for(int i=strlen(property_path)-1; i>=0; i--) {
		if(i==0) {
			property_path[1]='\0';
			break;
		}
		if(property_path[i]=='/') { 
			property_path[i]='\0';
			break;
		}
	}
	chdir(property_path);
	
	//sleep(2);
	for(int i=0; i<=(property_name_cnt-1)/2; i++) {
		ctmp=property_name[i];
		property_name[i]=property_name[property_name_cnt-i-1];
		property_name[property_name_cnt-i-1]=ctmp;
	}
	
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
	
	printdir_property(property_row, property_col+19);
	
	property_col+=2; property_row+=6;
	move(property_row++, property_col); 
	//Name: 
	addstr("         Name: ");
	printstr_property(property_name, property_row-1, property_col+15);
	move(++property_row, property_col);
	
	stat(property_name, &file_statt);
	
		addstr("     Contents: ");
		property_file_count=0;
		long long int lltmp=dirSize(property_name);
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
		
	//}

	
	property_row+=2;
	move(property_row, property_col);
	
	//Parent folder: 
	addstr("Parent folder: ");

	property_path[strlen(property_path)-strlen(property_name)-1]='\0';

	printstr_property(property_path, property_row, property_col+15);

	property_row+=2;
	move(property_row++, property_col);
	
	//time: 
	addstr("     Accessed: ");
	strcpy(tmpstr, ctime(&file_statt.st_atime));
	tmpstr[strlen(tmpstr)-1]='\0';
	addstr(tmpstr);
	move(property_row++, property_col);
	
	addstr("     Modified: ");
	strcpy(tmpstr, ctime(&file_statt.st_mtime));
	tmpstr[strlen(tmpstr)-1]='\0';
	addstr(tmpstr);
	move(property_row++, property_col);
	
	addstr("      Created: ");
	strcpy(tmpstr, ctime(&file_statt.st_ctime));
	tmpstr[strlen(tmpstr)-1]='\0';
	addstr(tmpstr);
	
	refresh();
	while(true) {
		c=getinput();
		if(c==' ' || c==Enter || c==Backspace) {
			clear();
			chdir(cur);
			return ;
		}
	}
}

