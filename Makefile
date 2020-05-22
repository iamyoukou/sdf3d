CXX=llvm-g++
INCS=-c -std=c++17 \
-I/usr/local/Cellar/glew/2.1.0_1/include \
-I/usr/local/Cellar/glfw/3.3.2/include \
-I/usr/local/Cellar/freeimage/3.18.0/include \
-I/usr/local/Cellar/anttweakbar/1.16/include \
-I/usr/local/Cellar/glm/0.9.9.8/include \
-I/usr/local/Cellar/opencv/4.3.0/include/opencv4 \
-I/Users/YJ-work/sdf3d/header

LIBS=-L/usr/local/Cellar/glew/2.1.0_1/lib -lglfw \
-L/usr/local/Cellar/glfw/3.3.2/lib -lGLEW \
-L/usr/local/Cellar/freeimage/3.18.0/lib -lfreeimage \
-L/usr/local/Cellar/anttweakbar/1.16/lib -lAntTweakBar \
-L/usr/local/Cellar/opencv/4.3.0/lib -lopencv_core -lopencv_highgui -lopencv_imgcodecs \
-lopencv_imgproc \
-framework GLUT -framework OpenGL -framework Cocoa

all: main test

main: main.o common.o
	$(CXX) $(LIBS) main.o common.o -o main
	rm -f *.o

test: test.o common.o sdf.o
	$(CXX) -g $(LIBS) $^ -o test
	rm -f *.o

test.o: ./src/test.cpp
	$(CXX) -c $(INCS) ./src/test.cpp -o test.o

main.o: ./src/main.cpp
	$(CXX) -c $(INCS) ./src/main.cpp -o main.o

common.o: ./src/common.cpp
	$(CXX) -c $(INCS) ./src/common.cpp -o common.o

sdf.o: ./src/sdf.cpp
	$(CXX) -c $(INCS) ./src/sdf.cpp -o sdf.o

.PHONY: clean video

clean:
	rm -vf main test ./result/*

video:
	ffmpeg -r 60 -start_number 0 -i ./result/sim%03d.png -vcodec mpeg4 -b:v 30M -s 100x100 ./video/result.mp4
