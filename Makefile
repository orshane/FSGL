all:
	clang++ *.cpp -o bin/FSGL -g -lglfw -lGL -lGLEW -lpthread -lX11