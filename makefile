all: main.o 
	g++ -o SetCover main.o 
main.o: main.cpp 
	g++ -c main.cpp

clean: cleanobj cleanmain 

cleanobj:
	rm -f *.o

cleanmain:
	rm SetCover

.PHONY: clean cleanobj cleanmain