#pragma once
#include "Board.h"

namespace dc
{
	class Evaluation
	{
	public:

		const int positionMap[13][13] =
		{
			1,1,2,3,2,3,4,3,2,3,2,1,1,
			1,0,1,0,2,0,3,0,2,0,1,0,1,
			2,1,3,3,2,2,5,2,2,3,3,1,2,
			2,0,2,0,3,0,3,0,3,0,2,0,1,
			3,3,3,3,5,4,5,4,5,3,3,3,3,
			3,0,2,0,3,0,5,0,3,0,2,0,3,
			3,2,4,3,4,3,5,3,4,3,4,2,3,
			3,0,2,0,3,0,5,0,3,0,2,0,3,
			3,3,3,3,5,4,5,4,5,3,3,3,3,
			2,0,2,0,3,0,3,0,3,0,2,0,1,
			2,1,3,3,2,2,5,2,2,3,3,1,2,
			1,0,1,0,2,0,3,0,2,0,1,0,1,
			1,1,2,3,2,3,4,3,2,3,2,1,1
		};
		const int pieceValue[22] = 
		{
			0, //none
			7, 20, 33, 37, 46, //normal towers 
			0, 0, 0,
			0, 30, 40, 45, 55, //blue towers
			0, 0, 0, 
			0, 45, 63, 67, 76 //red towrs
		};

		const int positionWeight = 8;
		const int materialWeight = 100;

		int evaluate(Board& board);

	};
}