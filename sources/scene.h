#pragma once

#include "core.h"
#include "raylib.h"
#include "ui.h"
#include "networking.h"

#include <iostream>
#include <cmath>
#include <memory>

class GameScene
{
public:
	virtual void uiRender() {}
	virtual void sceneRender() {}
	virtual void update() {}
	virtual void start() {}
	virtual void resize(int screenWidth, int screenHeight) {}
	virtual ~GameScene() {}
};

class Lobby : public GameScene
{
private:
	UI::Button hostButton;
    UI::Button joinButton;
	
	UI::Label ipLabel;
	UI::InputBox ipInput;

	UI::Label titleText;

	int screenWidth;
	int screenHeight;

	float sineWaveTime;

	enum class LobbyState
	{
		LOBBY,
		ROOM
	};

	LobbyState lobbyState = LobbyState::LOBBY;

	NetworkManager& netManager;
public:
	Lobby(int sWidth, int sHeight, NetworkManager& netManager)
		: hostButton{ 0, -80, 600, 100, UI::Anchor::Center, true, 60 }, joinButton{ 0, 65, 600, 100, UI::Anchor::Center, true, 60 }, titleText{ 0, -350, std::string("Stratego"), UI::Anchor::Center, true, 125 }, ipLabel{ 50.0f, 45.0f, "IP Address", UI::Anchor::TopLeft, false, 30 }, ipInput{ 50.0f, 80.0f, 200.0f, 40.0f, UI::Anchor::TopLeft, false }, netManager{ netManager }
	{
		hostButton.SetBaseResolution(1920, 1080);
		hostButton.SetLabel("Host");
		hostButton.SetLabelColor(RAYWHITE);
		hostButton.SetButtonColor(DARKBLUE);

		joinButton.SetBaseResolution(1920, 1080);
		joinButton.SetLabel("Join");
		joinButton.SetLabelColor(RAYWHITE);
		joinButton.SetButtonColor(DARKGREEN);

		ipInput.SetBaseResolution(1920, 1080);
		ipInput.SetBackgroundColor(RAYWHITE);
		ipInput.SetBorderColor(BLACK);
		ipInput.SetLabelTextColor(BLACK);
		ipInput.SetLabelText("http://localhost:3000");
		ipInput.SetBorderThickness(3);
		ipInput.OnSubmit([&](UI::InputBox& ip) 
		{
			netManager.connect(ip.GetLabelText());
		});

		ipLabel.SetBaseResolution(1920, 1080);
		ipLabel.SetTextColor(BLACK);
		
		titleText.SetBaseResolution(1920, 1080);
		titleText.SetTextColor(DARKGRAY);


		screenWidth = sWidth;
		screenHeight = sHeight;

		sineWaveTime = 0.0f;
	}

	void start()
	{
		netManager.onMessage("roomCode", [&](const std::string& message) 
		{
			std::cout << "roomCode: " << message << std::endl;
			lobbyState = LobbyState::ROOM;
		});
	}

	void update()
	{
		sineWaveTime += GetFrameTime();
        float deltaTitleY = sinf(sineWaveTime * 2.0f) * 20.0f;
		
		titleText.SetPosition(0, -350 + deltaTitleY);

		if (hostButton.IsClicked())
		{
			netManager.sendMessage("createGame", "");
		}

		ipInput.CheckForInput();
	}

	void uiRender()
	{
			
		switch (lobbyState)
		{
		case Lobby::LobbyState::LOBBY:
			hostButton.Draw();
			joinButton.Draw();
			titleText.Draw();
			ipLabel.Draw();
			ipInput.Draw();
			break;
		case Lobby::LobbyState::ROOM:

			break;
		}
		
    }

	void resize(int screenWidth, int screenHeight)
	{
		this->screenWidth = screenWidth;
		this->screenHeight = screenHeight;
	}
};

class Game : public GameScene
{
private:

public:

};