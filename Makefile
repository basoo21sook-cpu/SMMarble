CC=clang
CXXFLAGS=-W -Wall
TARGET=smmarble 

all: $(TARGET)

OBJS = main.o smm_database.o smm_object.o

smmarble : $(OBJS)
	$(CC) $(CXXFLAGS) -o $@ $^
		
clean:        
	rm -rf *.o smmarble
