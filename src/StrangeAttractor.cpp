#include "raylib.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include <iostream>
#include <vector>  //for STL vector

#include "raygui.h"
#include "rlgl.h"

using namespace std;

// Strange attractor discovered by Clifford A. Pickover
// An iterative system with x=sin(a*y)-z*cos(b*x), y=z*sin(c*x)-cos(d*y), z=e*sin(x)
// Vary the top sliders to demonstrate that the shape is an attractor: various starting states don't affect the shape
// Vary the bottom sliders to change the five parameters (a,b,c,d,e) and explore a wide variety of attractor shapes!
// K Points=1000s of points to plot.  More points fill in whispy objects.
// Inspiration by Roger T. Stevens, Fractal Programming in C
// Super handy RayLib graphics library and RayGUI immediate mode GUI by Ramon Santamaria
// Code by Eric J. Jenislawski

void DrawPoint3D(Vector3 pos, Color color) {
  // if (rlCheckBufferLimit(8)) rlglDraw();
  rlPushMatrix();
  rlTranslatef(pos.x, pos.y, pos.z);
  rlBegin(RL_LINES);
  rlColor4ub(color.r, color.g, color.b, 64);
  rlVertex3f(0.0, 0.0, 0.0);
  rlVertex3f(0.02, 0.0, 0.02);
  rlVertex3f(0.0, 0.0, 0.0);
  rlVertex3f(0.02, 0.02, 0.0);
  rlEnd();

  rlPopMatrix();

  return;
}

Color colorize(int i) {
  unsigned char r, g, b;
  i /= 10;
  r = (i / (64 * 64)) % 255;
  g = 2 * ((i / 128) % 128);
  b = 128 + i % 128;

  return {r, g, b, 128};
}

int main() {
  // Initialize Raylib
  InitWindow(800, 800, "Pickover Attractor");
  SetWindowPosition(500, 50);

  Camera camera = {0};
  camera.position = {30.0, 10.0, 30.0};
  camera.target = {0.0, 0.0, 0.0};
  camera.up = {0.0f, 1.0f, 0.0f};
  camera.fovy = 60.0f;
  camera.projection = CAMERA_PERSPECTIVE;
  UpdateCamera(&camera, CAMERA_FREE);
  SetTargetFPS(30);

  unsigned numPoints = 300000;
  float npf = 30.0f;
  float ix = 0.0, iy = 1.0, iz = 0.0;
  float p[5] = {2.24, 0.43, -0.65, -2.43, 1.0};
  vector<Vector3> vec_list;
  bool cycle = false, check[5] = {false};
  float cycle_inc = 0.005;

  while (!WindowShouldClose()) {
    // Update
    // The GUI
    GuiSlider({650, 10, 100, 20}, "K points", "", &npf, 30, 800);
    numPoints = (unsigned)1000 * npf;

    GuiSlider({150, 10, 100, 20}, "X", "", &ix, -40.0, 40.0);
    GuiSlider({300, 10, 100, 20}, "Y", "", &iy, -40.0, 40.0);
    GuiSlider({450, 10, 100, 20}, "Z", "", &iz, -40.0, 40.0);

    GuiSlider({15, 750, 100, 20}, "A", "", &p[0], -5.0, 5.0);
    GuiSlider({165, 750, 100, 20}, "B", "", &p[1], -5.0, 5.0);
    GuiSlider({315, 750, 100, 20}, "C", "", &p[2], -5.0, 5.0);
    GuiSlider({465, 750, 100, 20}, "D", "", &p[3], -5.0, 5.0);
    GuiSlider({615, 750, 100, 20}, "E", "", &p[4], -5.0, 5.0);

    GuiLabel({150, 40, 100, 20}, "Cycle Parameters");
    cycle = GuiCheckBox({250, 40, 20, 20}, "", &cycle);

    for (unsigned i = 0; i < 5; i++) {
      check[i] = GuiCheckBox({15.0f + (150 * i), 725, 20, 20}, "", &check[i]);
    }

    vec_list.clear();
    Vector3 pos = {ix, iy, iz};
    float tempx = 0.0, tempy = 0.0, tempz = 0.0;
    for (unsigned i = 0; i < numPoints; i++) {
      vec_list.push_back(pos);
      // The attractor equations
      tempx = sin(p[0] * pos.y) - pos.z * cos(p[1] * pos.x);
      pos.y = pos.z * sin(p[2] * pos.x) - cos(p[3] * pos.y);
      pos.z = p[4] * sin(pos.x);
      pos.x = tempx;
    }

    if (cycle) {
      unsigned i = 0;
      while (i < 5) {
        if (check[i] == true) {
          p[i] += cycle_inc;
          if (p[i] > 5.0) {
            p[i] = -5.0;
            unsigned j = i;
            if (j < 4) {
              j++;
              while (j < 5) {
                if (check[j] == true) {
                  p[j] += cycle_inc;
                }
                j++;
              }
            }
          }
          break;
        }
        i++;
      }
    }

    // Draw
    BeginDrawing();
    ClearBackground(BLACK);
    BeginMode3D(camera);

    for (unsigned i = 0; i < numPoints - 1; i++) {
      DrawPoint3D(vec_list[i], BLUE);
    }

    // UpdateCamera(&camera);
    EndMode3D();
    DrawFPS(10, 10);
    EndDrawing();
  }

  return 0;
}
