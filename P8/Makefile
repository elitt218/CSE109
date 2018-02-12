OPTS = -g -Wall -Werror
OBJS = Server.o

all: Server

Server: $(OBJS)
	g++ $(OPTS) -o $@ $^

Server.o: Server.cpp
	g++ $(OPTS) -c Server.cpp

clean:
	rm -f *~*.o Server