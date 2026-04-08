#include "fsgl.hpp"
#include <iostream>

void render() {
    fsgl::fs::fsClear();
    fsgl::fs::fsSetColor(0.0f, 1.0f, 0.0f);
    fsgl::fs::fsDrawTriangle(-0.5, -0.5,
                              0.5, -0.5,
                              0.0f, 0.5f, true);
}

int main(void) {

    if(fsgl::fs::fsInit() != fsgl::FSOK) {
        std::cerr << "Failed to initialize FSGL" << std::endl;
        return -1;
    }

    fsgl::fsWindow window = fsgl::fs::fsNewWindow("FSGL Triangles test", 800, 600);
    if(!window) { fsgl::fs::fsDrop(); return -1; }

    fsgl::fs::fsSetContext(window);
    fsgl::fs::fsInitContext();

    fsgl::fs::fsPollEvents(window, render);

    fsgl::fs::fsDrop();
    return 0;
}