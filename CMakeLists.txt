cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)

project(CROSSTONES_V4)

add_executable(Crosstones_V4 Main.cpp DCTestSuite.cpp BoardState.cpp Globals/Piece.cpp GameMaster.cpp ManualPlayer.cpp "Test Bots/TestBotRnd.cpp" "Felix Bots/Deepchad.cpp" "Trix Bots/TheFirst.cpp" "Trix Bots/Utils.cpp" "Trix Bots/DemoKnight.cpp" "Trix Bots/Hydra.cpp" Globals/PlayerInputKey.h UI.cpp "Felix Bots/Board.cpp" "Felix Bots/Evaluation.cpp" "Felix Bots/DebugPrinting.cpp" "Felix Bots/MoveGenerator.cpp" "Felix Bots/MoveOrdering.cpp" "Felix Bots/TranspositionTable.h" "Felix Bots/Utility.cpp" "Felix Bots/Zobrist.cpp" BoardState.h "Felix Bots/Searcher.cpp" "Trix Bots/TranspositionTable.cpp" "Trix Bots/Utils.h" "Felix Bots/MoveGenerator.cpp" "Test Bots/BasicGenerator.cpp" "Trix Bots/MCTS.cpp" "Trix Bots/AlphaCruncher.cpp")