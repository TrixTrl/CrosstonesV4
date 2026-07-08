#include "RichMoveGenerator.h"
#include "game-suite/Board.h"
#include "globals/Piece.h"
#include <algorithm>

static constexpr int dx[4] = { 0, 1, 0, -1 };
static constexpr int dy[4] = {-1, 0, 1,  0 };
static constexpr uint8_t hasTurnPiece = 128;
static constexpr uint8_t setTurnPiece = 64;

static bool isVoid(int i, int j) { return i % 2 == 1 && j % 2 == 1; }

// ── internal generator ──────────────────────────────────────────────

struct Generator {
    const GamePosition* original;
    int startI, startJ;
    std::vector<PathStep> currentPath;
    bool currentTurned;
    std::vector<MoveInfo> results;

    Generator(const GamePosition* orig, int si, int sj)
        : original(orig), startI(si), startJ(sj), currentTurned(false) {}
};

static void emitMove(Generator& gen, const GamePosition& state) {
    XMove move;
    for (int i = 0; i < 13; i++) {
        for (int j = 0; j < 13; j++) {
            if ((*gen.original)[i][j] == state[i][j])
                continue;
            move.emplace_back(XMoveTile{i, j,
                (uint8_t)((*gen.original)[i][j] ^ state[i][j])});
        }
    }
    if (gen.results.empty() || !move.empty()) {
        MoveInfo info;
        info.startI = gen.startI;
        info.startJ = gen.startJ;
        info.xmove = move;
        info.path = gen.currentPath;
        if (!gen.currentPath.empty()) {
            info.endI = gen.currentPath.back().i;
            info.endJ = gen.currentPath.back().j;
            info.finalEndI = info.endI;
            info.finalEndJ = info.endJ;
            info.finalHeight = Piece::height(state[info.endI][info.endJ]);
            for (auto& p : gen.currentPath) {
                if (p.turned) info.turned = true;
                if (p.pushed) info.pushed = true;
            }
        } else {
            info.endI = gen.startI;
            info.endJ = gen.startJ;
            info.finalEndI = info.endI;
            info.finalEndJ = info.endJ;
            info.finalHeight = Piece::height(state[gen.startI][gen.startJ]);
        }
        gen.results.push_back(std::move(info));
    }
}

static void captureGenerator(
    Generator& gen, const GamePosition& state,
    int originX, int originY, int x, int y,
    Board::VisitedMap& visited, int remainingSteps, bool turned, bool isWhite);

static void basicGenerator(
    Generator& gen, const GamePosition& state,
    int x, int y, Board::VisitedMap& visited,
    int remainingSteps, bool turned, bool isWhite)
{
    emitMove(gen, state);

    uint8_t piece = state[x][y];

    if (!turned && (piece & hasTurnPiece) != 0) {
        GamePosition copy = state;
        copy[x][y] ^= setTurnPiece;
        if (!gen.currentPath.empty())
            gen.currentPath.back().turned = true;
        Board::VisitedMap visitedCopy = visited;
        basicGenerator(gen, copy, x, y, visitedCopy, remainingSteps, true, isWhite);
        if (!gen.currentPath.empty())
            gen.currentPath.back().turned = false;
    }

    if (remainingSteps == 0)
        return;

    for (int d = 0; d < 4; d++) {
        int destX = x + dx[d];
        int destY = y + dy[d];
        uint8_t dest = state[destX][destY];

        if (destX < 0 || destX > 12 || destY < 0 || destY > 12) continue;
        if (isVoid(destX, destY)) continue;
        if (visited[destX][destY]) continue;
        if (Piece::isBlockedByGate(piece, d)) continue;
        if (Piece::isBlockedByGate(dest, d)) continue;

        // ── empty target ──
        if ((dest & 0b00111111) == 0) {
            for (int splitOff = 1; splitOff <= Piece::height(piece); splitOff++) {
                GamePosition boardCopy = state;

                if (splitOff == Piece::height(piece))
                    boardCopy[x][y] &= 0b11000000;
                else {
                    boardCopy[x][y] -= splitOff;
                    if (Piece::hasAddOn(piece) && splitOff == Piece::height(piece) - 1)
                        boardCopy[x][y] &= 0b11011111;
                }
                boardCopy[destX][destY] |= (uint8_t)((piece) & 0b00100000 | splitOff);
                if (splitOff == Piece::height(piece))
                    boardCopy[destX][destY] |= (uint8_t)((piece) & 0b00011000);

                PathStep step;
                step.i = destX;
                step.j = destY;
                step.splitOnwards = (splitOff == Piece::height(piece)) ? 0 : splitOff;
                gen.currentPath.push_back(step);

                Board::VisitedMap visitedCopy = visited;
                visitedCopy[destX][destY] = true;
                basicGenerator(gen, boardCopy, destX, destY, visitedCopy,
                               remainingSteps - 1, false, isWhite);

                gen.currentPath.pop_back();
            }
        }
        // ── merge target ──
        else if (Piece::height(dest) > 0 &&
                 (Piece::isAddOn(dest) ? true :
                  (Piece::colour(dest) == Piece::colour(piece))))
        {
            for (int splitOff = 1;
                 (splitOff <= Piece::height(piece)) &&
                 (splitOff <= 5 - Piece::height(dest));
                 splitOff++)
            {
                if (splitOff == Piece::height(piece) && Piece::hasAddOn(piece))
                    continue;

                GamePosition boardCopy = state;

                if (splitOff == Piece::height(piece))
                    boardCopy[x][y] &= 0b11000000;
                else {
                    boardCopy[x][y] -= splitOff;
                    if (Piece::hasAddOn(piece) && splitOff == Piece::height(piece) - 1)
                        boardCopy[x][y] &= 0b11011111;
                }
                boardCopy[destX][destY] |= (uint8_t)((piece) & 0b00100000);
                boardCopy[destX][destY] += splitOff;
                if (splitOff == Piece::height(piece))
                    boardCopy[destX][destY] |= (uint8_t)((piece) & 0b00011000);

                PathStep step;
                step.i = destX;
                step.j = destY;
                step.splitOnwards = 0;
                gen.currentPath.push_back(step);

                Board::VisitedMap visitedCopy = visited;
                visitedCopy[destX][destY] = true;
                basicGenerator(gen, boardCopy, destX, destY, visitedCopy,
                               0, false, isWhite);

                gen.currentPath.pop_back();
            }
        }

        // ── push target ──
        if (Piece::height(dest) > 0 &&
            Piece::height(dest) <= Piece::height(piece) &&
            (Piece::isAddOn(dest) ? false :
             (Piece::colour(dest) == Piece::colour(piece))))
        {
            int offset = 2;
            bool pushValid = false;
            int pushHeight = Piece::height(dest);

            while (true) {
                int pushX = x + dx[d] * offset;
                int pushY = y + dy[d] * offset;

                if (pushX < 0 || pushX > 12 || pushY < 0 || pushY > 12)
                    break;

                uint8_t pushPiece = state[pushX][pushY];

                if ((pushPiece & hasTurnPiece) != 0) {
                    if (((pushPiece & setTurnPiece)) == (d % 2) * setTurnPiece)
                        break;
                }
                if (Piece::height(pushPiece) == 0) {
                    pushValid = true;
                    break;
                }
                if (Piece::height(pushPiece) <= pushHeight &&
                    (Piece::isAddOn(pushPiece) ? false :
                     (Piece::colour(pushPiece) == Piece::colour(piece))))
                {
                    offset++;
                    pushHeight = Piece::height(pushPiece);
                } else {
                    break;
                }
            }
            if (pushValid) {
                GamePosition boardCopy = state;

                for (int o = offset; o > 0; o--) {
                    int dX = x + dx[d] * o;
                    int dY = y + dy[d] * o;
                    int sX = x + dx[d] * (o - 1);
                    int sY = y + dy[d] * (o - 1);
                    boardCopy[dX][dY] |= boardCopy[sX][sY] & 0b00111111;
                    boardCopy[sX][sY] &= 0b11000000;
                }

                PathStep step;
                step.i = destX;
                step.j = destY;
                step.splitOnwards = Piece::height(piece);
                step.pushed = true;
                gen.currentPath.push_back(step);

                Board::VisitedMap visitedCopy = visited;
                visitedCopy[destX][destY] = true;
                basicGenerator(gen, boardCopy, destX, destY, visitedCopy,
                               remainingSteps - 1, false, isWhite);

                gen.currentPath.pop_back();
            }
        }

        // ── capture target ──
        if (remainingSteps >= 2 &&
            Piece::isTower(dest) &&
            Piece::colour(dest) != Piece::colour(piece))
        {
            if (!Piece::isBlue(piece) && Piece::height(dest) > Piece::height(piece))
                continue;

            GamePosition boardCopy = state;
            boardCopy[destX][destY] &= 0b11000000;

            Board::VisitedMap visitedCopy = visited;
            visitedCopy[destX][destY] = true;
            captureGenerator(gen, boardCopy, x, y, destX, destY,
                             visitedCopy, remainingSteps - 1, false, isWhite);
        }
    }
}

static void captureGenerator(
    Generator& gen, const GamePosition& state,
    int originX, int originY, int x, int y,
    Board::VisitedMap& visited, int remainingSteps, bool turned, bool isWhite)
{
    uint8_t origin = state[originX][originY];


    if (!turned && ((state[x][y] & hasTurnPiece) != 0)) {
        GamePosition boardCopy = state;
        boardCopy[x][y] ^= setTurnPiece;
        if (!gen.currentPath.empty())
            gen.currentPath.back().turned = true;
        Board::VisitedMap visitedCopy = visited;
        captureGenerator(gen, boardCopy, originX, originY, x, y,
                         visitedCopy, remainingSteps, true, isWhite);
        if (!gen.currentPath.empty())
            gen.currentPath.back().turned = false;
    }

    for (int d = 0; d < 4; d++) {
        uint8_t piece = state[x][y];
        if ((piece & hasTurnPiece) != 0) {
            if (((piece & setTurnPiece)) == (d % 2) * setTurnPiece)
                continue;
        }
        int i = x + dx[d];
        int j = y + dy[d];
        uint8_t dest = state[i][j];
        if (i < 0 || i > 12 || j < 0 || j > 12) continue;
        if (isVoid(i, j)) continue;
        if (visited[i][j]) continue;
        if ((dest & hasTurnPiece) != 0 &&
            (((dest & setTurnPiece)) == (d % 2) * setTurnPiece))
            continue;

        if (Piece::height(dest) == 0 ||
            (Piece::isTower(dest) && Piece::colour(dest) == Piece::colour(origin)) ||
            Piece::isAddOn(dest))
        {
            for (int splitOff = 1;
                 (Piece::height(origin) == 1 ? splitOff <= 1 :
                  (splitOff < Piece::height(origin))) &&
                 (splitOff <= 5 - Piece::height(dest));
                 splitOff++)
            {
                GamePosition boardCopy = state;

                if (Piece::height(origin) == 1)
                    boardCopy[originX][originY] &= 0b11000000;
                else {
                    boardCopy[originX][originY] -= splitOff;
                    if (Piece::isAddOn(boardCopy[originX][originY]))
                        boardCopy[originX][originY] &= 0b11011111;
                }
                boardCopy[i][j] += splitOff;
                boardCopy[i][j] |= Piece::colour(origin);

                PathStep capStep;
                capStep.i = x;
                capStep.j = y;
                capStep.captured = true;
                capStep.capturedHeight = Piece::height(
                    state[x][y] ^ (0));

                PathStep splitStep;
                splitStep.i = i;
                splitStep.j = j;
                splitStep.splitOnwards = 0;
                gen.currentPath.push_back(capStep);
                gen.currentPath.push_back(splitStep);

                Board::VisitedMap visitedCopy = visited;
                visitedCopy[i][j] = true;
                basicGenerator(gen, boardCopy, i, j, visitedCopy,
                               0, false, isWhite);

                gen.currentPath.pop_back();
                gen.currentPath.pop_back();
            }
        }
        else if (remainingSteps + Piece::isBlue(origin) > 1 &&
                 Piece::isTower(dest) &&
                 Piece::colour(origin) != Piece::colour(dest))
        {
            if (!Piece::isBlue(origin) && Piece::height(dest) > Piece::height(origin))
                continue;

            GamePosition boardCopy = state;
            boardCopy[i][j] &= 0b11000000;

            PathStep capStep;
            capStep.i = i;
            capStep.j = j;
            capStep.captured = true;
            capStep.capturedHeight = Piece::height(dest);
            gen.currentPath.push_back(capStep);

            Board::VisitedMap visitedCopy = visited;
            visitedCopy[i][j] = true;
            captureGenerator(gen, boardCopy, originX, originY, i, j,
                             visitedCopy,
                             remainingSteps - !Piece::isBlue(origin),
                             false, isWhite);

            gen.currentPath.pop_back();
        }
    }
}

// ── public API ───────────────────────────────────────────────────────

std::vector<MoveInfo> RichMoveGenerator::generate(const GamePosition& pos, bool isWhite) {
    std::vector<MoveInfo> result;

    for (int i = 0; i < 13; i++) {
        for (int j = 0; j < 13; j++) {
            uint8_t piece = pos[i][j];
            if (Piece::height(piece) == 0 || Piece::isAddOn(piece))
                continue;
            if (Piece::isWhiteTower(piece) == !isWhite)
                continue;

            Generator gen(&pos, i, j);

            GamePosition posCopy = pos;
            Board::VisitedMap visited;
            visited[i][j] = true;

            basicGenerator(gen, posCopy, i, j, visited,
                           Piece::maxSteps(piece), false, isWhite);

            for (size_t k = 1; k < gen.results.size(); k++)
                result.push_back(std::move(gen.results[k]));
        }
    }
    return result;
}
