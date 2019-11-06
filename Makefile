INCS=-I/usr/local/Cellar/glew/2.1.0/include \
-I/usr/local/Cellar/glfw/3.3/include \
-I/usr/local/Cellar/freeimage/3.18.0/include \
-I/usr/local/Cellar/anttweakbar/1.16/include \
-I/usr/local/Cellar/glm/0.9.9.5/include \
-I/Users/YJ-work/sdf3d/header

LIBS=-L/usr/local/Cellar/glew/2.1.0/lib -lglfw \
-L/usr/local/Cellar/glfw/3.3/lib -lGLEW \
-L/usr/local/Cellar/freeimage/3.18.0/lib -lfreeimage \
-L/usr/local/Cellar/anttweakbar/1.16/lib -lAntTweakBar \
-framework GLUT -framework OpenGL -framework Cocoa

all: main

main: main.o common.o
	g++ $(LIBS) main.o common.o -o main
	rm -f *.o

main.o: ./src/main.cpp
	g++ -c $(INCS) ./src/main.cpp -o main.o

common.o: ./src/common.cpp
	g++ -c $(INCS) ./src/common.cpp -o common.o

.PHONY: clean

clean:
	rm -vf main
