#pragma once
#include <cstdint>
#include "Board.h"
#include <array>
#include "../Test Bots/BasicGenerator.h"
#include "Utility.h"
#include "Searcher.h"

typedef std::vector<BasicGenerator::xMove> Move;

namespace dc
{
	template <int sizeMB>
	class TranspositionTable
	{
		// Reference https://web.archive.org/web/20071031100051/http://www.brucemo.com/compchess/programming/hashing.htm

	public:
		
		struct Entry
		{
			uint64_t key;
			int value;
			Move move;
			uint8_t depth; // depth is how many ply were searched ahead from this position
			uint8_t nodeType;

			Entry() : key(0), value(0), depth(0), nodeType(0) {}

			Entry(uint64_t _key, int _value, uint8_t _depth, uint8_t _nodeType, Move& _move)
				: key(_key), value(_value), depth(_depth), nodeType(_nodeType), move() 
			{
				for (auto& xMove : _move)
				{
					move.push_back(xMove);
				}
			}
		};

	private:

		// Amount of entries
		static const uint64_t count = (uint64_t)((sizeMB * 1024 * 1024) / sizeof(Entry));
		Board& board;

	public:

		static const int LookupFailed = -1;

		// The value for this position is the exact evaluation
		static const int Exact = 0;
		// A move was found during the search that was too good, meaning the opponent will play a different move earlier on,
		// not allowing the position where this move was available to be reached. Because the search cuts off at
		// this point (beta cut-off), an even better move may exist. This means that the evaluation for the
		// position could be even higher, making the stored value the lower bound of the actual value.
		static const int LowerBound = 1;
		// No move during the search resulted in a position that was better than the current player could get from playing a
		// different move in an earlier position (i.e eval was <= alpha for all moves in the position).
		// Due to the way alpha-beta search works, the value we get here won't be the exact evaluation of the position,
		// but rather the upper bound of the evaluation. This means that the evaluation is, at most, equal to this value.
		static const int UpperBound = 2;

		std::array<Entry, count> entries;

		bool enabled = true;

		TranspositionTable(Board& _board) : board(_board) {};

		void clear()
		{
			entries.fill(Entry());
		}

		inline uint64_t index()
		{
			return board.zobristKey % count;
		}

		const Move tryGetStoredMove()
		{
			return entries[index()].move;
		}

		const int tryGetEvalForMove()
		{
			return entries[index()].value;
		}


		int lookupEvaluation(int depth, int plyFromRoot, int alpha, int beta)
		{
			if (!enabled)
			{
				return LookupFailed;
			}
			Entry entry = entries[index()];

			if (entry.key == board.zobristKey)
			{
				// Only use stored evaluation if it has been searched to at least the same depth as would be searched now
				if (entry.depth >= depth)
				{
					int correctedScore = correctRetrievedMateScore(entry.value, plyFromRoot);
					// We have stored the exact evaluation for this position, so return it
					if (entry.nodeType == Exact)
					{
						return correctedScore;
					}
					// We have stored the upper bound of the eval for this position. If it's less than alpha then we don't need to
					// search the moves in this position as they won't interest us; otherwise we will have to search to find the exact value
					if (entry.nodeType == UpperBound && correctedScore <= alpha)
					{
						return correctedScore;
					}
					// We have stored the lower bound of the eval for this position. Only return if it causes a beta cut-off.
					if (entry.nodeType == LowerBound && correctedScore >= beta)
					{
						return correctedScore;
					}
				}
			}
			return LookupFailed;
		}

		void storeEvaluation(int depth, int numPlySearched, int eval, int evalType, Move& move)
		{
			if (!enabled)
			{
				return;
			}
			//if (depth >= entries[Index()].depth) {
			Entry entry = Entry(board.zobristKey, correctMateScoreForStorage(eval, numPlySearched), (uint8_t)depth, (uint8_t)evalType, move);
			entries[index()] = entry;
			//}
		}

		int correctMateScoreForStorage(int score, int numPlySearched)
		{
			if (Utility::isWinScore(score))
			{
				int sign = Utility::sgn(score);
				return score + numPlySearched * sign;
			}
			return score;
		}

		int correctRetrievedMateScore(int score, int numPlySearched)
		{
			if (Utility::isWinScore(score))
			{
				int sign = Utility::sgn(score);
				return score + numPlySearched * sign;
			}
			return score;
		}

		Entry* getEntry(uint64_t zobristKey)
		{
			return &entries[zobristKey % (uint64_t)entries.size()];
		}
	};
}