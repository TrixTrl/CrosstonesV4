#pragma once

#include <cstdint>
#include <array>
#include <vector>

#include "Utils.h"

#define    hashfEXACT   0
#define    hashfALPHA   1
#define    hashfBETA    2

class TranspositionTable
{
public:
	struct Entry {

		uint64_t key;
		uint8_t depth;
		uint8_t flags;
		float value;
		int best;
	};
	struct tableReturn {
		bool found;
		float value;
		int best;
	};
	static const int sizeMB = 5;
	static const uint64_t count = (uint64_t)((sizeMB * 1024 * 1024) / sizeof(Entry));
	static std::array<Entry, count> table_alwaysReplace;
	static std::array<Entry, count> table_maximizeDepth;

	static void recordHash(uint64_t zobristKey, uint8_t depth, uint8_t flags, float value, int best);
	static Entry probeHash(uint64_t zobristKey);
};

class ZobristHasher {
	static uint64_t piecesData[169][0b111111];
	static uint64_t turnPiecesData[20];
	static uint64_t isWhiteData;
	static bool initialized;
public:
	static void fillValues();
	static uint64_t zobristKey(uint8_t(*board)[13][13], bool isWhite);
	static uint64_t zobristKey(uint8_t(*board)[13][13], uint8_t(*changedBoard)[13][13], std::vector<Utils::xMove>* move, uint64_t hash);
};

/*
	169 * 64 pieces on every square
	20 turn pieces
	1 which side moves



*/