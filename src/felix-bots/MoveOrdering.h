#pragma once
#include <vector>
#include "Utility.h"
#include "TranspositionTable.h"
#include "Evaluation.h"

namespace dc
{
	class MoveOrdering
	{
	private:
		std::vector<int> moveScores;

		const int million = 1000000;
		const int hashMoveScore = 100 * million;
		//const int addOnCaptureBias = 8 * million; //to capture a 3r for example
		const int redClaimBias = 8 * million;
		const int blueClaimBias = 6 * million;
		//const int killerBias = 4 * million;
		const int normalCaptureBias = 2 * million;
		const int regularBias = 0;

		//TranspositionTable<5>& transpositionTable;
		Evaluation& evaluation;

	public:
		MoveOrdering(Evaluation& _evaluation/*TranspositionTable<5>& tt) : transpositionTable(tt)*/)
			: evaluation(_evaluation)
		{
			moveScores.reserve(1000);
		}

		std::vector<int> makeMoveOrdering(const Move& hashMove, const Board& board, const std::vector<Move>& moveList, bool debug = false);

	private:
		void quicksort(std::vector<int>& indices, std::vector<int>& vals, int L, int R);
		void swap(std::vector<int>& indices, std::vector<int>& vals, int x, int y);
	};

}

