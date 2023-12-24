#include "Deepchad.h"
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
	int depth = 2;
	shared_ptr<vector<vector<BasicGenerator::xMove>>> moves =
		BasicGenerator::getMoves(isWhite, board);
	vector<BasicGenerator::xMove>* bestMove = nullptr;
	if (isWhite)
	{
		float value = -numeric_limits<float>::infinity();
		for (int i = 1; i < moves->size(); i++)
		{
			applyXMove(board, moves->at(i));
			float eval = minimax(board, depth - 1, false);
			if (eval > value) {
				value = eval;
				bestMove = &(moves->at(i));
			}
			applyXMove(board, moves->at(i));
		}
	}
	else
	{
		float value = numeric_limits<float>::infinity();
		for (int i = 1; i < moves->size(); i++)
		{
			applyXMove(board, moves->at(i));
			float eval = minimax(board, depth - 1, false);
			if (eval < value) {
				value = eval;
				bestMove = &(moves->at(i));
			}
			applyXMove(board, moves->at(i));
		}
	}
	if (bestMove != nullptr)
		applyXMove(board, (*bestMove));

	this_thread::sleep_for(chrono::milliseconds(50));
}

float Deepchad::minimax(uint8_t(*board)[13][13], uint8_t depth, bool isWhiteTurn)
{
	if (depth == 0)
		return heuristic(board, isWhiteTurn);

	shared_ptr<vector<vector<BasicGenerator::xMove>>> moves =
		BasicGenerator::getMoves(isWhiteTurn, board);
	if (isWhiteTurn)
	{
		float value = -numeric_limits<float>::infinity();
		for (int i = 1; i < moves->size(); i++)
		{
			applyXMove(board, moves->at(i));
			value = max(value, minimax(board, depth - 1, false));
			applyXMove(board, moves->at(i));
		}
		return value;
	}
	else
	{
		float value = numeric_limits<float>::infinity();
		for (int i = 1; i < moves->size(); i++)
		{
			applyXMove(board, moves->at(i));
			value = min(value, minimax(board, depth - 1, false));
			applyXMove(board, moves->at(i));
		}
		return value;
	}
}

float Deepchad::heuristic(uint8_t(*board)[13][13], bool isWhiteTurn)
{
	int counter = 0;
	for (int x = 0; x < 13; x++)
	{
		for (int y = 0; y < 13; y++)
		{
			uint8_t content = (*board)[x][y];
			if (Piece::isTower(content))
			{
				int delta = 1;
				if (!Piece::isWhite(content))
					delta *= -1;
				delta *= Piece::height(content);
				counter += delta;
			}
		}
	}
	return counter;
	//return rand() % 100;
}

void Deepchad::applyXMove(uint8_t(*board)[13][13], vector<BasicGenerator::xMove>& move)
{
	for (int i = 0; i < move.size(); i++) {
		(*board)[move[i].i][move[i].j] ^= move[i].delta;
	}
}

/*
(* Initial call *)
minimax(origin, depth, TRUE)
*/