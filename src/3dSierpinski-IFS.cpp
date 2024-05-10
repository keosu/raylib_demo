#include <cstdlib>  //for RAND
#include <iostream>
#include <vector>  //for STL vector

#include "raylib.h"
#include "raymath.h"

using namespace std;

Vector3 affine_transform(int num) {
  int random_number;
  int counter = 0;
  float rx, ry, rz;
  rx = (float)rand() / RAND_MAX;
  ry = (float)rand() / RAND_MAX;
  rz = (float)rand() / RAND_MAX;
  Vector3 position = {rx, ry, rz};

  while (counter < num) {
    random_number = rand() % 4;
    position = Vector3Scale(position, 0.5);
    if (random_number == 0) {
    }
    if (random_number == 1) position = Vector3Add(position, {0.5, 0.0, 0.0});
    if (random_number == 2) position = Vector3Add(position, {0.0, 0.5, 0.0});
    if (random_number == 3) position = Vector3Add(position, {0.0, 0.0, 0.5});

    counter++;
  }

  return position;
}

int main() {
  // Initialize Raylib
  InitWindow(800, 800, "Sierpinski Triangles");
  SetWindowPosition(500, 50);

  Camera camera = {0};
  camera.position = {100.0, 50.0, 100.0};
  camera.target = {0.0, 0.0, 0.0};
  camera.up = {0.0f, 1.0f, 0.0f};
  camera.fovy = 60.0f;
  camera.projection = CAMERA_PERSPECTIVE;
  SetTargetFPS(60);

  unsigned numPoints = 100000;  // 1000000;  // number of points to plot
  int afn = 20;                 // number of affine transforms to apply to each random initial point
  float scale = 100.0;
  float cubeScale = 0.1;
  Vector3 offset = {50, 0, 50};

  vector<Vector3> vec_list;

  for (unsigned i = 0; i < numPoints; i++) {
    vec_list.push_back(Vector3Subtract(Vector3Scale(affine_transform(afn), scale), offset));
  }

  while (!WindowShouldClose()) {
    UpdateCamera(&camera, CAMERA_ORBITAL);

    BeginDrawing();
    ClearBackground(BLACK);

    BeginMode3D(camera);
    DrawGrid(100, 1.0f);

    for (unsigned i = 0; i < numPoints; i++) {
      DrawPoint3D(vec_list[i], RED);
    }

    EndMode3D();
    DrawFPS(10, 10);
    EndDrawing();
  }

  return 0;
}
