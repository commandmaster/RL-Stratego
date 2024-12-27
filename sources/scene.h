#pragma once


#include "core.h"
#include "raylib.h"
#include "ui.h"
#include "networking.h"
#include "gameGrid.h"


#include "WasmOnly/rapidjson/document.h"
#include "WasmOnly/rapidjson/writer.h"
#include "WasmOnly/rapidjson/stringbuffer.h"

#include <iostream>
#include <cmath>
#include <memory>
#include <vector>
#include <string>

class GameScene
{
public:
	GameScene() {}
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
	UI::Button joinButton;;
	UI::Popup inputPopup;
	
	UI::Label ipLabel;
	UI::InputBox ipInput;
	UI::InputBox nameInput;

	UI::Label titleText;

	UI::Label playerOneLabel;
	UI::Label playerTwoLabel;
	UI::Label versusLabel;

	UI::Label roomCodeLabel;

	int screenWidth;
	int screenHeight;

	float sineWaveTime;

	enum class LobbyState
	{
		LOBBY,
		ROOM,
		ROOMCODE
	};

	bool isHost = false;
	LobbyState lobbyState = LobbyState::LOBBY;

	NetworkManager& netManager;
public:
	Lobby(int sWidth, int sHeight, NetworkManager& netManager)
		: hostButton{ 0, -80, 600, 100, UI::Anchor::Center, true, 60 }, joinButton{ 0, 65, 600, 100, UI::Anchor::Center, true, 60 }, inputPopup{0, 0, 900, 600, UI::Anchor::Center, true}, titleText {
		0, -350, std::string("Stratego"), UI::Anchor::Center, true, 125
		}, ipLabel{ 50.0f, 45.0f, "IP Address", UI::Anchor::TopLeft, false, 30 }, ipInput{ 50.0f, 80.0f, 200.0f, 40.0f, UI::Anchor::TopLeft, false }, nameInput{ 50.0f, -80.0f, 200.0f, 40.0f, UI::Anchor::BottomLeft, false }, playerOneLabel{ 0, -100, "", UI::Anchor::Center, true, 50 }, playerTwoLabel{ 0, 100, "", UI::Anchor::Center, true, 50 }, versusLabel{ 0, 0, "Vs", UI::Anchor::Center, true, 50 }, roomCodeLabel{ 0, 100, "0000", UI::Anchor::TopCenter, true, 50 }, netManager {
		netManager
	}
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

		nameInput.SetBaseResolution(1920, 1080);
		nameInput.SetBackgroundColor(RAYWHITE);
		nameInput.SetBorderColor(BLACK);
		nameInput.SetLabelTextColor(BLACK);
		nameInput.SetLabelText("UserTest");
		nameInput.SetBorderThickness(3);
		nameInput.OnSubmit([&](UI::InputBox& ip) 
		{
			netManager.sendMessage("setName", ip.GetLabelText());
			playerOneLabel.SetText(ip.GetLabelText());
		});

		inputPopup.SetBackgroundColor(LIGHTGRAY);
		inputPopup.SetBorderColor(BLACK);
		inputPopup.SetBorderThickness(3);
		inputPopup.SetLabelFontSize(50);
		inputPopup.SetOnSubmit([&](const std::string& text) {
			netManager.sendMessage("joinGame", text);
		});

		ipLabel.SetBaseResolution(1920, 1080);
		ipLabel.SetTextColor(BLACK);
		
		titleText.SetBaseResolution(1920, 1080);
		titleText.SetTextColor(DARKGRAY);

		playerOneLabel.SetBaseResolution(1920, 1080);
		playerOneLabel.SetTextColor(BLACK);

		playerTwoLabel.SetBaseResolution(1920, 1080);
		playerTwoLabel.SetTextColor(BLACK);

		versusLabel.SetBaseResolution(1920, 1080);
		versusLabel.SetTextColor(BLACK);

		roomCodeLabel.SetBaseResolution(1920, 1080);
		roomCodeLabel.SetTextColor(BLACK);

		screenWidth = sWidth;
		screenHeight = sHeight;

		sineWaveTime = 0.0f;
	}

	void start()
	{
		netManager.onMessage("roomCode", [&](const std::string& message) 
		{
			roomCodeLabel.SetText(message);
			lobbyState = LobbyState::ROOM;
		});

		netManager.onMessage("nameTaken", [&](const std::string& message) 
		{
				nameInput.SetLabelText(message);
				playerOneLabel.SetText(message);
		});

		netManager.onMessage("requestName", [&](const std::string& message) 
		{
			netManager.sendMessage("setName", nameInput.GetLabelText());
		});

		netManager.onMessage("renameOtherPlayer", [&](const std::string& message) {
			playerTwoLabel.SetText(message);
		});

		netManager.onJson("gameFull", [&](rapidjson::Document& document) {
			std::cout << "json" << std::endl;

			for (const auto& element : document.GetArray())
			{
				const auto& obj = element.GetObject();

				std::cout << "\n";
				if (obj.HasMember("name")) 
				{
					std::cout << "client id: " << netManager.getSocketId().c_str() << std::endl;
					std::cout << "server id: " << obj["socketID"].GetString() << std::endl;
					if (std::string(obj["socketID"].GetString()) == netManager.getSocketId())
					{

					}
					else
					{
						playerTwoLabel.SetText(obj["name"].GetString());
					}
				}
				else { std::cerr << "missing name atrib" << std::endl; }
				std::cout << "\n";
			}
			
		});
	}

	void update()
	{
		sineWaveTime += GetFrameTime();
		float deltaTitleY = sinf(sineWaveTime * 2.0f) * 20.0f;

		titleText.SetPosition(0, -350 + deltaTitleY);

		if (hostButton.IsClicked() && lobbyState == LobbyState::LOBBY)
		{
			netManager.connect(ipInput.GetLabelText());
			netManager.sendMessage("createGame", "");
			netManager.sendMessage("setName", nameInput.GetLabelText());
			playerOneLabel.SetText(nameInput.GetLabelText());
			isHost = true;
		}
		if (joinButton.IsClicked() && lobbyState == LobbyState::LOBBY)
		{
			netManager.connect(ipInput.GetLabelText());
			isHost = false;
			netManager.sendMessage("setName", nameInput.GetLabelText());
			playerOneLabel.SetText(nameInput.GetLabelText());
			lobbyState = LobbyState::ROOMCODE;
		}

		switch (lobbyState)
		{
		case Lobby::LobbyState::LOBBY:
			ipInput.CheckForInput();
			nameInput.CheckForInput();
			break;
		case Lobby::LobbyState::ROOM:
			nameInput.CheckForInput();
			
			if (isHost)
			{
				if (UI::ResponsiveGui::DrawComponent(UI::Anchor::BottomCenter, {0, 150}, {250, 50}, GuiButton, "Start Game")) {
					netManager.sendMessage("startGame", "");
				}
			}

			break;
		case LobbyState::ROOMCODE:
			inputPopup.CheckForInput();
			if (IsKeyPressed(KEY_ESCAPE))
			{
				lobbyState = LobbyState::LOBBY;
			}
			break;
		}
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
			nameInput.Draw();
			break;
		case Lobby::LobbyState::ROOM:
			nameInput.Draw();

			playerOneLabel.Draw();
			playerTwoLabel.Draw();
			
			versusLabel.Draw();
			roomCodeLabel.Draw();
			break;
		case LobbyState::ROOMCODE:
			inputPopup.Draw();
			break;
		}
		
		
    }

	void resize(int screenWidth, int screenHeight)
	{
		this->screenWidth = screenWidth;
		this->screenHeight = screenHeight;
	}

	bool getIsHost() const
	{
		return isHost;
	}
};

class Game : public GameScene
{
private:
	enum class Mode
	{
		PLACE,
		MOVE
	};

	struct Pieces
	{
		int selectedPiece = Grid::EMPTY;

		uint8_t pieceArr[12] = {
			1, 1, 8, 5, 4, 4, 4, 3, 2, 1, 1, 6
		};

		uint8_t& operator[](size_t gridPiece)
		{
			size_t index = gridPiece - 3;

			if (index >= 12 || index < 0)
			{
				throw std::out_of_range("Index out of bounds");
			}
			return pieceArr[index];
		}

		const uint8_t& operator[](size_t gridPiece) const
		{
			size_t index = gridPiece - 3;

			if (index >= 12 || index < 0)
			{
				throw std::out_of_range("Index out of bounds");
			}
			return pieceArr[index];
		}
	};

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

	Pieces pieces;
	Mode mode = Mode::PLACE;
	bool isHost;
	Camera2D& camera;

	static constexpr int cellWidth = 100;
	static constexpr int cellHeight = 100;
	
public:
	Game(bool isHost, Camera2D& camera)
		: pieces{}, camera{ camera }
	{
		this->isHost = isHost;
	}

	void start()
	{
		std::cout << "setting up board listener" << "\n";

		NetworkManager::getInstance().onJson("setBoard", [&](rapidjson::Document& document) {
			auto boardData = document.GetArray();

			for (int i = 0; i < boardData.Size(); ++i)
			{
				auto byte = static_cast<uint8_t>(boardData[i].GetUint());
				assert(i < 100);

				if (i < 100)
				{
					gameGrid.board[i] = byte;
				}

			}
		});

		NetworkManager::getInstance().sendMessage("boardLoaded", "");
		

		
	}

	void uiRender()
	{

	}

	void sceneRender()
	{
		drawGameBoard(gameGrid, cellWidth, cellHeight);
		drawPiecePicker(cellWidth, cellHeight, isHost, pieces);

	}

	void update()
	{
		pickPiece(pieces, cellWidth, cellHeight, camera);
		placePiece(pieces, gameGrid, cellWidth, cellHeight, isHost, camera);
	}

	static void deletePiece(Pieces& pieces, Grid& grid, int cellWidth, int cellHeight, bool isRed, const Camera2D& camera)
	{
		Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), camera);
		for (int i = 0; i < Grid::COLUMNS; ++i)
		{
			for (int j = 0; j < Grid::ROWS; ++j)
			{
				int cellX = i * cellWidth + GetScreenWidth() / 2 - cellWidth * Grid::COLUMNS / 2;
				int cellY = j * cellHeight + GetScreenHeight() / 2 - cellHeight * Grid::ROWS / 2;
				Rectangle pieceRect = { static_cast<float>(cellX), static_cast<float>(cellY), static_cast<float>(cellWidth), static_cast<float>(cellHeight) };
				if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
				{
					if (CheckCollisionPointRec(mousePos, pieceRect))
					{
						if (grid.board[i + j * Grid::COLUMNS] != Grid::EMPTY && grid.board[i + j * Grid::COLUMNS] & (isRed? Grid::TEAM_RED : Grid::TEAM_BLUE))
						{
							grid.board[i + j * Grid::COLUMNS] = Grid::EMPTY;
						}
					}
				}
			}
		}
	}

	static void placePiece(Pieces& pieces, Grid& grid, int cellWidth, int cellHeight, bool isRed, const Camera2D& camera)
	{
		// Transform the mouse position to world coordinates
		Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), camera);
		for (int i = 0; i < Grid::COLUMNS; ++i)
		{
			for (int j = 0; j < Grid::ROWS; ++j)
			{
				int cellX = i * cellWidth + GetScreenWidth() / 2 - cellWidth * Grid::COLUMNS / 2;
				int cellY = j * cellHeight + GetScreenHeight() / 2 - cellHeight * Grid::ROWS / 2;

				Rectangle pieceRect = { static_cast<float>(cellX), static_cast<float>(cellY), static_cast<float>(cellWidth), static_cast<float>(cellHeight) };

				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
				{
					if (CheckCollisionPointRec(mousePos, pieceRect))
					{
						if (grid.board[i + j * Grid::COLUMNS] == (Grid::EMPTY & (isRed ? Grid::TEAM_RED : Grid::TEAM_BLUE)) && pieces[pieces.selectedPiece] > 0)
						{
							grid.board[i + j * Grid::COLUMNS] = pieces.selectedPiece | (isRed ? Grid::TEAM_RED : Grid::TEAM_BLUE);
							pieces[pieces.selectedPiece]--;

							rapidjson::Document document;
							document.SetObject();

							int column = i;
							int row = j;
							int piece = pieces.selectedPiece;
							
							document.AddMember("column", column, document.GetAllocator());
							document.AddMember("row", row, document.GetAllocator());
							document.AddMember("piece", piece, document.GetAllocator());

							rapidjson::StringBuffer buffer;
							rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
							document.Accept(writer);
							
							NetworkManager::getInstance().sendMessage("placePiece", buffer.GetString());
						}
					}
				}
			}
		}
	}

	static void pickPiece(Pieces& pieces, int cellWidth, int cellHeight, const Camera2D& camera)
	{
		// Transform the mouse position to world coordinates
		Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), camera);

		for (int i = 0; i < 12; ++i)
		{
			// Calculate rectangle position in world coordinates
			int cellX = (i - 1) * cellWidth + GetScreenWidth() / 2 - cellWidth * Grid::COLUMNS / 2;
			int cellY = 11 * cellHeight + GetScreenHeight() / 2 - cellHeight * Grid::ROWS / 2;

			// Define the rectangle for the piece in world coordinates
			Rectangle pieceRect = { static_cast<float>(cellX), static_cast<float>(cellY), static_cast<float>(cellWidth), static_cast<float>(cellHeight) };

			// Check if the rectangle is clicked
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
			{
				if (CheckCollisionPointRec(mousePos, pieceRect))
				{
					switch (i)
					{
					case 0:
						pieces.selectedPiece = Grid::FLAG;
						break;
					case 1:
						pieces.selectedPiece = Grid::BOMB;
						break;
					case 2:
						pieces.selectedPiece = Grid::SPY;
						break;
					case 3:
						pieces.selectedPiece = Grid::SCOUT;
						break;
					case 4:
						pieces.selectedPiece = Grid::MINER;
						break;
					case 5:
						pieces.selectedPiece = Grid::SERGEANT;
						break;
					case 6:
						pieces.selectedPiece = Grid::LIEUTENANT;
						break;
					case 7:
						pieces.selectedPiece = Grid::CAPTAIN;
						break;
					case 8:
						pieces.selectedPiece = Grid::MAJOR;
						break;
					case 9:
						pieces.selectedPiece = Grid::COLONEL;
						break;
					case 10:
						pieces.selectedPiece = Grid::GENERAL;
						break;
					case 11:
						pieces.selectedPiece = Grid::MARSHAL;
						break;
					}
				}
			}
		}
	}



	static void drawPiecePicker(int cellWidth, int cellHeight, bool isRed, const Pieces& pieces)
	{
		for (int i = 0; i < 12; ++i)
		{
			int cellX = (i-1) * cellWidth + GetScreenWidth() / 2 - cellWidth * Grid::COLUMNS / 2;
			int cellY = 11 * cellHeight + GetScreenHeight() / 2 - cellHeight * Grid::ROWS / 2;
			
			DrawRectangleLinesEx({ static_cast<float>(cellX), static_cast<float>(cellY), static_cast<float>(cellWidth), static_cast<float>(cellHeight) }, 1, GRAY);

			std::string pieceText;
			std::string smallNumber;
			std::string countText;

			Color color = (isRed) ? RED : DARKBLUE;

			
			switch (i)
			{
			case 0: 
				pieceText = "F"; 
				countText = std::to_string(pieces[Grid::FLAG]);
				color = (pieces.selectedPiece == Grid::FLAG) ? Color( color.r, color.b, color.g, 100 ) : color;
				break; // Flag
			case 1: 
				pieceText = "B"; 
				countText = std::to_string(pieces[Grid::BOMB]); 
				color = (pieces.selectedPiece == Grid::BOMB) ?  Color( color.r, color.b, color.g, 100 ) : color;
				break; // Bomb
			case 2: 
				pieceText = "S"; 
				smallNumber = "1"; 
				countText = std::to_string(pieces[Grid::SPY]); 
				color = (pieces.selectedPiece == Grid::SPY) ?  Color( color.r, color.b, color.g, 100 ) : color;
				break; // Spy
			case 3: 
				pieceText = "Sc"; 
				smallNumber = "2"; 
				countText = std::to_string(pieces[Grid::SCOUT]); 
				color = (pieces.selectedPiece == Grid::SCOUT) ?  Color( color.r, color.b, color.g, 100 ) : color;
				break; // Scout
			case 4: 
				pieceText = "M"; 
				smallNumber = "3"; 
				countText = std::to_string(pieces[Grid::MINER]); 
				color = (pieces.selectedPiece == Grid::MINER) ?  Color( color.r, color.b, color.g, 100 ) : color;
				break; // Miner
			case 5: 
				pieceText = "Se"; 
				smallNumber = "4"; 
				countText = std::to_string(pieces[Grid::SERGEANT]); 
				color = (pieces.selectedPiece == Grid::SERGEANT) ?  Color( color.r, color.b, color.g, 100 ) : color;
				break; // Sergeant
			case 6: 
				pieceText = "L"; 
				smallNumber = "5"; 
				countText = std::to_string(pieces[Grid::LIEUTENANT]); 
				color = (pieces.selectedPiece == Grid::LIEUTENANT) ?  Color( color.r, color.b, color.g, 100 ) : color;
				break; // Lieutenant
			case 7: 
				pieceText = "C"; 
				smallNumber = "6"; 
				countText = std::to_string(pieces[Grid::CAPTAIN]); 
				color = (pieces.selectedPiece == Grid::CAPTAIN) ?  Color( color.r, color.b, color.g, 100 ) : color;
				break; // Captain
			case 8: 
				pieceText = "Mj"; 
				smallNumber = "7"; 
				countText = std::to_string(pieces[Grid::MAJOR]); 
				color = (pieces.selectedPiece == Grid::MAJOR) ?  Color( color.r, color.b, color.g, 100 ) : color;
				break; // Major
			case 9: 
				pieceText = "Cl"; 
				smallNumber = "8"; 
				countText = std::to_string(pieces[Grid::COLONEL]); 
				color = (pieces.selectedPiece == Grid::COLONEL) ?  Color( color.r, color.b, color.g, 100 ) : color;
				break; // Colonel
			case 10: 
				pieceText = "G"; 
				smallNumber = "9"; 
				countText = std::to_string(pieces[Grid::GENERAL]); 
				color = (pieces.selectedPiece == Grid::GENERAL) ?  Color( color.r, color.b, color.g, 100 ) : color;
				break; // General
			case 11: 
				pieceText = "Msh"; 
				smallNumber = "10"; 
				countText = std::to_string(pieces[Grid::MARSHAL]); 
				color = (pieces.selectedPiece == Grid::MARSHAL) ?  Color( color.r, color.b, color.g, 100 ) : color;
				break; // Marshal
			default: 
				pieceText = ""; 
				break;
			}


			DrawRectangle(cellX, cellY, cellWidth, cellHeight, color);
			
			// Draw the main text in the center
			if (!pieceText.empty())
			{
				int fontSize = 40;
				int textWidth = MeasureText(pieceText.c_str(), fontSize);
				int textX = cellX + (cellWidth - textWidth) / 2;
				int textY = cellY + (cellHeight - fontSize) / 2;
				DrawText(pieceText.c_str(), textX, textY, fontSize, RAYWHITE);
			}

			if (!countText.empty())
			{
				int fontSize = 40;
				int textWidth = MeasureText(countText.c_str(), fontSize);
				int textX = cellX + (cellWidth - textWidth) / 2;
				int textY = cellY + (cellHeight - fontSize) / 2 + cellHeight;
				DrawText(countText.c_str(), textX, textY, fontSize, BLACK);
			}

			// Draw the small number in the corners if it exists
			if (!smallNumber.empty())
			{
				int smallFontSize = 20;

				// Top-left corner
				DrawText(smallNumber.c_str(), cellX + 5, cellY + 5, smallFontSize, RAYWHITE);

				// Bottom-right corner
				Vector2 textSize = MeasureTextEx(GetFontDefault(), smallNumber.c_str(), smallFontSize, 0);
				DrawText(smallNumber.c_str(), cellX + cellWidth - 5 - textSize.x, cellY + cellHeight - 5 - textSize.y, smallFontSize, RAYWHITE);
			}
		}
	}


	static void drawGameBoard(const Grid& grid, int cellWidth, int cellHeight)
	{
		for (int i = 0; i < Grid::COLUMNS; ++i)
		{
			for (int j = 0; j < Grid::ROWS; ++j)
			{
				Color borderColor = GRAY;

				const uint8_t& squareValue = grid.board[i + j * Grid::COLUMNS];
				int cellX = i * cellWidth + GetScreenWidth() / 2 - cellWidth * Grid::COLUMNS / 2;
				int cellY = j * cellHeight + GetScreenHeight() / 2 - cellHeight * Grid::ROWS / 2;

				if (grid.board[i + j * Grid::COLUMNS] == Grid::BLOCKED)
				{
					DrawRectangle(i * cellWidth + GetScreenWidth() / 2 - cellWidth * Grid::COLUMNS / 2,
						j * cellHeight + GetScreenHeight() / 2 - cellHeight * Grid::ROWS / 2,
						cellWidth, cellHeight, BLUE);
				}
				else
				{
					if ((squareValue & ~Grid::TEAM_MASK) != Grid::EMPTY && (squareValue) != Grid::UNPLACEABLE) DrawRectangle(cellX, cellY, cellWidth, cellHeight, (squareValue & Grid::TEAM_RED) ? RED : DARKBLUE);
					DrawRectangleLines(i * cellWidth + GetScreenWidth() / 2 - cellWidth * Grid::COLUMNS / 2, 
									   j * cellHeight + GetScreenHeight() / 2 - cellHeight * Grid::ROWS / 2, 
									   cellWidth, cellHeight, borderColor);
				}

				std::string numberText{};
				int fontSize = 40;
				int textWidth{};
				int textX{};
				int textY{};

				switch (squareValue & ~Grid::TEAM_MASK){ 
				case Grid::FLAG:
					numberText = "F";
					break;

				case Grid::BOMB:
					numberText = "B";
					break;

				case Grid::SPY:
					numberText = "S";
					break;
				case Grid::SCOUT:
					numberText = "Sc";
					break;
				case Grid::MINER:
					numberText = "M";
					break;
				case Grid::SERGEANT:
					numberText = "Se";
					break;
				case Grid::LIEUTENANT:
					numberText = "L";
					break;
				case Grid::CAPTAIN:
					numberText = "C";
					break;
				case Grid::MAJOR:
					numberText = "Mj";
					break;
				case Grid::COLONEL:
					numberText = "Cl";
					break;
				case Grid::GENERAL:
					numberText = "G";
					break;
				case Grid::MARSHAL:
					numberText = "Msh";
					break;

				default:
					numberText = "";
					break;
				}

				if (!numberText.empty()) 
				{
					fontSize = 40; 
					textWidth = MeasureText(numberText.c_str(), fontSize);
					textX = cellX + (cellWidth - textWidth) / 2;
					textY = cellY + (cellHeight - fontSize) / 2;
					DrawText(numberText.c_str(), textX, textY, fontSize, RAYWHITE);

					if ((squareValue &~ Grid::TEAM_MASK) != Grid::FLAG && (squareValue &~ Grid::TEAM_MASK) != Grid::BOMB) 
					{
						std::string smallNumber = std::to_string((squareValue &~ Grid::TEAM_MASK) - 3);
						int smallFontSize = 20;

						Vector2 textSize = MeasureTextEx(GetFontDefault(), smallNumber.c_str(), smallFontSize, 0);

						DrawText(smallNumber.c_str(), cellX + 5, cellY + 5, smallFontSize, RAYWHITE);
						DrawText(smallNumber.c_str(), cellX + cellWidth - 5 - textSize.x, cellY + cellHeight - textSize.y - 5, smallFontSize, RAYWHITE);

					}
				}
				
			}
		}
	}
};

