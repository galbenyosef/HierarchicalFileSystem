myFS: main.o myFile.o myFolder.o myFSEntry.o myTerminal.o
	g++ main.o myFile.o myFolder.o myFSEntry.o myTerminal.o -o myFS
main.o: main.cpp myFolder.h myFSEntry.h myFile.h myTerminal.h
	g++ -Wall -pedantic-errors -ansi -c main.cpp
myFile.o: myFile.cpp myFile.h myFSEntry.h
	g++ -Wall -pedantic-errors -ansi -c myFile.cpp
myFolder.o: myFolder.cpp myFolder.h myFSEntry.h
	g++ -Wall -pedantic-errors -ansi -c myFolder.cpp
myFSEntry.o: myFSEntry.h
	g++ -Wall -pedantic-errors -ansi -c myFSEntry.cpp
myTerminal.o: myTerminal.cpp myTerminal.h myFSEntry.h myFolder.h myFile.h
	g++ -Wall -pedantic-errors -ansi -c myTerminal.cpp
