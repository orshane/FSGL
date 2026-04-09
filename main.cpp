#include "fsgl.hpp"
#include <cmath>

struct Vec3 { float x,y,z; };

// cube vertices
Vec3 cube[8] = {
    {-0.5,-0.5,-0.5},{0.5,-0.5,-0.5},
    {0.5,0.5,-0.5},{-0.5,0.5,-0.5},
    {-0.5,-0.5,0.5},{0.5,-0.5,0.5},
    {0.5,0.5,0.5},{-0.5,0.5,0.5}
};

int edges[12][2] = {
    {0,1},{1,2},{2,3},{3,0},
    {4,5},{5,6},{6,7},{7,4},
    {0,4},{1,5},{2,6},{3,7}
};

float angle = 0.0f;

// rotate in 3D
Vec3 rotate(Vec3 v) {
    float c = cos(angle), s = sin(angle);

    // Y rotation
    float x = v.x * c + v.z * s;
    float z = -v.x * s + v.z * c;

    // X rotation
    float y = v.y * c - z * s;
    z = v.y * s + z * c;

    return {x,y,z};
}

void DrawSmoothGlowLine(float x1,float y1,float x2,float y2) {
    int layers = 20;          // more layers = smoother glow
    float maxWidth = 12.0f;   // glow radius

    for(int i = layers; i > 0; --i) {
        float t = (float)i / layers;

        // smooth falloff (quadratic)
        float alpha = t * t * 0.08f;

        fsgl::fs::fsSetColor(0.0f, 1.0f, 0.0f);
        fsgl::fs::fsSetAlpha(alpha);
        fsgl::fs::fsSetLineWidth(t * maxWidth);

        fsgl::fs::fsDrawLine(x1,y1,x2,y2);
    }

    // bright core
    fsgl::fs::fsSetAlpha(1.0f);
    fsgl::fs::fsSetLineWidth(2.0f);
    fsgl::fs::fsDrawLine(x1,y1,x2,y2);
}

void render() {
    fsgl::fs::fsClear(0.0f,0.0f,0.0f);

    Vec3 r[8];
    for(int i=0;i<8;i++)
        r[i] = rotate(cube[i]);

    float scale = 0.7f;

    for(int i=0;i<12;i++){
        Vec3 p1 = r[edges[i][0]];
        Vec3 p2 = r[edges[i][1]];

        DrawSmoothGlowLine(
            p1.x * scale, p1.y * scale,
            p2.x * scale, p2.y * scale
        );
    }

    angle += 0.01f;
}

int main() {
    if(fsgl::fs::fsInit() != fsgl::FSOK) return -1;

    fsgl::fsWindow window = fsgl::fs::fsNewWindow("FSGL Glow Cube", 800, 600);
    if(!window) return -1;

    fsgl::fs::fsSetContext(window);
    fsgl::fs::fsInitContext();

    fsgl::fs::fsEnableBlend();
    fsgl::fs::fsSetBlendAdditive();

    fsgl::fs::fsPollEvents(window, render);

    fsgl::fs::fsDrop();
    return 0;
}