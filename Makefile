CXX=llvm-g++
INCS=-c -std=c++17 \
-I/usr/local/Cellar/glew/2.1.0_1/include \
-I/usr/local/Cellar/glfw/3.3.2/include \
-I/usr/local/Cellar/freeimage/3.18.0/include \
-I/usr/local/Cellar/glm/0.9.9.8/include \
-I/Users/YJ-work/cpp/myGL_glfw/sdf3d/header

LIBS=-L/usr/local/Cellar/glew/2.1.0_1/lib -lglfw \
-L/usr/local/Cellar/glfw/3.3.2/lib -lGLEW \
-L/usr/local/Cellar/freeimage/3.18.0/lib -lfreeimage \
-framework GLUT -framework OpenGL -framework Cocoa

SRC_DIR=/Users/YJ-work/cpp/myGL_glfw/sdf3d/src

all: createSdf solidVoxelizer simulation sdfVisualizer

createSdf: createSdf.o common.o sdf.o
	$(CXX) -g $(LIBS) $^ -o createSdf
	rm -f *.o

solidVoxelizer: solidVoxelizer.o common.o sdf.o
	$(CXX) -g $(LIBS) $^ -o solidVoxelizer
	rm -f *.o

simulation: simulation.o common.o sdf.o
	$(CXX) -g $(LIBS) $^ -o $@
	rm -f *.o

sdfVisualizer: sdfVisualizer.o common.o sdf.o
	$(CXX) -g $(LIBS) $^ -o $@
	rm -f *.o


createSdf.o: $(SRC_DIR)/createSdf.cpp
	$(CXX) -c $(INCS) $^ -o createSdf.o

common.o: $(SRC_DIR)/common.cpp
	$(CXX) -c $(INCS) $^ -o common.o

sdf.o: $(SRC_DIR)/sdf.cpp
	$(CXX) -c $(INCS) $^ -o sdf.o

solidVoxelizer.o: $(SRC_DIR)/solidVoxelizer.cpp
	$(CXX) -c $(INCS) $^ -o solidVoxelizer.o

simulation.o: $(SRC_DIR)/simulation.cpp
	$(CXX) -c $(INCS) $^ -o $@

sdfVisualizer.o: $(SRC_DIR)/sdfVisualizer.cpp
	$(CXX) -c $(INCS) $^ -o $@

.PHONY: clean video

clean:
	rm -v ./result/*

video:
	ffmpeg -r 30 -start_number 0 -i ./result/output%04d.bmp -vcodec mpeg4 -b:v 30M -s 400x300 ./result.mp4
