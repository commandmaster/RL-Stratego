#include <iostream>

#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"

#include "core.h"
#include "networking.h"
#include "sceneController2D.h"
#include "gameGrid.h"




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

		
		constexpr int cellWidth = 100;
		constexpr int cellHeight = 100;

		for (int i = 0; i < Grid::COLUMNS; ++i)
		{
			for (int j = 0; j < Grid::ROWS; ++j)
			{
				Color borderColor = GRAY;

				if (Grid::boardConfiguration[i + j * Grid::COLUMNS] == 1)
				{
					DrawRectangle(i * cellWidth + GetScreenWidth() / 2 - cellWidth * Grid::COLUMNS / 2, 
								  j * cellHeight + GetScreenHeight() / 2 - cellHeight * Grid::ROWS / 2, 
								  cellWidth, cellHeight, BLUE);
				}
				else
				{
					DrawRectangleLines(i * cellWidth + GetScreenWidth() / 2 - cellWidth * Grid::COLUMNS / 2, 
									   j * cellHeight + GetScreenHeight() / 2 - cellHeight * Grid::ROWS / 2, 
									   cellWidth, cellHeight, borderColor);
				}
			}
		}


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