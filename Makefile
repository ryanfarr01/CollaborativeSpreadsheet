

all: spreadsheet_server.o spreadsheet.o
	g++ spreadsheet_server.o spreadsheet.o /usr/local/lib/libboost_regex.a /usr/local/lib/libboost_system.a /usr/local/lib/libboost_filesystem.a -o spreadsheet_server

spreadsheet_server.o:
	g++ -c spreadsheet_server.cpp

spreadsheet.o:
	g++ -c spreadsheet.h spreadsheet.cpp 

clean:
	-rm -f *.o spreadsheet_server *.h.gch