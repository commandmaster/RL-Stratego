#pragma once
#include <stdint.h>

struct Grid
{
	static constexpr uint8_t EMPTY = 0;
	static constexpr uint8_t BLOCKED = 1;
	static constexpr uint8_t UNPLACEABLE = 2;
	static constexpr uint8_t FLAG = 3;
	static constexpr uint8_t SPY = 4;
	static constexpr uint8_t SCOUT = 5;
	static constexpr uint8_t MINER = 6;
	static constexpr uint8_t SERGEANT = 7;
	static constexpr uint8_t LIEUTENANT = 8;
	static constexpr uint8_t CAPTAIN = 9;
	static constexpr uint8_t MAJOR = 10;
	static constexpr uint8_t COLONEL = 11;
	static constexpr uint8_t GENERAL = 12;
	static constexpr uint8_t MARSHAL = 13;
	static constexpr uint8_t BOMB = 14;
	static constexpr uint8_t HIDDEN_PIECE = 15;

	static constexpr uint8_t TEAM_RED = 0x80;
	static constexpr uint8_t TEAM_BLUE = 0x40;

	static constexpr uint8_t TEAM_MASK = 0xC0;

	static constexpr int COLUMNS = 10;
	static constexpr int ROWS = 10;

	uint8_t board[100];
};


