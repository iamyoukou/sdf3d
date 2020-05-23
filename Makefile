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

SRC_DIR=/Users/YJ-work/sdf3d/src

all: main test solidVoxelizer

main: main.o common.o
	$(CXX) $(LIBS) $^ -o main
	rm -f *.o

test: test.o common.o sdf.o
	$(CXX) -g $(LIBS) $^ -o test
	rm -f *.o

solidVoxelizer: solidVoxelizer.o common.o sdf.o
	$(CXX) -g $(LIBS) $^ -o solidVoxelizer
	rm -f *.o

test.o: $(SRC_DIR)/test.cpp
	$(CXX) -c $(INCS) $^ -o test.o

main.o: $(SRC_DIR)/main.cpp
	$(CXX) -c $(INCS) $^ -o main.o

common.o: $(SRC_DIR)/common.cpp
	$(CXX) -c $(INCS) $^ -o common.o

sdf.o: $(SRC_DIR)/sdf.cpp
	$(CXX) -c $(INCS) $^ -o sdf.o

solidVoxelizer.o: $(SRC_DIR)/solidVoxelizer.cpp
	$(CXX) -c $(INCS) $^ -o solidVoxelizer.o

.PHONY: clean video

clean:
	rm -vf main test ./result/*

video:
	ffmpeg -r 60 -start_number 0 -i ./result/sim%03d.png -vcodec mpeg4 -b:v 30M -s 100x100 ./video/result.mp4
