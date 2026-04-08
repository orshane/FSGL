FSGL – From Scratch Graphics Library

FSGL is a minimalist, beginner-friendly graphics library built on OpenGL and GLFW. Run 2D & 3D demos in under 30 lines of code. Perfect for learning graphics without the pain of boilerplate.


Features

🎨 Simple color management: fsSetColor(r, g, b)

✏️ Draw lines: fsDrawLine(x1, y1, x2, y2)

🔺 Draw triangles (wireframe or filled): fsDrawTriangle(x1, y1, x2, y2, x3, y3, fill)

🖥️ Create windows: fsNewWindow(title, width, height)

🔄 Event loop: fsPollEvents(window, renderCallback)

⚡ Lightweight, no heavy boilerplate — 28 lines for a working triangle demo

Quick Start

Run the demo binary

./FSGL_TriangleDemo. 

Or compile from source
clang++ *.cpp -o FSGL -g -lglfw -lGL -lGLEW -lpthread -lX11
./FSGL

Minimal Example
```cpp
#include "fsgl.hpp"

void render() {
    fsgl::fs::fsClear(0.1f, 0.1f, 0.1f);
    fsgl::fs::fsSetColor(0.0f, 1.0f, 0.0f);
    fsgl::fs::fsDrawTriangle(-0.5f, -0.5,
                              0.5f, -0.5,
                              0.0f, 0.5f, true);
}

int main() {
    if(fsgl::fs::fsInit() != fsgl::FSOK) return -1;

    fsgl::fsWindow window = fsgl::fs::fsNewWindow("FSGL Demo", 800, 600);
    if(!window) return fsgl::FSERR;

    fsgl::fs::fsSetContext(window);
    fsgl::fs::fsInitContext();
    fsgl::fs::fsPollEvents(window, render);
    fsgl::fs::fsDrop();
    return 0;
}
```
Why FSGL?
🛠 Beginner-friendly – focus on graphics, not setup
⚡ Fast iteration – demos run in under 30 lines
💎 Expandable – add shaders, 3D primitives, or custom rendering easily
License

FSGL is released under the Apache License 2.0.

You can use, modify, and distribute FSGL freely
Must include the original copyright notice
Provides explicit patent protection
No warranty provided

See the LICENSE file for full details.
