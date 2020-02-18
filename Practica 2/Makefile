DIROBJ := obj/
DIREXE := exec/
DIRHEA := include/
DIRSRC := src/
FILELOG := log.txt

CC := g++
CFLAGS := -I $(DIRHEA) -c -Wall -std=c++11 -pthread

LFLAGS := -pthread

all : dirs ssooiigle

dirs:
	mkdir -p $(DIROBJ) $(DIREXE)

ssooiigle: $(DIROBJ)ssooiigle.o
	$(CC) -o $(DIREXE)$@ $^ $(LFLAGS)

$(DIROBJ)%.o: $(DIRSRC)%.cpp
	$(CC) $(CFLAGS) $^ -o $@

test1:
	./$(DIREXE)SSOOIIGLE prueba.txt hecho 5

clean : 
	rm -rf *~ core $(DIROBJ) $(DIREXE) $(FILELOG)