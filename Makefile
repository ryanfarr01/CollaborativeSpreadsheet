

all: spreadsheet_server.o spreadsheet.o
	g++ spreadsheet_server.o spreadsheet.o /usr/local/lib/libboost_regex.a /usr/local/lib/libboost_system.a /usr/local/lib/libboost_filesystem.a

spreadsheet_server.o:
	g++ -c spreadsheet_server.cpp 

spreadsheet.o:
	g++ -c spreadsheet.h spreadsheet.cpp 

clean:
	rm *o a.out