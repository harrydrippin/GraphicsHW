main: 
	g++ -std=c++11 -c -o gg.o gg.cpp
	g++ -std=c++11 -c -o MainScene.o MainScene.cpp
	g++ -std=c++11 $@.cpp -o a gg.o MainScene.o -lglut -lglew -framework OpenGL
	./a &
	rm -rf a *.o
	