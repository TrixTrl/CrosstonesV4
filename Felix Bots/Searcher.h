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
		static const int immediateWinScore = 100000000;
		static const int positiveInfinity = 999999999;
		static const int negativeInfinity = -positiveInfinity;

	private:
		// References
		Board& board;
		TranspositionTable<10/*MB*/> transpositionTable;
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

		Searcher(Board& _board, int _maxDepth) : evaluation(), board(_board), transpositionTable(_board), moveOrdering(evaluation), maxDepth(_maxDepth)
		{}

		void startSearch();
		std::pair<Move, int> getSearchResult();

		static bool isWinScore(int score)
		{
			const int maxMateDepth = 1000;
			return abs(score) > immediateWinScore - maxMateDepth;
		}
		static int numPlyToWinFromScore(int score)
		{
			return immediateWinScore - abs(score);
		}

	private:
		void runIterativeDeepeningSearch();
		int search(uint8_t plyRemaining, uint8_t plyFromRoot, int alpha, int beta);
		int quiescenceSearch(int alpha, int beta);


		void sortMoves(std::shared_ptr<std::vector<Move>> moves);
	};
}
