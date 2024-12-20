#include <stdint.h>

namespace Grid
{
	constexpr uint8_t EMPTY = 0;
	constexpr uint8_t BLOCKED = 1;
	constexpr uint8_t UNPLACEABLE = 2; 
	constexpr uint8_t FLAG = 3;
	constexpr uint8_t SPY = 4;
	constexpr uint8_t SCOUT = 5;
	constexpr uint8_t MINER = 6;
	constexpr uint8_t SERGEANT = 7;
	constexpr uint8_t LIEUTENANT = 8;
	constexpr uint8_t CAPTAIN = 9;
	constexpr uint8_t MAJOR = 10;
	constexpr uint8_t COLONEL = 11;
	constexpr uint8_t GENERAL = 12;
	constexpr uint8_t MARSHAL = 13;
	constexpr uint8_t BOMB = 14;

	constexpr int COLUMNS = 10;
	constexpr int ROWS = 10;

	constexpr uint8_t boardConfiguration[] = {
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
	};
}


