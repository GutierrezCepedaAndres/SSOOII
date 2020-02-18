DIROBJ := obj/
DIREXE := exec/
DIRHEA := include/
DIRSRC := src/
DIRDIRS := dirs/
FILELOG := log.txt

CFLAGS := -I$(DIRHEA) -c -Wall -ansi
LDLIBS := -lpthread
CC := gcc

all : dirs manager ProcesoA ProcesoB ProcesoC ProcesoD

dirs:
	mkdir -p $(DIROBJ) $(DIREXE) $(DIRDIRS)

manager: $(DIROBJ)manager.o 
	$(CC) -o $(DIREXE)$@ $^ $(LDLIBS)

ProcesoA: $(DIROBJ)ProcesoA.o 
	$(CC) -o $(DIREXE)$@ $^ $(LDLIBS)

ProcesoB: $(DIROBJ)ProcesoB.o 
	$(CC) -o $(DIREXE)$@ $^ $(LDLIBS)

ProcesoC: $(DIROBJ)ProcesoC.o 
	$(CC) -o $(DIREXE)$@ $^ $(LDLIBS)

ProcesoD: $(DIROBJ)ProcesoD.o 
	$(CC) -o $(DIREXE)$@ $^ $(LDLIBS)

$(DIROBJ)%.o: $(DIRSRC)%.c
	$(CC) $(CFLAGS) $^ -o $@

solution:
	./$(DIREXE)manager

clean : 
	rm -r $(DIROBJ) $(DIREXE) $(DIRDIRS) $(FILELOG)
