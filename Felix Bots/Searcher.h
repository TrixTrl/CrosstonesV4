#pragma once
#include <cstdint>
#include <memory>
#include "../Test Bots/BasicGenerator.h"
#include "Board.h"
#include "Utility.h"
#include "Evaluation.h"
#include "TranspositionTable.h"
#include <vector>

namespace dc
{
	class Searcher
	{
	public:

		static const int immediateWinScore = 100000;
		static const int positiveInfinity = 9999999;
		static const int negativeInfinity = -positiveInfinity;

	private:

		Board& board;
		TranspositionTable<5/*MB*/> transpositionTable;
		Evaluation evaluation;

		Move bestMove;
		int bestEval;
		Move bestMoveThisIteration;
		int bestEvalThisIteration;
		bool hasSearchedAtLeastOneMove;

		int currentDepth;

		std::vector<Move> availableMoves;

	public:

		Searcher(Board& _board) : board(_board), transpositionTable(board)
		{
			availableMoves = std::vector<Move>();
		}

		//
		int negamax(uint8_t plyRemaining, uint8_t plyFromRoot, int alpha, int beta);
		int negamaxAtRoot(std::shared_ptr<std::vector<Move>> moves,
			uint8_t depth, int alpha, int beta);

		void startSearch(std::vector<Move>& _availableMoves);
		void runIterativeDeepeningSearch();
		int search(uint8_t plyRemaining, uint8_t plyFromRoot, int alpha, int beta);

		std::pair<Move, int> getSearchResult();

		void sortMoves(std::shared_ptr<std::vector<Move>> moves);

		static bool isWinScore(int score)
		{
			const int maxMateDepth = 1000;
			return abs(score) > immediateWinScore - maxMateDepth;
		}
		static int numPlyToWinFromScore(int score)
		{
			return immediateWinScore - abs(score);
		}

	};
}
