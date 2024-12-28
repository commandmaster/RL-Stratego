#include <iostream>
#include <memory>

#define RAYGUI_IMPLEMENTATION
#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"

#include "core.h"
#include "networking.h"
#include "sceneController2D.h"
#include "gameGrid.h"
#include "scene.h"


int main()
{
	int screenWidth = 960;
	int screenHeight = 540;

	constexpr int LOBBY_MODE = 0;
	constexpr int GAME_MODE = 1;

	int gameState = LOBBY_MODE;

	NATIVE_ONLY(SetConfigFlags(FLAG_WINDOW_RESIZABLE););
	InitWindow(screenWidth, screenHeight, "Stratego");

	auto& netManager = NetworkManager::getInstance();
	SceneController2D sceneManager{};
	
	SetTargetFPS(60);

	auto lobbyScene = std::make_shared<Lobby>(screenWidth, screenHeight, netManager);
	sceneManager.instantiateScene(lobbyScene);

	netManager.onMessage("changeGameState", [&](const std::string& message)
	{
		if (std::stoi(message) == GAME_MODE)
		{
			gameState = GAME_MODE;
			auto gameScene = std::make_shared<Game>(lobbyScene->getIsHost(), sceneManager.camera);

			sceneManager.instantiateScene(gameScene);
		}
	});

	SetExitKey(0);
	while (!WindowShouldClose())  
	{
		BeginDrawing();

		sceneManager.beginUpdate(&screenWidth, &screenHeight);

		
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