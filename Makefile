.PHONY : clean
RM = rm -f

myfinder: core.o menu.o source.o dirsort.o foldermenu.o menusource.o core.h source.h
	gcc -w core.o menu.o source.o dirsort.o foldermenu.o menusource.o -lcurses -lpthread -o myfinder
core.o:core.c core.h
	gcc -w -Wall -g -c core.c -lcurses
menu.o: menu.c core.h
	gcc -w -Wall -g -c menu.c -lcurses
source.o: source.c core.h source.h
	gcc -w -Wall -g -c source.c -lcurses
menusource.o : menusource.c source.h
	gcc -w -Wall -g -c menusource.c -lcurses
dirsort.o: dirsort.c core.h
	gcc -w -Wall -g -c dirsort.c -lcurses -lpthread
foldermenu.o: foldermenu.c core.h
	gcc -w -Wall -g -c foldermenu.c -lcurses
clean:
	$(RM) *.o
