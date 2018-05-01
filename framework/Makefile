# main: 
# 	make build
# 	make run
# 	make clear
	
# build:
# 	g++ -std=c++11 -c -o gg.o gg.cpp
# 	g++ -std=c++11 -c -o MainScene.o MainScene.cpp
# 	g++ -std=c++11 main.cpp -o a gg.o MainScene.o -lglut -lglew -framework OpenGL

# run:
# 	./a &
	
# clear:
# 	rm -rf a *.o

SOURCES = main.cpp gg.cpp MainScene.cpp
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
