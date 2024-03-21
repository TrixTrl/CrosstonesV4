#pragma once

#include <cstdint>
#include <string>

#include "../Test Bots/BasicGenerator.h"

class Utils
{
public:
	enum class winValue {
		none,
		white,
		black,
		draw
	};

	struct extraBoardData {
		uint64_t colorBitboards[2][3];

		extraBoardData() {
			colorBitboards[0][0] = 0;
			colorBitboards[0][1] = 0;
			colorBitboards[0][2] = 0;
			colorBitboards[1][0] = 0;
			colorBitboards[1][1] = 0;
			colorBitboards[1][2] = 0;
		};

		extraBoardData(extraBoardData* data) {
			colorBitboards[0][0] = data->colorBitboards[0][0];
			colorBitboards[0][1] = data->colorBitboards[0][1];
			colorBitboards[0][2] = data->colorBitboards[0][2];
			colorBitboards[1][0] = data->colorBitboards[1][0];
			colorBitboards[1][1] = data->colorBitboards[1][1];
			colorBitboards[1][2] = data->colorBitboards[1][2];
		};
	};

	struct debugContainer {
		int n = 0;
		int depthCounts[5] = { 0, 0, 0, 0, 0 };
		int exact = 0;
		int alpha = 0;
		int beta = 0;
		int sanityCheck = 0;
		int alphaCutoff = 0;
		int betaCutoff = 0;
	};

	struct xMove {
		int i;
		int j;
		uint8_t delta;
		xMove(int I, int J, uint8_t D) : i(I), j(J), delta(D) {}
	};

	static extraBoardData generateMetadata(uint8_t(*pieces)[13][13]);

	static winValue gameOver(bool isWhite, extraBoardData metaData);
	static winValue gameOver(bool isWhite, uint8_t(*pieces)[13][13]);

	static float basicPosEval(bool isWhite, uint8_t(*pieces)[13][13]);
	static float improvedPosEval(bool isWhite, uint8_t(*pieces)[13][13]);

	static int getBestMoveBasic(bool isWhite, uint8_t(*pieces)[13][13], int depth);
	static int trivialBestMove(bool isWhite, uint8_t(*pieces)[13][13]);
	static int getBestMoveThreaded(bool isWhite, uint8_t(*pieces)[13][13], int depth, int threadCount);

	static void evaluatePositionsThread(bool isWhite, uint8_t(*pieces)[13][13], std::shared_ptr<std::vector<std::vector<BasicGenerator::xMove>>> moves, std::vector<float>& evaluations, int depth, int threadIndex, int threadCount, debugContainer& debug);

	static void print(std::string s, bool newLine = false);
	static void print(int s, bool newLine = false);
	static void print(size_t s, bool newLine = false);
	static void print(float s, bool newLine = false);
	static void print(BasicGenerator::xMove, bool newLine = false);
	static void print(extraBoardData, bool formatted = true);
	static void print(std::string s, int n, bool newLine = false);
	static void print(std::string s, float n, bool newLine = false);
	static void print(std::string s, size_t n, bool newLine = false);

	static std::shared_ptr<std::vector<std::vector<xMove>>> getMoves_halfSplit_noPush(bool isWhite, uint8_t(*pieces)[13][13]);
	static void basicGenerator_halfSplit_noPush(std::shared_ptr<std::vector<std::vector<xMove>>> moves, uint8_t(*state)[13][13], int x, int y, bool(*visited)[13][13], int remainingSteps, bool turned, bool isWhite, uint8_t(*pieces)[13][13]);
	static void captureGenerator_singleSplit(std::shared_ptr<std::vector<std::vector<xMove>>> moves, uint8_t(*state)[13][13], int originX, int originY, int x, int y, bool(*visited)[13][13], int remainingSteps, bool turned, bool isWhite, uint8_t(*pieces)[13][13]);
	
	/*
	std::shared_ptr<std::vector<std::vector<xMove>>> getMoves_qiessence(bool isWhite) const;
	void basicGenerator(std::shared_ptr<std::vector<std::vector<xMove>>> moves, uint8_t(*state)[13][13], int x, int y, bool(*visited)[13][13], int remainingSteps, bool turned, bool isWhite) const;
	void captureGenerator(std::shared_ptr<std::vector<std::vector<xMove>>> moves, uint8_t(*state)[13][13], int originX, int originY, int x, int y, bool(*visited)[13][13], int remainingSteps, bool turned, bool isWhite) const;
	*/
	
	static const uint8_t hasTurnPiece = 128;
	static const uint8_t setTurnPiece = 64;

	static float alphaBeta(uint8_t(*pieces)[13][13], int depth, float alpha, float beta, bool maximizing, float (*evalFunc)(bool isWhite, uint8_t(*pieces)[13][13]), debugContainer* debug = nullptr);
	static float alphaBeta_wTable(uint8_t(*pieces)[13][13], int depth, float alpha, float beta, bool maximizing, float (*evalFunc)(bool isWhite, uint8_t(*pieces)[13][13]), uint64_t zobristKey, debugContainer* debug = nullptr);
};

