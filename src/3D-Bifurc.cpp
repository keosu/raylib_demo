#include "raylib.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include <iostream>
#include <vector>  //for STL vector

#include "raygui.h"
#include "rlgl.h"

using namespace std;

// Bifurcation Diagram of the logistic map, x=kx(1-x)
// Top slider controls initial value of x.  Equation is iterated without plotting until split value is reached, then
// points are plotted. Discarding the points below split value allows one to see better the periodic values certain
// values of K achieve. For the classic diagram, keep the split value >64.  You can change j to max of 4 in for-loop to
// increase FPS and decrease numPoints to around 300. For another interesting view, lower the split level to zero, keep
// j as is, zoom out on the bifurcated part and explore what happens when the starting values of x are close to zero or
// one.  Slightly increase the split value to add or subtract "waves" emanating from the right side. There's really no
// need to do this in 3D.  It just gave me easy zooming and panning.

void DrawPoint3D(Vector3 pos, Color color) {
  //   if (rlCheckBufferLimit(8)) rlglDraw();
  rlPushMatrix();
  rlTranslatef(pos.x, pos.y, pos.z);
  rlBegin(RL_LINES);
  rlColor4ub(color.r, color.g, color.b, 255);
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
  Color color;
  i /= 10;
  r = (i / (64 * 64)) % 255;
  g = 2 * ((i / 128) % 128);
  b = 128 + i % 128;

  return {r, g, b, 128};
}

int main() {
  // Initialize Raylib
  InitWindow(800, 800, "Bifurcation Diagram");
  SetWindowPosition(500, 50);

  Camera camera = {0};
  camera.position = {30.0, 10.0, 30.0};
  camera.target = {0.0, 0.0, 0.0};
  camera.up = {0.0f, 1.0f, 0.0f};
  camera.fovy = 60.0f;
  camera.projection = CAMERA_PERSPECTIVE;
  SetTargetFPS(30);

  vector<Vector3> vec_list;
  unsigned numPoints = 1000;
  float split_val = 64;
  Vector3 pos = {0.0, 0.5, 0.0};
  float start_val = 0.5;

  while (!WindowShouldClose()) {
    // Update
    // The GUI
    GuiSlider({150, 10, 500, 20}, "X", "", &start_val, 0, 1.0);
    GuiSlider({150, 30, 500, 20}, "Split", "", &split_val, 0, 100);

    vec_list.clear();
    for (float j = 0.0; j < 8.0; j += 0.002) {
      pos.y = start_val;
      pos.x = j;
      for (unsigned i = 0; i < split_val; i++) {
        pos.y = pos.x * pos.y * (1.0 - pos.y);
      }
      for (unsigned i = split_val; i < numPoints; i++) {
        pos.y = pos.x * pos.y * (1.0 - pos.y);
        vec_list.push_back(pos);
      }
    }

    // UpdateCamera(&camera, CAMERA_FREE);

    // Draw
    BeginDrawing();
    ClearBackground(BLACK);
    BeginMode3D(camera);

    for (unsigned i = 0; i < vec_list.size(); i++) {
      DrawPoint3D(Vector3Scale(vec_list[i], 10.0), BLUE);
    }

    EndMode3D();
    DrawFPS(10, 10);
    EndDrawing();
  }

  return 0;
}
