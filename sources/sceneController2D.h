#pragma once
#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"

#include "core.h"
#include "memory"
#include "scene.h"

class SceneController2D
{
private:
	int zoomMode;
	std::shared_ptr<GameScene> activeScene;
	
public:
	Camera2D camera;

	SceneController2D()
		: zoomMode{ 0 }, camera{ 0 }
	{
		camera.zoom = 1.0f;
		activeScene = std::make_shared<GameScene>();
	}

	void instantiateScene(std::shared_ptr<GameScene> scenePtr)
	{
		activeScene = scenePtr;
		scenePtr->start();
	}

	void beginUpdate(int* screenWidth, int* screenHeight)
	{
		// Web window resizing
		#ifdef WASM
		*screenWidth = EM_ASM_INT({ return window.innerWidth; });
		*screenHeight = EM_ASM_INT({ return window.innerHeight; });
		SetWindowSize(*screenWidth, *screenHeight);
		#else
		*screenWidth = GetScreenWidth();
		*screenHeight = GetScreenHeight();
		#endif


		if (IsKeyPressed(KEY_ONE)) zoomMode = 0;
		else if (IsKeyPressed(KEY_TWO)) zoomMode = 1;

		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
		{
			Vector2 delta = GetMouseDelta();
			delta = Vector2Scale(delta, -1.0f / camera.zoom);
			camera.target = Vector2Add(camera.target, delta);
		}

		if (zoomMode == 0)
		{
			float wheel = GetMouseWheelMove();
			if (wheel != 0)
			{
				Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

				camera.offset = GetMousePosition();

				camera.target = mouseWorldPos;

				// Zoom increment
				float scaleFactor = 1.0f + (0.25f * fabsf(wheel));
				if (wheel < 0) scaleFactor = 1.0f / scaleFactor;
				camera.zoom = Clamp(camera.zoom * scaleFactor, 0.125f, 64.0f);
			}
		}
		else
		{
			if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
			{
				Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

				camera.offset = GetMousePosition();

				camera.target = mouseWorldPos;
			}
			if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
			{
				// Zoom increment
				float deltaX = GetMouseDelta().x;
				float scaleFactor = 1.0f + (0.01f * fabsf(deltaX));
				if (deltaX < 0) scaleFactor = 1.0f / scaleFactor;
				camera.zoom = Clamp(camera.zoom * scaleFactor, 0.125f, 64.0f);
			}
		}

		activeScene->resize(*screenWidth, *screenHeight);
		activeScene->update();
		ClearBackground(RAYWHITE);

		BeginMode2D(camera);
		activeScene->sceneRender();
	}

	void endUpdate()
	{
		EndMode2D();
		activeScene->uiRender();
	}

	SceneController2D (const SceneController2D&) = delete;
	SceneController2D& operator= (const SceneController2D&) = delete;
};