SOURCES = main.cpp help.cpp Teapot.cpp
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
	$(CC) $(SOURCES) $(CFLAGS) -o $(EXECUTABLE)

run:
	./$(EXECUTABLE) &

clean:
	$(RM) *.o $(EXECUTABLE)
