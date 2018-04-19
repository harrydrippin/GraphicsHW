main: 
	make build
	make run
	make clear
	
build:
	g++ -std=c++11 -c -o gg.o gg.cpp
	g++ -std=c++11 -c -o MainScene.o MainScene.cpp
	g++ -std=c++11 main.cpp -o a gg.o MainScene.o -lglut -lglew -framework OpenGL

run:
	./a &
	
clear:
	rm -rf a *.o