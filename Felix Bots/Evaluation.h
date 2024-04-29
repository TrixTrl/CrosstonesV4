#pragma once
#include "Board.h"
#include <string>

namespace dc
{
	class Evaluation
	{
	public:
		struct Config {
			int positionMap[13][13];
			const int pieceValue[22];

			const int positionWeight;
			const int materialWeight;
			const int relativeWeight; 
			const int relativeExp; //exp
			Config(
				const int _positionMap[13][13],
				const int _pieceValue[22],
				const int _positionWeight = 6,
				const int _materialWeight = 50,
				const int _relativeWeight = 50,
				const int _relativeExp = 2) 
				: positionMap(), pieceValue(), positionWeight(_positionWeight), materialWeight(_materialWeight), relativeWeight(_relativeWeight), relativeExp(_relativeExp)
			{
				memcpy(&positionMap, &_positionMap, sizeof(positionMap));
			};
		} config;
		

		int evaluate(const Board& board);
		int getEffectivePieceWorth(const Board& board, const uint8_t piece, const int x, const int y) const;
		void initialize(std::string jsonConfigPath);

		Evaluation()
		{
			initialize("Felix Bots/DC_Eval_Config.json");
		}
	};
}