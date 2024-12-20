#include <iostream>

#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"

#include "core.h"
#include "networking.h"
#include "sceneController2D.h"




int main()
{
	int screenWidth = 960;
	int screenHeight = 540;

	NATIVE_ONLY(SetConfigFlags(FLAG_WINDOW_RESIZABLE););
	InitWindow(screenWidth, screenHeight, "Stratego");

	NetworkManager netManager{ "http://localhost:3000" };
	SceneController2D sceneManager{};
	
	SetTargetFPS(60);

	while (!WindowShouldClose())  
	{
		char buffer[3] = {0, 1, 2};
		netManager.sendBinary("test", buffer, 3);
		
		BeginDrawing();

		sceneManager.beginUpdate(&screenWidth, &screenHeight);

		// Draw the 3d grid, rotated 90 degrees and centered around 0,0 
		// just so we have something in the XY plane
		rlPushMatrix();
		rlTranslatef(0, 25 * 50, 0);
		rlRotatef(90, 1, 0, 0);
		DrawGrid(100, 50);
		rlPopMatrix();

		// Draw a reference circle
		DrawCircle(GetScreenWidth() / 2, GetScreenHeight() / 2, 50, MAROON);
		
		sceneManager.endUpdate();

		// Draw mouse reference
		//Vector2 mousePos = GetWorldToScreen2D(GetMousePosition(), camera)
		DrawCircleV(GetMousePosition(), 4, DARKGRAY);
		DrawTextEx(GetFontDefault(), TextFormat("[%i, %i]", GetMouseX(), GetMouseY()), Vector2Add(GetMousePosition(), { -44, -24 }), 20, 2, BLACK);


		EndDrawing();
	}

	CloseWindow();        // Close window and OpenGL context
	return 0;
}