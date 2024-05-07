#include <raylib.h>

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void) {
  // Initialization
  //--------------------------------------------------------------------------------------
  const int screenWidth = 800;
  const int screenHeight = 450;

  InitWindow(screenWidth, screenHeight, "raylib [models] example - geometric shapes");

  // Define the camera to look into our 3d world
  Camera camera = {0};
  camera.position = {0.0f, 10.0f, 10.0f};
  camera.target = {0.0f, 0.0f, 0.0f};
  camera.up = {0.0f, 1.0f, 0.0f};
  camera.fovy = 45.0f;
  camera.projection = CAMERA_PERSPECTIVE;

  SetTargetFPS(60);  // Set our game to run at 60 frames-per-second
  //--------------------------------------------------------------------------------------

  // Main game loop
  while (!WindowShouldClose()) {
    BeginDrawing();

    ClearBackground(RAYWHITE);

    BeginMode3D(camera);

    DrawCube({-4.0f, 0.0f, 2.0f}, 2.0f, 5.0f, 2.0f, RED);
    DrawCubeWires({-4.0f, 0.0f, 2.0f}, 2.0f, 5.0f, 2.0f, GOLD);
    DrawCubeWires({-4.0f, 0.0f, -2.0f}, 3.0f, 6.0f, 2.0f, MAROON);

    DrawSphere({-1.0f, 0.0f, -2.0f}, 1.0f, GREEN);
    DrawSphereWires({1.0f, 0.0f, 2.0f}, 2.0f, 16, 16, LIME);

    DrawCylinder({4.0f, 0.0f, -2.0f}, 1.0f, 2.0f, 3.0f, 4, SKYBLUE);
    DrawCylinderWires({4.0f, 0.0f, -2.0f}, 1.0f, 2.0f, 3.0f, 4, DARKBLUE);
    DrawCylinderWires({4.5f, -1.0f, 2.0f}, 1.0f, 1.0f, 2.0f, 6, BROWN);

    DrawCylinder({1.0f, 0.0f, -4.0f}, 0.0f, 1.5f, 3.0f, 8, GOLD);
    DrawCylinderWires({1.0f, 0.0f, -4.0f}, 0.0f, 1.5f, 3.0f, 8, PINK);

    DrawCapsule({-3.0f, 1.5f, -4.0f}, {-4.0f, -1.0f, -4.0f}, 1.2f, 8, 8, VIOLET);
    DrawCapsuleWires({-3.0f, 1.5f, -4.0f}, {-4.0f, -1.0f, -4.0f}, 1.2f, 8, 8, PURPLE);

    DrawGrid(10, 1.0f);  // Draw a grid

    EndMode3D();

    DrawFPS(10, 10);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}