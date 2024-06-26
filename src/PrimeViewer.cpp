#include <cmath>  // For floor, sqrt
#include <iostream>
#include <vector>  // For vector

#include "raylib.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

// Four ways to visualize the distribution of primes under 1,000,000
// Press 1, 2, 3 or 4 to change views
// Mode 1 = 3D view of distribution of primes in a 100x100x100 cube
// Mode 2 = 3D Sieve of Erathosthenes, showing primes in white, and eliminated non-primes in color
//           The color represents the lowest prime factor which divides the non-prime.
//           Use the sliders to display only the factors you want, and the check box to draw primes or not
// Mode 3 = Factor wheel, using sectors.  Google "Wheel Factorization" for details.
// Mode 4 = Factor wheel, using points, which displays better when using a high factor base for the wheel
// Code and concepts by Eric J. Jenislawski.
// Thanks to Ramon Santamaria for the great RayLib and RayGUI libraries

using namespace std;

Color cubecolor(int f, unsigned char transparency) {
  // separating color vals by 8, and using range 128-248, that leaves us: 15*15*15=3375 distinct colors
  Color retcol;
  unsigned char red, green, blue;
  blue = 120 + (f % 15) * 8;
  red = 120 + ((f / 15) % 15) * 8;
  green = 120 + ((f / 255) % 15) * 8;
  retcol = {red, green, blue, transparency};
  // cout<<"f="<<f<<" RGB"<<red<<" "<<green<<" "<<blue<<endl;
  return retcol;
}

int main() {
  unsigned int maxval = 1000000;
  unsigned int bound;
  int viewmode = 1;
  float transparency = 64, lowerval = 2, upperval = 1000;
  float factorbase = 60;
  int fbase = 60;
  bool boolDrawPrimes = true;
  Vector3 position = {0.0, 0.0, 0.0};
  Vector3 posOffset = {50, 0, 50};
  Vector3 unitV = {1.0, 1.0, 1.0};
  Color cubecol = {255, 255, 255, 32};
  vector<unsigned int> plist(maxval, 1);
  cout << "Array initialized" << endl;

  for (unsigned i = 2; i * i < maxval; i++) {
    if (plist[i] == 1) {
      bound = maxval / i;
      for (unsigned x = bound; x >= i; x--) {
        if (plist[x] == 1) {
          plist[x * i] = i;
        }
      }
    }
  }
  cout << "Primes to " << maxval << " sieved." << endl;

  // Init raylib
  InitWindow(1000, 1000, "Prime viewer");
  SetWindowPosition(400, 100);

  Camera camera = {0};
  camera.position = {1.0, 0.0, 150.0};
  camera.target = {0.0, 0.0, 0.0};
  camera.up = {0.0f, 1.0f, 0.0f};
  camera.fovy = 60.0f;
  camera.projection = CAMERA_PERSPECTIVE;
  SetTargetFPS(60);

  // LOOP

  while (!WindowShouldClose()) {
    UpdateCamera(&camera, CAMERA_THIRD_PERSON);
    // Process any input here
    if (IsKeyPressed(KEY_ONE)) {
      viewmode = 1;
    }
    if (IsKeyPressed(KEY_TWO)) {
      viewmode = 2;
    }
    if (IsKeyPressed(KEY_THREE)) {
      viewmode = 3;
    }
    if (IsKeyPressed(KEY_FOUR)) {
      viewmode = 4;
    }

    // 3D cube, only primes
    if (viewmode == 1) {
      BeginDrawing();
      ClearBackground(BLACK);
      BeginMode3D(camera);
      DrawGrid(100, 1.0f);
      // DrawGizmo({0, 0, 0});

      for (unsigned i = 2; i < maxval; i++) {
        if (plist[i] == 1) {
          position = {float(i % 100), float((i / 100) % 100), float((i / 10000) % 100)};
          DrawCubeV(Vector3Subtract(position, posOffset), unitV, cubecol);
        }
      }
 
      EndMode3D();
      DrawFPS(10, 10);
      EndDrawing();
    }

    // 3D cube, all numbers by factor
    if (viewmode == 2) {
      BeginDrawing();
      ClearBackground(BLACK);
      BeginMode3D(camera);
      DrawGrid(100, 1.0f);
      // DrawGizmo({0, 0, 0});

      for (unsigned i = 2; i < maxval; i++) {
        if ((plist[i] == 1) && (boolDrawPrimes)) {
          position = {float(i % 100), float((i / 100) % 100), float((i / 10000) % 100)};
          DrawCubeV(Vector3Subtract(position, posOffset), unitV, RAYWHITE);
        }
        if ((plist[i] > lowerval) && (plist[i] < upperval)) {
          position = {float(i % 100), float((i / 100) % 100), float((i / 10000) % 100)};
          DrawCubeV(Vector3Subtract(position, posOffset), unitV, cubecolor(plist[i], transparency));
        }
      }

      EndMode3D();
      GuiSlider({200, 10, 100, 10}, "Transparency", "", &transparency, 16, 255);
      GuiCheckBox({350, 10, 10, 10}, "Draw Primes", &boolDrawPrimes);
      GuiSlider({200, 30, 400, 10}, "Draw factors above:", "", &lowerval, 2, 1000);
      GuiSlider({200, 50, 400, 10}, "Draw factors below:", "", &upperval, 2, 1000);
      DrawFPS(10, 10);
      EndDrawing();
    }

    // Sector wheel
    if (viewmode == 3) {
      float inrad, outrad;
      int startangle, endangle;
      Color segcolor = WHITE;
      BeginDrawing();
      ClearBackground(BLACK);

      for (unsigned i = 2; i < 5000; i++) {
        if (plist[i] == 1) {
          // segcolor={16*(i%15),16*(i%15),16*(i%15),64};
          inrad = 30 + floor((i + 30) / 30) * 3;
          outrad = inrad + 3;
          startangle = (360 / factorbase) * (i % fbase);
          endangle = startangle + (360 / factorbase) - 1;
          DrawRing({500, 500}, inrad, outrad, startangle, endangle, 20, segcolor);
          // cout<<"i="<<i<<" inner rad="<<inrad<<" outer rad="<<outrad<<" Start angle="<<startangle<<" End
          // angle="<<endangle<<endl;
        }
      }

      GuiSpinner({5, 30, 100, 40}, "spinner",&fbase, 2, 180,true);
      factorbase = (float)fbase;
      DrawFPS(10, 10);
      EndDrawing();
    }

    // Point wheel
    if (viewmode == 4) {
      // cin>>factorbase;
      float inrad, startangle;
      BeginDrawing();
      ClearBackground(BLACK);

      for (unsigned i = 2; i < 50000; i++) {
        if (plist[i] == 1) {
          // segcolor={16*(i%15),16*(i%15),16*(i%15),64};
          inrad = 30 + floor((i + 30) / 30);
          startangle = (3.14159 * 2.0 / factorbase) * (float)(i % fbase);
          DrawPixel(inrad * cos(startangle) + 500, inrad * sin(startangle) + 500, WHITE);
          // cout<<"i="<<i<<" inner rad="<<inrad<<" Start angle="<<startangle<<endl;
        }
      }

      GuiSpinner({5, 30, 100, 40}, "spinner", &fbase, 2, 100, false);
      factorbase = (float)fbase;

      DrawFPS(10, 10);
      EndDrawing();
    }

  }  // End LOOP
}
