#define Backspace 127
#define Enter 13
#define ctrlD 4
#define BUFFERSIZE 4096
#define COPYMODE 0664

#define flashingTime 1000

#define cal_row(row) 8*(row-1)+7
#define cal_col(col) 15*(col-1)+5

extern char cur[];
extern char filepath[];
extern struct dirent *dir_arr[], *dir_me, *dir_papa;
extern int menu_cursor_col, menu_cursor_row;
extern int cursor_num, filecnt;
extern int add_cursor;
extern int para_row, para_col;
extern int cut_or_copy, noFile, screen_row, screen_col;
extern char filepath_copy[1000]; //original file path for cut, copy
extern char copy_file_name[256]; //name of original file
extern int inputflag, flashing, change_screen;
static char staticmainline1[50]={"Press \"Space bar\" to go to the selected directory."};
static char staticmainline2[50]={"Press \"Backspace\" to go to the upper directory."};
extern char inputstr[], mainline1[], mainline2[];
extern char *units[];
extern int flash_num, sortType, show_hidden;
extern int property_file_count;
void inputdata();
void makefilepath();
long long int dirSize();
void foldermenu_flash();
