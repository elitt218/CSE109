OPTS = -g -Wall -Werror
OBJS = p7.o Hasher.o

all: p7

p7: $(OBJS)
	g++ $(OPTS) -o $@ $^

p7.o: Hasher.cpp p7.cpp
	g++ $(OPTS) -c p7.cpp

Hasher.o: Hasher.cpp Hasher.h
	g++ $(OPTS) -c Hasher.cpp

clean:
	rm -f *~*.o p7
