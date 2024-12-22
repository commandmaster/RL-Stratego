#include <iostream>
#include <memory>

#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"

#include "core.h"
#include "networking.h"
#include "sceneController2D.h"
#include "gameGrid.h"
#include "scene.h"

static void drawGameBoard(const Grid& grid, int cellWidth, int cellHeight)
{
	for (int i = 0; i < Grid::COLUMNS; ++i)
		{
			for (int j = 0; j < Grid::ROWS; ++j)
			{
				Color borderColor = GRAY;

				if (grid.board[i + j * Grid::COLUMNS] == 1)
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
}


int main()
{
	int screenWidth = 960;
	int screenHeight = 540;

	constexpr int LOBBY_MODE = 0;
	constexpr int PLACE_MODE = 1;
	constexpr int PLAY_MODE = 2;

	int gameState = LOBBY_MODE;
	Grid gameGrid{
		{
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		
			2, 2, 1, 1, 2, 2, 1, 1, 2, 2,		
			2, 2, 1, 1, 2, 2, 1, 1, 2, 2,		
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0	
		}
	};

	NATIVE_ONLY(SetConfigFlags(FLAG_WINDOW_RESIZABLE););
	InitWindow(screenWidth, screenHeight, "Stratego");

	auto& netManager = NetworkManager::getInstance();
	SceneController2D sceneManager{};
	
	SetTargetFPS(60);

	auto lobbyScene = std::make_shared<Lobby>(screenWidth, screenHeight, netManager);
	sceneManager.instantiateScene(lobbyScene);

	while (!WindowShouldClose())  
	{
		BeginDrawing();

		sceneManager.beginUpdate(&screenWidth, &screenHeight);

		if (gameState == PLACE_MODE || gameState == PLAY_MODE)
		{
			constexpr int cellWidth = 100;
			constexpr int cellHeight = 100;

			drawGameBoard(gameGrid, cellWidth, cellHeight);
		}
		else if (gameState == LOBBY_MODE)
		{

		}
		
		sceneManager.endUpdate();

		// Draw mouse reference
		//Vector2 mousePos = GetWorldToScreen2D(GetMousePosition(), camera)
		// DrawCircleV(GetMousePosition(), 4, DARKGRAY);
		// DrawTextEx(GetFontDefault(), TextFormat("[%i, %i]", GetMouseX(), GetMouseY()), Vector2Add(GetMousePosition(), { -44, -24 }), 20, 2, BLACK);


		EndDrawing();
	}

	CloseWindow();        // Close window and OpenGL context
	return 0;
}