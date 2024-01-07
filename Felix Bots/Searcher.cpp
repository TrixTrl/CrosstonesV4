#include "Searcher.h"
#include "Utility.h"
#include <algorithm>
#include <format>

using namespace std;
using namespace dc;

void Searcher::startSearch(std::vector<Move>& _availableMoves)
{
	availableMoves = _availableMoves;

	// Initialize search
	bestEvalThisIteration = 0; 
	bestEval = 0;
	bestMoveThisIteration = Utility::createNullMove();
	bestMove = Utility::createNullMove();

	// Initialize debug info
	currentDepth = 0;
	Utility::print("Starting DC search", true);
	runIterativeDeepeningSearch();
}

// Run iterative deepening. This means doing a full search with a depth of 1, then with a depth of 2, and so on.
// This allows the search to be cancelled at any time and still yield a useful result.
void Searcher::runIterativeDeepeningSearch()
{
	for (int searchDepth = 1; searchDepth <= 10; searchDepth++)
	{
		hasSearchedAtLeastOneMove = false;
		search(searchDepth, 0, negativeInfinity, positiveInfinity);

		/*if (searchCancelled)
		{
			if (hasSearchedAtLeastOneMove)
			{
				bestMove = bestMoveThisIteration;
				bestEval = bestEvalThisIteration;
				searchDiagnostics.move = MoveUtility.GetMoveNameUCI(bestMove);
				searchDiagnostics.eval = bestEval;
				searchDiagnostics.moveIsFromPartialSearch = true;
				debugInfo += "\nUsing partial search result: " + MoveUtility.GetMoveNameUCI(bestMove) + " Eval: " + bestEval;
			}

			debugInfo += "\nSearch aborted";
			break;
		}
		else*/
		{
			currentDepth = searchDepth;
			bestMove = bestMoveThisIteration;
			bestEval = bestEvalThisIteration;

			Utility::print(std::format(
					"\nIteration {} result: \n{} \nEval: {}", 
					searchDepth,
					Utility::toString(bestMove),
					bestEval),
				true);
			if (isWinScore(bestEval))
			{
				Utility::print(std::format("Mate in ply: {}", numPlyToWinFromScore(bestEval)), true);
			}

			bestEvalThisIteration = negativeInfinity;
			bestMoveThisIteration = Utility::createNullMove();

			// Exit search if found a win within search depth.
			// A win found outside of search depth (due to extensions) may not be the fastest win.
			if (isWinScore(bestEval) && numPlyToWinFromScore(bestEval) <= searchDepth)
			{
				Utility::print("Exitting search due to mate found within search depth", true);
				break;
			}
		}
	}
}

int Searcher::search(uint8_t plyRemaining, uint8_t plyFromRoot, int alpha, int beta)
{
	if (plyFromRoot > 0)
	{
		// Skip this position if a winning sequence has already been found earlier in the search, which would be shorter
		// than any mate we could find from here. This is done by observing that alpha can't possibly be worse
		// (and likewise beta can't possibly be better) than being mated in the current position.
		alpha = max(alpha, -immediateWinScore + plyFromRoot);
		beta = min(beta, immediateWinScore - plyFromRoot);
		if (alpha >= beta)
		{
			return alpha;
		}
	}
	// Try looking up the current position in the transposition table.
	// If the same position has already been searched to at least an equal depth
	// to the search we're doing now,we can just use the recorded evaluation.
	int ttVal = transpositionTable.lookupEvaluation(plyRemaining, plyFromRoot, alpha, beta);
	if (ttVal != TranspositionTable<1>::LookupFailed)
	{
		if (plyFromRoot == 0)
		{
			bestMoveThisIteration = transpositionTable.tryGetStoredMove();
			bestEvalThisIteration = transpositionTable.tryGetEvalForMove();
		}
		return ttVal;
	}

	if (plyRemaining == 0 || Utility::gameOver(&board.square))
	{
		int eval = evaluation.evaluate(board);
		return eval;
	}

	shared_ptr<vector<vector<BasicGenerator::xMove>>> moves =
		BasicGenerator::getMoves(board.isWhiteTurn, &(board.square));
	Move prevBestMove = plyFromRoot == 0 ? bestMove : transpositionTable.tryGetStoredMove();

	// Detect draw by no moves available
	if (moves->size() == 0)
	{
		/*if (Utility::isWon())
		{
			int winScore = immediateWinScore - plyFromRoot;
			return -mateScore;
		}*/
		return 0;
	}

	int evaluationBound = TranspositionTable<1>::UpperBound;
	Move* bestMoveInThisPosition = &Utility::nullMove;

	// Skip i = 0 to not allow the Null move
	for (int i = 1; i < moves->size(); i++)
	{
		Move* move = &(*moves)[i];
		int eval = 0;

		board.makeMove(moves->at(i));

		eval = -search(plyRemaining - 1, plyFromRoot + 1, -beta, -alpha);

		board.unmakeMove(moves->at(i));

		// Move was *too* good, opponent will choose a different move earlier on to avoid this position.
		// (Beta-cutoff / Fail high)
		if (eval >= beta)
		{
			// Store evaluation in transposition table. Note that since we're exiting the search early, there may be an
			// even better move we haven't looked at yet, and so the current eval is a lower bound on the actual eval.
			transpositionTable.storeEvaluation(plyRemaining, plyFromRoot, beta, TranspositionTable<1>::LowerBound, *move);
			return beta;
		}

		// Found a new best move in this position
		if (eval > alpha)
		{
			evaluationBound = TranspositionTable<1>::Exact;
			bestMoveInThisPosition = move;

			alpha = eval;
			if (plyFromRoot == 0)
			{
				bestMoveThisIteration = *move;
				bestEvalThisIteration = eval;
			}
		}
	}
	transpositionTable.storeEvaluation(plyRemaining, plyFromRoot, alpha, evaluationBound, *bestMoveInThisPosition);

	return alpha;
}

std::pair<Move, int> Searcher::getSearchResult()
{
	return std::pair(bestMove, bestEval);
}

int Searcher::negamax(uint8_t plyRemaining, uint8_t plyFromRoot, int alpha, int beta)
{
	// color: 1 when white, -1 when black
	uint8_t color = 2 * board.isWhiteTurn - 1;

	if (plyRemaining == 0 || Utility::gameOver(&board.square))
		return evaluation.evaluate(board) * color; 
	
	// Try looking up the current position in the transposition table.
	// If the same position has already been searched to at least an equal depth
	// to the search we're doing now,we can just use the recorded evaluation.
	int ttVal = transpositionTable.lookupEvaluation(plyRemaining, plyFromRoot, alpha, beta);
	if (ttVal != TranspositionTable<1>::LookupFailed)
	{
		return ttVal;
	}

	shared_ptr<vector<vector<BasicGenerator::xMove>>> moves =
		BasicGenerator::getMoves(board.isWhiteTurn, &(board.square));
	//sortMoves(squares, moves);
	int value = negativeInfinity;

	for (int i = 1; i < moves->size(); i++)
	{
		board.makeMove(moves->at(i));

		value = max(value, -negamax(plyRemaining - 1, plyFromRoot + 1, -beta, -alpha));

		board.unmakeMove(moves->at(i));

		alpha = max(alpha, value);
		if (alpha >= beta)
			break; // cutoff
	}
	return value;
}

/* returns index of best move*/
int Searcher::negamaxAtRoot(shared_ptr<vector<Move>> moves,
	uint8_t depth, int alpha, int beta)
{

	if (depth == 0 || Utility::gameOver(&board.square))
		return -1;

	int bestMove = -1;
	int eval, value = negativeInfinity;

	//sortMoves(board, moves);
	for (int i = 0; i < moves->size(); i++)
	{
		board.makeMove(moves->at(i));

		eval = -negamax(depth - 1, 0, -beta, -alpha);

		board.unmakeMove(moves->at(i));

		if (eval > value) {
			value = eval;
			bestMove = i;
		}
		alpha = max(alpha, value);
		if (alpha >= beta)
			break; // cutoff
	}
	return bestMove;
}

void Searcher::sortMoves(shared_ptr<vector<vector<BasicGenerator::xMove>>> moves)
{
	//ideal would be in order:
	// - merges onto addon
	// - captures of addOn
	// - normal captures
	// - moves with addOn
	// - the rest
	auto compareMoves = [](const vector<BasicGenerator::xMove>& i1, const vector<BasicGenerator::xMove>& i2) {
		return i1.size() > i2.size();
	};
	sort(moves.get()->begin(), moves.get()->end(), compareMoves);
}