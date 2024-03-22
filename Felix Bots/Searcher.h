#pragma once
#include "../Test Bots/BasicGenerator.h"
#include "Board.h"
#include "Evaluation.h"
#include "MoveGenerator.h"
#include "MoveOrdering.h"
#include "TranspositionTable.h"
#include "Utility.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>


namespace dc
{
	class Searcher
	{
	public:
		struct SearchDiagnostics
		{
			int numCompletedIterations = 0;
			int numPositionsEvaluated = 0;
			int numTranspositionHits = 0;
			unsigned long numCutOffs = 0;

			std::string moveVal = "";
			std::string move = "";
			int movesSize = 0;
			int bestMoveNumber = 0;

			int eval = 0;
			bool moveIsFromPartialSearch = false;
			int NumQChecks = 0;
			int numQMates = 0;

			bool isBook = false;

			int maxExtentionReachedInSearch = 0;
		};

		// Constants
		static const int immediateWinScore = 1000000;
		static const int positiveInfinity = 99999999;
		static const int negativeInfinity = -positiveInfinity;

	private:
		// References
		Board& board;
		TranspositionTable<5/*MB*/> transpositionTable;
		Evaluation evaluation;
		MoveOrdering moveOrdering;
		MoveGenerator moveGenerator;

		// State
		int currentDepth;
		Move bestMove;
		int bestEval;
		Move bestMoveThisIteration;
		int bestEvalThisIteration;
		bool hasSearchedAtLeastOneMove;


		int maxDepth;

		// Diagnostics
		SearchDiagnostics searchDiagnostics;


	public:

		Searcher(Board& _board, int _maxDepth) : board(_board), transpositionTable(_board), moveOrdering(), maxDepth(_maxDepth)
		{}

		void startSearch();
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
