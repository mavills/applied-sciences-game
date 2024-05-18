#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <math.h>
#include <stdio.h>

float max(float a, float b) { return a > b ? a : b; }
float min(float a, float b) { return a < b ? a : b; }

void DrawCell(int x, int y, int z, Mesh mesh, Material material) {
  Matrix mat = MatrixTranslate(0.5f, 0.5f, 0.5f);
  // DrawCube((Vector3){0.5f, 0.5f, 0.5f}, 1.0f, 1.0f, 1.0f, RED);
  DrawMesh(mesh, material, MatrixMultiply(MatrixTranslate(x, y, z), mat));
}

int main() {
  // Initialization
  //--------------------------------------------------------------------------------------
  const int screenWidth = 1240;
  const int screenHeight = 720;

  SetConfigFlags(FLAG_MSAA_4X_HINT);
  InitWindow(screenWidth, screenHeight, "applied sciences");

  SetTargetFPS(60); // Set our game to run at 60 frames-per-second
  // DisableCursor();

  // Vector2 startPoint = { 30, 30 };
  // Vector2 endPoint = { (float)screenWidth - 30, (float)screenHeight - 30 };
  // bool moveStartPoint = false;
  // bool moveEndPoint = false;

  // typedef struct Camera3D {
  //     Vector3 position;       // Camera position
  //     Vector3 target;         // Camera target it looks-at
  //     Vector3 up;             // Camera up vector (rotation over its axis)
  //     float fovy;             // Camera field-of-view aperture in Y (degrees)
  //     in perspective, used as near plane width in orthographic int
  //     projection;         // Camera projection: CAMERA_PERSPECTIVE or
  //     CAMERA_ORTHOGRAPHIC
  // } Camera3D;
  Vector3 cameraPos = {50.0f, 50.0f, 50.0f};
  // Vector3 cameraPos = {0.0f, 50.0f, 0.0f};
  Vector3 cameraTarget = {0.0f, 0.0f, 0.0f};
  const Vector3 cameraUp = {0.0f, 1.0f, 0.0f};
  float zoom = 30.0f;
  Camera3D camera = {cameraPos, cameraTarget, cameraUp, zoom,
                     CAMERA_ORTHOGRAPHIC};
  float rotation = 0.0f;
  Vector3 translation = {0.0f, 0.0f, 0.0f};
  double pixelsPerUnit = GetRenderHeight() / zoom;

  Mesh mesh = GenMeshCube(1, 1, 1);
  UploadMesh(&mesh, false);
  Material material = LoadMaterialDefault();
  Texture2D texture = LoadTexture("resources/energy_cell.png");
  SetMaterialTexture(&material, MATERIAL_MAP_DIFFUSE, texture);
  Transform transform = {(Vector3){0, 0, 0}, QuaternionIdentity(),
                         (Vector3){1, 1, 1}};

  //--------------------------------------------------------------------------------------

  // Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    // Update
    //----------------------------------------------------------------------------------
    Vector2 mouseDelta = GetMouseDelta();
    // Camera zoom controls
    zoom -= GetMouseWheelMove() * 15.0f * (zoom / 90.0f);
    zoom = max(5.0f, min(zoom, 50.0f));
    camera.fovy = zoom;

    pixelsPerUnit = GetRenderHeight() / zoom;
    // Camera rotation controls
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
      Vector3 move = {-mouseDelta.y / pixelsPerUnit / 0.6154797086703871, 0.0,
                      mouseDelta.x / pixelsPerUnit};
      move = Vector3RotateByAxisAngle(move, (Vector3){0.0, 1.0, 0.0},
                                      (-rotation - 45.0) / 180 * PI);
      // translation.z += mouseDelta.x * 0.02f * cos(rotation / 180 * PI) -
      //                  mouseDelta.y * 0.02f * sin(rotation / 180 * PI);
      // translation.x += mouseDelta.x * 0.02f * sin(rotation / 180 * PI) +
      //                  mouseDelta.y * 0.02f * cos(rotation / 180 * PI);
      // translation.x -= move.x / sin(PI / 5);
      translation.x -=
          // move.x / sin(50/sqrt(pow(50.0, 2) + pow(50 / sin(PI / 4), 2)));
          move.x;
      translation.z -= move.z;
    }
    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
      rotation += mouseDelta.x * 0.2f;
      if (rotation > 360.0f)
        rotation -= 360.0f;
      if (rotation < 0.0f)
        rotation += 360.0f;
    }

    if (IsKeyPressed('Q'))
      break;
    // if (CheckCollisionPointCircle(mouse, startPoint, 10.0f) &&
    //     IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    //   moveStartPoint = true;
    // else if (CheckCollisionPointCircle(mouse, endPoint, 10.0f) &&
    //          IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    //   moveEndPoint = true;

    // if (moveStartPoint) {
    //   startPoint = mouse;
    //   if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
    //     moveStartPoint = false;
    // }

    // if (moveEndPoint) {
    //   endPoint = mouse;
    //   if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
    //     moveEndPoint = false;
    // }
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

    ClearBackground(RAYWHITE);
    char str[180];
    sprintf(str, "camera fovy: %f\ntrans: (%f %f %f)\nrot: %f, cos %f, sin %f",
            camera.fovy, translation.x, translation.y, translation.z, rotation,
            cos(rotation / 180 * PI), sin(rotation / 180 * PI));
    DrawText(str, 15, 20, 20, GRAY);

    BeginMode3D(camera);
    rlPushMatrix();
    rlRotatef(rotation, 0, 1, 0);
    rlTranslatef(translation.x, 0, translation.z);
    // rlTranslatef(mouseDelta.x *10, 0, mouseDelta.y*10);
    // MatrixTranslate(float x, float y, float z);

    Matrix mat = MatrixTranslate(0.5f, 0.5f, 0.5f);
    // DrawCube((Vector3){0.5f, 0.5f, 0.5f}, 1.0f, 1.0f, 1.0f, RED);
    DrawMesh(mesh, material, mat);
    // MatrixMultiply(MatrixTranslate(0.5f, 0.5f, 0.5f), mat);
    DrawMesh(mesh, material,
             MatrixMultiply(MatrixTranslate(1.0f, 1.0f, 1.0f), mat));

    DrawGrid(200, 1.0f);

    // Draw line Cubic Bezier, in-out interpolation (easing), no control
    // points DrawLineBezier(startPoint, endPoint, 4.0f, BLUE);

    // Draw start-end spline circles with some details
    // DrawCircleV(startPoint,
    //             CheckCollisionPointCircle(mouse, startPoint, 10.0f) ? 14
    //             : 8, moveStartPoint ? RED : BLUE);
    // DrawCircleV(endPoint,
    //             CheckCollisionPointCircle(mouse, endPoint, 10.0f) ? 14 :
    //             8, moveEndPoint ? RED : BLUE);
    rlPopMatrix();
    EndMode3D();

    DrawFPS(0, 0);

    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}
