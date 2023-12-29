#include "Deepchad.h"
#include "DCUtils.h"
#include "DCUtils.cpp"
#include "../Test Bots/BasicGenerator.h"
#include "../Globals/Player.h"
#include "../Globals/Piece.h"
#include <algorithm>
#include <iostream>
#include <thread>
#include <chrono>
#include <limits>

using namespace std;


void Deepchad::getMoveToPlay(uint8_t(*board)[13][13], bool isWhite, int endTime)
{
	DCUtils::print("this is windows test" , true);
	cout << "this is cout test\n";
	int maxDepth = 3,
		color = (isWhite ? 1 : -1);

	constexpr float alpha = -numeric_limits<float>::infinity(),
		beta = numeric_limits<float>::infinity();

	shared_ptr<vector<vector<BasicGenerator::xMove>>> moves =
		BasicGenerator::getMoves(isWhite, board);
	/*vector<BasicGenerator::xMove>* bestMove = nullptr;
	float eval;
	if (isWhite)
	{
		float value = -numeric_limits<float>::infinity();
		for (int i = 1; i < moves->size(); i++)
		{
			DCUtils::wrapWithXMove(board, moves->at(i), [&]() {
				eval = minimax(board, depth - 1, alpha, beta, false);
			});
			if (eval > value) {
				value = eval;
				bestMove = &(moves->at(i));
			}
			if (value > beta)
				break; // beta cutoff
			alpha = max(alpha, value);
		}
	}
	else
	{
		float value = numeric_limits<float>::infinity();
		for (int i = 1; i < moves->size(); i++)
		{
			DCUtils::wrapWithXMove(board, moves->at(i), [&]() {
				eval = minimax(board, depth - 1, alpha, beta, true);
			});
			if (eval < value) {
				value = eval;
				bestMove = &(moves->at(i));
			}
			if (value < alpha)
				break; // alpha cutoff
			beta = min(beta, value);
		}
	}
	if (bestMove != nullptr)
		DCUtils::applyXMove(board, (*bestMove));*/

	// remove the passing move
	moves->erase(moves->begin());

	for (int depth = 1; depth <= maxDepth; depth++) {
		int bestMoveIdx = negamaxAtRoot(board, moves, depth, alpha, beta, color);
		auto bestMove = moves->at(bestMoveIdx);

		//move best move from iteration to front
		moves->erase(moves->begin() + bestMoveIdx);
		moves->insert(moves->begin(), bestMove);
	}
	if (!moves->empty())
		DCUtils::applyXMove(board, moves->front());

	this_thread::sleep_for(chrono::milliseconds(100));
}

float Deepchad::minimax(uint8_t(*board)[13][13], uint8_t depth, float alpha, float beta, bool isWhiteTurn)
{
	if (depth == 0) // or is Terminal node
		return heuristic(board, isWhiteTurn);

	shared_ptr<vector<vector<BasicGenerator::xMove>>> moves =
		BasicGenerator::getMoves(isWhiteTurn, board);
	if (isWhiteTurn)
	{
		float value = -numeric_limits<float>::infinity();
		for (int i = 1; i < moves->size(); i++)
		{
			DCUtils::wrapWithXMove(board, moves->at(i), [&]() {
				value = minimax(board, depth - 1, alpha, beta, false);
			});
			if (value > beta)
				break; // beta cutoff
			alpha = max(alpha, value);
		}
		return value;
	}
	else
	{
		float value = numeric_limits<float>::infinity();
		for (int i = 1; i < moves->size(); i++)
		{
			DCUtils::wrapWithXMove(board, moves->at(i), [&]() {
				value = minimax(board, depth - 1, alpha, beta, true);
			});
			if (value < alpha)
				break; // alpha cutoff
			beta = min(beta, value);
		}
		return value;
	}
}

/* color: 1 when white, -1 when black */
float Deepchad::negamax(uint8_t(*board)[13][13], uint8_t depth, float alpha, float beta, int color)
{
	bool isWhite = (color == 1);

	if (depth == 0 || DCUtils::gameOver(board))
		return heuristic(board, isWhite) * color;

	shared_ptr<vector<vector<BasicGenerator::xMove>>> moves =
		BasicGenerator::getMoves(isWhite, board);
	float value = -numeric_limits<float>::infinity();

	for (int i = 1; i < moves->size(); i++)
	{
		DCUtils::wrapWithXMove(board, moves->at(i), 
			[&]() {
				value = max(value, -negamax(board, depth - 1, -beta, -alpha, -color));
			});
		alpha = max(alpha, value);
		if (alpha >= beta)
			break; // cutoff
	}
	return value;
}

/* returns index of best move*/
int Deepchad::negamaxAtRoot(uint8_t(*board)[13][13], shared_ptr<vector<vector<BasicGenerator::xMove>>> moves, 
	uint8_t depth, float alpha, float beta, int color)
{
	bool isWhite = (color == 1);
	
	if (depth == 0 || DCUtils::gameOver(board))
		return heuristic(board, isWhite) * color;

	int bestMove = -1;
	float eval, value = -numeric_limits<float>::infinity();

	for (int i = 0; i < moves->size(); i++)
	{
		DCUtils::wrapWithXMove(board, moves->at(i), [&]() {
			eval = -negamax(board, depth - 1, -beta, -alpha, -color);
			});
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

float Deepchad::heuristic(uint8_t(*board)[13][13], bool isWhiteTurn)
{
	auto winValue = DCUtils::calcWinValue(board, isWhiteTurn);
	if (winValue != DCUtils::WinValue::none) {
		switch (winValue)
		{
		case DCUtils::WinValue::white:
			return numeric_limits<float>::infinity();
			break;
		case DCUtils::WinValue::black:
			return -numeric_limits<float>::infinity();
			break;
		case DCUtils::WinValue::draw:
			return 0;
			break;
		}
	}
	int blackWhitePoints[2] = { 0, 0 }; // 0: points for black; 1: points for white
	for (int x = 0; x < 13; x++)
	{
		for (int y = 0; y < 13; y++)
		{
			uint8_t content = (*board)[x][y];
			if (Piece::isTower(content))
			{
				int blackWhiteIndex = (Piece::isWhite(content) ? 1 : 0);
				int pieceVal = Piece::height(content)
					+ (Piece::isBlue(content) ? 1 : 0)
					+ (Piece::isRed(content) ? 3 : 0);
				blackWhitePoints[blackWhiteIndex] += pieceVal;
			}
		}
	}
	float relativeStrength = 5 * ((blackWhitePoints[1] > blackWhitePoints[0]) 
		? (blackWhitePoints[1] + 0.01) / (blackWhitePoints[0] + 0.01)
		: -(blackWhitePoints[0] + 0.01) / (blackWhitePoints[1] + 0.01));

	return blackWhitePoints[1] - blackWhitePoints[0] + relativeStrength;
	//return rand() % 100;
}


/*
(* Initial call *)
minimax(origin, depth, TRUE)
*/