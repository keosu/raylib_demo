#include <raylib.h>

//  _canvas.AddBall(mass: 99, radius: 25, x: Center, y: Middle, vx: -0.0, vy: 0.0, Color.YELLOW, "Sun");
//   _canvas.AddBall(mass: 0.01, radius: 01, x: Center + 100 + 000, y: Middle, vx: -0.06, vy: +1.80, Color.DARKBROWN,
//   "Mercury"); _canvas.AddBall(mass: 0.02, radius: 03, x: Center + 100 + 050, y: Middle, vx: -0.36, vy: +1.35,
//   Color.GREEN    , "Venus"); _canvas.AddBall(mass: 0.02, radius: 04, x: Center + 100 + 100, y: Middle, vx: -0.36, vy:
//   +1.10, Color.BLUE     , "Earth"); _canvas.AddBall(mass: 0.01, radius: 02, x: Center + 100 + 150, y: Middle, vx:
//   -0.36, vy: +1.00, Color.RED      , "Mars"); _canvas.AddBall(mass: 0.09, radius: 09, x: Center + 100 + 300, y:
//   Middle, vx: -0.20, vy: +0.90, Color.MAROON   , "Jupiter"); _canvas.AddBall(mass: 0.08, radius: 08, x: Center + 100
//   + 400, y: Middle, vx: -0.15, vy: +0.70, Color.SKYBLUE  , "Saturn"); _canvas.AddBall(mass: 0.06, radius: 06, x:
//   Center + 100 + 500, y: Middle, vx: -0.13, vy: +0.60, Color.BROWN    , "Uranus"); _canvas.AddBall(mass: 0.05,
//   radius: 05, x: Center + 100 + 600, y: Middle, vx: -0.08, vy: +0.48, Color.LIME     , "Neptune");

int main(void) {
  // Initialization
  //--------------------------------------------------------------------------------------
  const int screenWidth = 1800;
  const int screenHeight = 900;

  InitWindow(screenWidth, screenHeight, "Solar System");

  // Define the camera to look into our 3d world
  Camera camera = {0};
  camera.position = {0.0f, 0.0f, 20.0f};
  camera.target = {0.0f, 0.0f, 0.0f};
  camera.up = {0.0f, 1.0f, 0.0f};
  camera.fovy = 70.0f;
  camera.projection = CAMERA_PERSPECTIVE;

  SetTargetFPS(60);  // Set our game to run at 60 frames-per-second
  //--------------------------------------------------------------------------------------


  // Main game loop
  while (!WindowShouldClose()) {
    UpdateCamera(&camera, CAMERA_FREE);
    BeginDrawing();

    ClearBackground(RAYWHITE);

    BeginMode3D(camera);

    DrawSphere({0.0f, 0.0f, 0}, 1.0f, YELLOW);
    DrawSphere({2 + 0.0f, 0.0f, 0}, 0.2f, DARKBROWN);
    DrawSphere({2 + 1.0f, 0.0f, 0}, 0.2f, GREEN);
    DrawSphere({2 + 2.0f, 0.0f, 0}, 0.2f, BLUE);
    DrawSphere({2 + 3.0f, 0.0f, 0}, 0.2f, RED);
    DrawSphere({2 + 5.0f, 0.0f, 0}, 0.4f, MAROON);
    DrawSphere({2 + 6.0f, 0.0f, 0}, 0.4f, SKYBLUE);
    DrawSphere({2 + 7.0f, 0.0f, 0}, 0.3f, BROWN);
    DrawSphere({2 + 8.0f, 0.0f, 0}, 0.3f, LIME);

    DrawCircle3D({0,0,0},2,{0,0,1},0,BLUE);
    DrawCircle3D({0,0,0},3,{0,0,0},0,BLUE);
    DrawCircle3D({0,0,0},4,{0,0,0},0,BLUE);
    DrawCircle3D({0,0,0},5,{0,0,0},0,BLUE);
    DrawCircle3D({0,0,0},7,{0,0,0},0,BLUE);
    DrawCircle3D({0,0,0},8,{0,0,0},0,BLUE);
    DrawCircle3D({0,0,0},9,{0,0,0},0,BLUE);
    DrawCircle3D({0,0,0},10,{0,0,0},0,BLUE);

    // DrawSphereWires({1.0f, 0.0f, 2.0f}, 2.0f, 16, 16, LIME);

    DrawGrid(20, 1.0f);  // Draw a grid
    // UpdateCamera(&camera);
    EndMode3D();

    DrawFPS(10, 10);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}