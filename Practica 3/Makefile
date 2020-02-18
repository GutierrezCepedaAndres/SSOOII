DIROBJ := obj/
DIREXE := exec/
DIRHEA := include/
DIRSRC := src/
DIR := dirs/

CC := g++
CF := -pthread -std=c++11 


all : dirs Cine

dirs:
	mkdir -p $(DIROBJ) $(DIREXE) $(DIRHEA) $(DIR)

Cine: 
	$(CC) $(DIRSRC)/Cine.cpp -o $(DIROBJ)/Cine.o $(CF)
	$(CC) $(DIRSRC)/Cine.cpp -o $(DIREXE)/Cine $(CF)

test:
	./$(DIREXE)Cine

clean : 
	rm -rf *~ core $(DIROBJ) $(DIREXE) $(DIRHEA) 

cleanDirs : 
	rm -rf *~ core $(DIR)