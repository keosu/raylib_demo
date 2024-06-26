#include "raylib.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include <cmath>  //for atan2
#include <deque>  //for deque
#include <iostream>

#include "raygui.h"

using namespace std;

struct line {
  Vector2 startpt;
  Vector2 endpt;
  Color color;
};

Vector2 Vector2Rotatex(Vector2 vec, float angle) {
  float len = Vector2Length(vec);
  float theta = atan2(vec.y, vec.x);
  Vector2 rval = {len * cos(theta + angle), len * sin(theta + angle)};
  return rval;
}

int main() {
  // Initialize Raylib
  InitWindow(1000, 900, "Koch Curve");
  SetWindowPosition(500, 50);

  Camera camera = {0};
  camera.position = {10.0, 0.0, 10.0};
  camera.target = {0.0, 0.0, 0.0};
  camera.up = {0.0f, 1.0f, 0.0f};
  camera.fovy = 60.0f;
  camera.projection = CAMERA_PERSPECTIVE;
  UpdateCamera(&camera, CAMERA_FREE);
  SetTargetFPS(30);

  line linetemp, linenew;
  Vector2 midpt1, midpt2;
  unsigned vsize, counter = 0;
  float levels = 2, oldlevels = 2;
  bool draw_analysis = false;

  deque<line> LineList;
  // Draw our initial figure
  line initiator = {{200, 200}, {800, 200}, BLUE};
  LineList.push_back(initiator);
  initiator = {{800, 200}, {500, float(200 + 600 * sqrt(3.0) / 2.0)}, BLUE};
  LineList.push_back(initiator);
  initiator = {{500, float(200 + 600 * sqrt(3.0) / 2.0)}, {200, 200}, BLUE};
  LineList.push_back(initiator);

  while (!WindowShouldClose()) {
    // Update

    GuiSlider({900, 50, 60, 40}, "Levels", "", &levels, 1.0, 9.0);
    if (levels != oldlevels) {
      counter = 0;
      oldlevels = levels;
      LineList.clear();
      initiator = {{200, 200}, {800, 200}, BLUE};
      LineList.push_back(initiator);
      initiator = {{800, 200}, {500, float(200 + 600 * sqrt(3.0) / 2.0)}, BLUE};
      LineList.push_back(initiator);
      initiator = {{500, float(200 + 600 * sqrt(3.0) / 2.0)}, {200, 200}, BLUE};
      LineList.push_back(initiator);
    }

    GuiCheckBox({900, 100, 20, 20}, "Analysis", &draw_analysis);

    counter++;
    if (counter < levels) {
      vsize = LineList.size();  // Need to capture LineList.size() here to make one pass through existing deque items
                                // and not new ones we add this cycle
      for (unsigned i = 0; i < vsize; i++) {
        linetemp = LineList.front();
        LineList.pop_front();
        midpt1 = Vector2Add(linetemp.startpt, Vector2Scale(Vector2Subtract(linetemp.endpt, linetemp.startpt),
                                                           1.0 / 3.0));  // Trisect the line
        midpt2 =
            Vector2Add(linetemp.startpt, Vector2Scale(Vector2Subtract(linetemp.endpt, linetemp.startpt), 2.0 / 3.0));
        linenew = {linetemp.startpt, midpt1, BLUE};  // Create 4 new lines in place of previous line
        LineList.push_back(linenew);
        linenew = {midpt2, linetemp.endpt, BLUE};
        LineList.push_back(linenew);
        linenew = {midpt1, Vector2Add(midpt1, Vector2Rotatex(Vector2Subtract(midpt1, linetemp.startpt), -60 * DEG2RAD)),
                   BLUE};
        LineList.push_back(linenew);
        linenew = {linenew.endpt, midpt2, BLUE};
        LineList.push_back(linenew);
      }
    }

    // Draw

    BeginDrawing();
    ClearBackground(BLACK);

    for (unsigned i = 0; i < LineList.size(); i++) {
      DrawLineV(LineList[i].startpt, LineList[i].endpt, LineList[i].color);
    }
    if (draw_analysis) {
      DrawCircleLines(500, 371, 300 * 1.1547, MAROON);
      float perimeter = 3 * 600 * pow(4.0 / 3.0, levels - 1);
      float area_orig = (sqrt(3.0) / 4.0) * 600.0 * 600.0;
      float area = (area_orig / 5.0) * (8.0 - 3.0 * pow(4.0 / 9.0, levels - 1));
      // cout<<"Perimeter="<<perimeter<<" Area="<<area<<" Area/Perimeter"<<(area/perimeter)<<endl;
      char str[200];
      sprintf(str, "Perimeter=%.0f  Area=%.0f  Area/Perimeter=%.0f", perimeter, area, (area / perimeter));
      DrawText(str, 50, 800, 20, RED);
      float area_limit = 2 * 600 * 600 * sqrt(3.0) / 5.0;
      sprintf(str, "Limit of Perimeter = infinity.  Limit of Area=%.0f  Limit of area/perimeter=0", area_limit);
      DrawText(str, 50, 840, 20, RED);
    }

    DrawFPS(10, 10);
    EndDrawing();
  }

  return 0;
}
