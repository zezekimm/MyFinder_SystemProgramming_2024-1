#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pthread.h>

#include "core.h"
#include "source.h"

int compareByName(const void *a, const void *b) {
	char name1[300], name2[300];
	struct dirent *direntp1 = *((struct dirent **)a);
	struct dirent *direntp2 = *((struct dirent **)b);
	strcpy(name1, direntp1->d_name);
	strcpy(name2, direntp2->d_name);
	for(int i=0; i<strlen(name1); i++) {
		if(name1[i]>='A' && name1[i]<='Z') name1[i]+='a'-'A';
	}
	for(int i=0; i<strlen(name2); i++) {
		if(name2[i]>='A' && name2[i]<='Z') name2[i]+='a'-'A';
	}
	return strcmp(name1, name2);
}


int compareByMtime(const void *a, const void *b) {
    struct dirent *direntp1 = *((struct dirent **)a);
    struct dirent *direntp2 = *((struct dirent **)b);
    struct stat file1, file2;
    if(stat(direntp1->d_name, &file1) == -1) {
    	perror("stat()");
    }
    if(stat(direntp2->d_name, &file2) == -1) {
    	perror("stat()");
    }
    return file1.st_mtime - file2.st_mtime;
}

long long int dirSize(char *dirname) {
    DIR *dir;
    struct dirent *dit;
    struct stat st;
    long long int totalSize = 0, dirsize, size;
    char path[1024];
    if ((dir = opendir(dirname)) == NULL) {
        return 0;
    }

    while ((dit = readdir(dir)) != NULL) {
        sprintf(path, "%s/%s", dirname, dit->d_name);
        if (lstat(path, &st) == -1) {
            continue;
        }

        if (S_ISDIR(st.st_mode)) {
            if (!strcmp(dit->d_name, ".") || !strcmp(dit->d_name, "..")) {
                continue; 
            }
            property_file_count++;
            dirsize = dirSize(path)+4096;
            totalSize += dirsize;
        } 
        else {
            property_file_count++;
            size = st.st_size;
            totalSize += size;
        }
        
    }
    closedir(dir);
    
    return totalSize;
}



int compareBySize(const void *a, const void *b) {
    struct dirent *direntp1 = *((struct dirent **)a);
    struct dirent *direntp2 = *((struct dirent **)b);
    struct stat file1, file2;
    long long int file1Size, file2Size;
    if(stat(direntp1->d_name, &file1) == -1) {
    	perror("stat()");
    }
    if(stat(direntp2->d_name, &file2) == -1) {
    	perror("stat()");
    }
    if(S_ISDIR(file1.st_mode)) {
    	file1Size = dirSize(direntp1->d_name);
    }
    else
    	file1Size = file1.st_size;
    
    if(S_ISDIR(file2.st_mode)) {
    	file2Size = dirSize(direntp2->d_name);
    }
    else {
    	file2Size = file2.st_size;
    }
    return file1Size - file2Size;
}

struct dirdir {
	struct dirent *direntp;
	long long int size;
} dir_size[1000];

long long int compareBySize2(const void *a, const void *b) {
    return (((struct dirdir *)a)->size) > (((struct dirdir *)b)->size);
}

void sizeSearch(void* arg) {
	char *darg=(char*)arg;
	int st=0, ed=0, key;
	for(key=0; ; key++) {
		if(darg[key]=='/') break;
		st*=10;
		st+=darg[key]-'0';
	}
	for(int i=key+1; i<strlen(darg); i++) {
		ed*=10;
		ed+=darg[i]-'0';
	}
	for(int i=st; i<=ed; i++) {
		dir_size[i].size=dirSize(dir_size[i].direntp->d_name);
	}
}

char argu[5][100];
void makeargu(int argnum, int st, int ed) {
	int key;
	intToString(st);
	key=strlen(intString);
	strcpy(argu, intString);
	intToString(ed);
	sprintf(argu[argnum], "%s/%s", argu, intString);
}
void dirSort(int sort_type) {
	pthread_t t1, t2, t3, t4;
    if (sort_type==1) {
        qsort(dir_arr+1, filecnt, sizeof(struct dirent *), compareByName);
    }
    else if (sort_type==2) {
        qsort(dir_arr+1, filecnt, sizeof(struct dirent *), compareByMtime);
    } 
    else if (sort_type==3) {
    	struct stat st;
    	int key=1, dirdirnum;
    	for(int i=1; i<=filecnt; i++) {
    		if(dir_arr[i]->d_type==4) {
    			dir_size[key++].direntp=dir_arr[i];
 			//dir_size[key++]=st.st_size;
 		}
    	}
    	dirdirnum=key-1;
    	for(int i=1; i<=filecnt; i++) {
    		if(dir_arr[i]->d_type!=4) {
 			stat(dir_arr[i], &st);
 			dir_size[key].direntp=dir_arr[i];
 			dir_size[key++].size=st.st_size;
 		}
    	}
    	//key--;
    	if(dirdirnum==2) {
    		makeargu(1, 1, 1);
    		pthread_create(&t1, NULL, sizeSearch, argu[1]);
    		
    		makeargu(2, 2, 2);
    		pthread_create(&t2, NULL, sizeSearch, argu[2]);
    		
    		pthread_join(t1, NULL);
    		pthread_join(t2, NULL);
    	}
    	if(dirdirnum==3) {
    		makeargu(1, 1, 1);
    		pthread_create(&t1, NULL, sizeSearch, argu[1]);
    		
    		makeargu(2, 2, 2);
    		pthread_create(&t2, NULL, sizeSearch, argu[2]);
    		
    		makeargu(3, 3, 3);
    		pthread_create(&t3, NULL, sizeSearch, argu[3]);
    		
    		pthread_join(t1, NULL);
    		pthread_join(t2, NULL);
    		pthread_join(t3, NULL);
    	}
    	else if(dirdirnum>=4) {
    		int tmpnum=dirdirnum/2;
    		
    		makeargu(1, 1, tmpnum/2);
    		//printf("%d %d\n", 1, tmpnum/2);
    		pthread_create(&t1, NULL, sizeSearch, argu[1]);
    		
    		makeargu(2, tmpnum/2+1, tmpnum);
    		//printf("%d %d\n", tmpnum/2+1, tmpnum);
    		pthread_create(&t2, NULL, sizeSearch, argu[2]);
    		
    		tmpnum=(dirdirnum+(dirdirnum/2+1))/2;
    		
    		makeargu(3, dirdirnum/2+1, tmpnum);
    		//printf("%d %d\n", dirdirnum/2+1, tmpnum);
    		pthread_create(&t3, NULL, sizeSearch, argu[3]);
    		
    		makeargu(4, tmpnum+1, dirdirnum);
    		//printf("%d %d\n", tmpnum+1, dirdirnum);
    		pthread_create(&t4, NULL, sizeSearch, argu[4]);
    		
    		pthread_join(t1, NULL);
    		pthread_join(t2, NULL);
    		pthread_join(t3, NULL);
    		pthread_join(t4, NULL);
    	}
    	// _______________________________
    	qsort(dir_size+1, dirdirnum, sizeof(struct dirdir), compareBySize2);
    	qsort(dir_size+dirdirnum+1, filecnt-dirdirnum, sizeof(struct dirdir), compareBySize2);
    	for(int i=1; i<=filecnt; i++) {
    		dir_arr[i]=dir_size[i].direntp;
    		//printf("%s : %lld\n", dir_arr[i]->d_name, dir_size[i].size);
    	}
    	//sleep(1000);
    }
}
