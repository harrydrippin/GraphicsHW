SOURCES = main.cpp help.cpp
CC = g++
CFLAGS = -std=c++11 -framework OpenGL -lglut -lGLEW
# CFLAGS = -std=c++11 -lGL -lglut -lGLEW
EXECUTABLE = a
RM = rm -rf

all: 
	@make build
	@make run
	@make clean

build:
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(SOURCES)

run:
	./$(EXECUTABLE) &

clean:
	$(RM) *.o $(EXECUTABLE)
