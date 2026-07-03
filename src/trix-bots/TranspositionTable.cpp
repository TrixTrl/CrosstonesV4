#include "TranspositionTable.h"
#include <random>
#include "globals/Piece.h"

uint64_t ZobristHasher::piecesData[169][0b111111];
uint64_t ZobristHasher::turnPiecesData[20];
uint64_t ZobristHasher::isWhiteData;
bool ZobristHasher::initialized = false;

void ZobristHasher::fillValues()
{
	if (initialized) return;
	std::mt19937_64 rnd(0);
	for (int i = 0; i < 169; i++) {
		for (int j = 0; j < 0b111111; j++) {
			if (j == 0) { piecesData[i][j] = 0; continue; }
			piecesData[i][j] = rnd();
		}
	}
	for (int i = 0; i < 20; i++) {
		turnPiecesData[i] = rnd();
	}

	isWhiteData = rnd();

	initialized = true;
}

uint64_t ZobristHasher::zobristKey(uint8_t(*board)[13][13], bool isWhite)
{
	uint64_t output = isWhite ? isWhiteData : 0;
	for (int i = 0; i < 13; i++) {
		for (int j = 0; j < 13; j++) {
			output ^= piecesData[13 * i + j][(*board)[i][j] & 0b111111];
			if (Piece::turnPieceIndices[13 * j + i] == 0) continue;
			output ^= (((*board)[i][j]) & 0b01000000) ? turnPiecesData[Piece::turnPieceIndices[13 * j + i] - 1] : 0;
		}
	}
	return output;
}

uint64_t ZobristHasher::zobristKey(uint8_t(*board)[13][13], uint8_t(*changedBoard)[13][13], std::vector<Utils::xMove>* move, uint64_t hash)
{
	for (int I = 0; I < move->size(); I++) {
		int i = (*move)[I].i;
		int j = (*move)[I].j;
		hash ^= piecesData[13 * i + j][(*board)[i][j] & 0b111111];
		if (Piece::turnPieceIndices[13 * j + i] != 0) hash ^= (((*board)[i][j]) & 0b01000000) ? turnPiecesData[Piece::turnPieceIndices[13 * j + i] - 1] : 0;

		hash ^= piecesData[13 * i + j][(*changedBoard)[i][j] & 0b111111];
		if (Piece::turnPieceIndices[13 * j + i] != 0) hash ^= (((*changedBoard)[i][j]) & 0b01000000) ? turnPiecesData[Piece::turnPieceIndices[13 * j + i] - 1] : 0;
	}
	hash ^= isWhiteData;
	return hash;
}

std::array<TranspositionTable::Entry, TranspositionTable::count> TranspositionTable::table_alwaysReplace;
std::array<TranspositionTable::Entry, TranspositionTable::count> TranspositionTable::table_maximizeDepth;

void TranspositionTable::recordHash(uint64_t zobristKey, uint8_t depth, uint8_t flags, float value, int best)
{
	table_alwaysReplace[zobristKey % count] = Entry(zobristKey, depth, flags, value, best);
	if (depth > table_maximizeDepth[zobristKey % count].depth) {
		table_maximizeDepth[zobristKey % count] = Entry(zobristKey, depth, flags, value, best);
	}
}

TranspositionTable::Entry TranspositionTable::probeHash(uint64_t zobristKey)
{
	if (table_alwaysReplace[zobristKey % count].key == zobristKey) return table_alwaysReplace[zobristKey % count];
	return table_maximizeDepth[zobristKey%count];
}
/*int ProbeHash(int depth, int alpha, int beta)
	{
		HASHE* phashe = &hash_table[ZobristKey() % TableSize()];
		if (phashe->key == ZobristKey()) {
			if (phashe->depth >= depth) {
				if (phashe->flags == hashfEXACT)
					return phashe->val;
				if ((phashe->flags == hashfALPHA) &&
					(phashe->val <= alpha))
					return alpha;
				if ((phashe->flags == hashfBETA) &&
					(phashe->val >= beta))
					return beta;
			}
			RememberBestMove();
		}
		return valUNKNOWN;
	}*/