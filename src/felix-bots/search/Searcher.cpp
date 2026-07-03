#include "felix-bots/search/Searcher.h"
#include "felix-bots/util/Utility.h"
#include <algorithm>
#include <format>

using namespace std;
using namespace dc;

void Searcher::startSearch()
{
	// Initialize search
	bestEvalThisIteration = negativeInfinity; 
	bestEval = negativeInfinity;
	bestMoveThisIteration = Utility::createNullMove();
	bestMove = Utility::createNullMove();

	// Initialize debug info
	currentDepth = 0;
	searchDiagnostics = SearchDiagnostics();
	Utility::print("Starting DC search for ", false);
	Utility::print(board.isWhiteTurn?"white":"black", true);
	runIterativeDeepeningSearch();
	Utility::print("Exiting DC search", true);
}

// Run iterative deepening. This means doing a full search with a depth of 1, then with a depth of 2, and so on.
// This allows the search to be cancelled at any time and still yield a useful result.
void Searcher::runIterativeDeepeningSearch()
{
	for (int searchDepth = 1; searchDepth <= maxDepth; searchDepth++)
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
		}*/
		
		currentDepth = searchDepth;
		bestMove = bestMoveThisIteration;
		bestEval = bestEvalThisIteration;

		Utility::print(std::format(
				"\nIteration {} result: \n{} \nEval: {}\nPOV: {}", 
				searchDepth,
				board.moveToString(bestMove),
				bestEval,
				board.isWhiteTurn?"white":"black"),
			true);
		Utility::print(std::format(
			"Found move at ordering {} / {}",
			searchDiagnostics.bestMoveNumber,
			searchDiagnostics.movesSize
		), true);
		Utility::print(std::format(
			"Number of nodes visited: {} \nNumber of Cutoffs: {} \nNumber of Transposition Table hits: {}",
			searchDiagnostics.numPositionsEvaluated,
			searchDiagnostics.numCutOffs,
			searchDiagnostics.numTranspositionHits
		), true);

		if (isWinScore(bestEval))
		{
			Utility::print(std::format("Mate in ply: {}", numPlyToWinFromScore(bestEval)), true);
		}

		bestEvalThisIteration = negativeInfinity;
		bestMoveThisIteration = Utility::createNullMove();

		// Update diagnostics
		searchDiagnostics.numCompletedIterations = searchDepth;
		searchDiagnostics.move = board.moveToString(bestMove);
		searchDiagnostics.eval = bestEval;
		// Exit search if found a win within search depth.
		// A win found outside of search depth (due to extensions) may not be the fastest win.
		if (isWinScore(bestEval) && numPlyToWinFromScore(bestEval) <= searchDepth)
		{
			Utility::print("Exiting search due to mate found within search depth.", true);
			break;
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
		searchDiagnostics.numTranspositionHits++;
		return ttVal;
	}

	if (plyRemaining == 0 || board.gameResult != GameResult::InProgress)
	{
		searchDiagnostics.numPositionsEvaluated++;
		int eval = quiescenceSearch(alpha, beta);
			//evaluation.evaluate(board);
		return eval;
	}

	std::vector<Move> moves = std::vector<Move>();
	moveGenerator.getMoves(&moves, board.square, board.isWhiteTurn);

	Move prevBestMove = plyFromRoot == 0 ? bestMove : transpositionTable.tryGetStoredMove();
	std::vector<int> moveIndices = moveOrdering.makeMoveOrdering(prevBestMove, board, moves);

	//add passing move if there are no moves left
	if (moves.size() == 0)
	{
		/*if (Utility::isWon()){}*/
		//int winScore = immediateWinScore - plyFromRoot;
		//return -winScore;
		moves.emplace_back(Utility::createNullMove());
		moveIndices.push_back(0);
	}

	int evaluationBound = TranspositionTable<1>::UpperBound;
	Move* bestMoveInThisPosition = nullptr;

	for (int moveNumber = 0; moveNumber < moves.size(); moveNumber++)
	{
		const int i = moveIndices[moveNumber];

		board.makeMove(moves[i]);

		const int eval = -search(plyRemaining - 1, plyFromRoot + 1, -beta, -alpha);

		board.unmakeMove(moves[i]);

		// Move was *too* good, opponent will choose a different move earlier on to avoid this position.
		// (Beta-cutoff / Fail high)
		if (eval >= beta)
		{
			// Store evaluation in transposition table. Note that since we're exiting the search early, there may be an
			// even better move we haven't looked at yet, and so the current eval is a lower bound on the actual eval.
			transpositionTable.storeEvaluation(plyRemaining, plyFromRoot, beta, TranspositionTable<1>::LowerBound, moves[i]);

			searchDiagnostics.numCutOffs++;
			return beta;
		}

		// Found a new best move in this position
		if (eval > alpha )//|| (eval == alpha && plyFromRoot == 0 && (rand() % 10 == 1)))
		{
			evaluationBound = TranspositionTable<1>::Exact;
			bestMoveInThisPosition = &moves[i];

			alpha = eval;
			if (plyFromRoot == 0)
			{
				bestMoveThisIteration = moves[i];
				bestEvalThisIteration = eval;
				searchDiagnostics.bestMoveNumber = moveNumber;
				searchDiagnostics.movesSize = moves.size();
			}
		}
	}
	if (bestMoveInThisPosition != nullptr)
		transpositionTable.storeEvaluation(plyRemaining, plyFromRoot, alpha, evaluationBound, *bestMoveInThisPosition);

	return alpha;
}

// Search capture moves until a 'quiet' position is reached.
int Searcher::quiescenceSearch(int alpha, int beta, bool madeQMove)
{
	// A player isn't forced to make a capture (typically), so see what the evaluation is without capturing anything.
	int eval = evaluation.evaluate(board);
	searchDiagnostics.numPositionsEvaluated++;

	if (board.gameResult != GameResult::InProgress)
	{
		if (eval >= beta)
		{
			searchDiagnostics.numCutOffs++;
			return beta;
		}
		return max(alpha, eval);
	}

	 // Continue until position is quiet for both players
	//if (!madeQMove) {
	//	Move move = Utility::createNullMove();

	//	board.makeMove(move);
	//	eval = -quiescenceSearch(-beta, -alpha, /*madeQMove: */true);
	//	board.unmakeMove(move);
	//}
	
	if (eval >= beta)
	{
		searchDiagnostics.numCutOffs++;
		return beta;
	}
	if (eval > alpha)
	{
		alpha = eval;
	}

	std::vector<Move> moves;
	moveGenerator.getMoves(&moves, board.square, board.isWhiteTurn, true);
	std::vector<int> moveIndices = moveOrdering.makeMoveOrdering(Utility::createNullMove(), board, moves);

	for (int moveNumber = 0; moveNumber < moves.size(); moveNumber++)
	{
		const int i = moveIndices[moveNumber];
		Move* move = &(moves[i]);

		board.makeMove(*move);
		eval = -quiescenceSearch(-beta, -alpha, madeQMove);
		board.unmakeMove(*move);

		if (eval >= beta)
		{
			searchDiagnostics.numCutOffs++;
			return beta;
		}
		if (eval > alpha)
		{
			alpha = eval;
		}
	}
	
	return alpha;
}

std::pair<Move, int> Searcher::getSearchResult()
{
	return std::pair<Move, int>(bestMove, bestEval);
}


void Searcher::sortMoves(shared_ptr<vector<Move>> moves)
{
	//ideal would be in order:
	// - merges onto addon
	// - captures of addOn
	// - normal captures
	// - moves with addOn
	// - the rest
	auto compareMoves = [](const Move& i1, const Move& i2) {
		return i1.size() > i2.size();
	};
	sort(moves.get()->begin(), moves.get()->end(), compareMoves);
}