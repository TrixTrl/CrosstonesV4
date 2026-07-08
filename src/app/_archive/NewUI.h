#pragma once

#include "fix_win32_compatibility.h"
namespace raylib
{
#include "raylib.h"
}
#include "trix-bots/Utils.h"
#include "globals/Piece.h"

// using namespace std;

const int cellSize = 75;

class RaylibUI
{

    static raylib::Color color(float r, float g, float b)
    {
        return raylib::Color{(unsigned char)r, (unsigned char)g, (unsigned char)b, 255};
    }

public:
    static void init()
    {
        raylib::InitWindow(13 * cellSize, 13 * cellSize, "CrosstonesV4");
        raylib::SetTargetFPS(60);
        raylib::BeginDrawing();
        raylib::ClearBackground(raylib::BLUE);
        raylib::EndDrawing();
    }

    static void
    draw(uint8_t (*displayBoard)[13][13])
    {
        std::string drawTypes[] = {
            "ahthbht",
            "v v b v",
            "ththbht",
            "v v b v",
            "bbbbbbb",
            "b v v b",
            "ahththa"};

        enum DrawPath
        {
            NONE = 0,
            HORIZONTAL,
            VERTICAL
        };

        while (!raylib::WindowShouldClose())
        {
            // Utils::print("Frame", true);
            raylib::BeginDrawing();
            raylib::ClearBackground(raylib::GREEN);
            for (int i = 0; i < 13; i++)
            {
                for (int j = 0; j < 13; j++)
                {
                    // raylib::DrawRectangle(i * cellSize, j * cellSize, cellSize, cellSize, raylib::Color{(unsigned char)(i * 10), (unsigned char)(j * 10), (unsigned char)(255), 255});
                    raylib::Color col;
                    DrawPath path = DrawPath::NONE;
                    uint8_t piece = (*displayBoard)[i][j];
                    switch (drawTypes[j < 7 ? j : 5 - (j % 7)][i < 7 ? i : 5 - (i % 7)])
                    {
                    case 'a':
                        col = color(63, 45, 35);
                        break;
                    case ' ':
                        col = color(122, 70, 58);
                        break;
                    case 'h':
                        col = color(214, 176, 116);
                        path = DrawPath::HORIZONTAL;
                        break;
                    case 'v':
                        col = color(214, 176, 116);
                        path = DrawPath::VERTICAL;
                        break;
                    case 'b':
                        col = color(231, 176, 88);
                        break;
                    case 't':
                        col = color(214, 176, 116);
                        path = (piece & Board::setTurnPiece) == 0 ? DrawPath::HORIZONTAL : DrawPath::VERTICAL;
                        break;
                    default:
                        col = color(255, 0, 0);
                        break;
                    }
                    raylib::DrawRectangle(i * cellSize, j * cellSize, cellSize, cellSize, col);
                    switch (path)
                    {
                    case DrawPath::NONE:
                        break;
                    case DrawPath::HORIZONTAL:
                        col = color(63, 45, 35);
                        raylib::DrawRectangle(i * cellSize, (float(j) + 1.0 / 3.0) * float(cellSize), cellSize, cellSize / 3.0, col);
                        break;
                    case DrawPath::VERTICAL:
                        col = color(63, 45, 35);
                        raylib::DrawRectangle((float(i) + 1.0 / 3.0) * cellSize, j * cellSize, cellSize / 3.0, cellSize, col);
                        break;
                    }

                    if (Piece::tower(piece))
                    {
                        float piecePadding = 0.2;
                        float pieceBorder = 0.1;
                        float textScale = 0.7;
                        raylib::Color colors[4][2] = {{color(15, 15, 15), color(250, 250, 250)}, {color(250, 250, 250), color(15, 15, 15)}, {color(224, 36, 36), color(230, 37, 37)}, {color(31, 20, 250), color(44, 35, 219)}};
                        int ringColorIndex = 0;
                        int centerColorIndex = 0;

                        if (Piece::isRed(piece))
                        {
                            ringColorIndex = 2;
                        }
                        else if (Piece::isBlue(piece))
                        {
                            ringColorIndex = 3;
                        }

                        if (Piece::isAddOn(piece))
                        {
                            centerColorIndex = ringColorIndex;
                        }
                        else if (Piece::isWhite(piece))
                        {
                            centerColorIndex = 1;
                            if (ringColorIndex == 0)
                                ringColorIndex = 1;
                        }

                        raylib::DrawEllipse((float(i) + .5) * cellSize, (float(j) + .5) * cellSize, float(cellSize) / 2.0 * (1 - piecePadding), float(cellSize) / 2.0 * (1 - piecePadding), colors[ringColorIndex][1]);
                        raylib::DrawEllipse((float(i) + .5) * cellSize, (float(j) + .5) * cellSize, float(cellSize) / 2.0 * (1 - piecePadding - pieceBorder), float(cellSize) / 2.0 * (1 - piecePadding - pieceBorder), colors[centerColorIndex][0]);

                        if (Piece::isAddOn(piece))
                            continue;

                        float textWidth = raylib::MeasureText(std::to_string(Piece::height(piece)).c_str(), cellSize * textScale);
                        raylib::DrawText(std::to_string(Piece::height(piece)).c_str(), (float(i) + 0.5) * cellSize - textWidth / 2.0, (float(j) + (1 - textScale) / 2.0) * cellSize, cellSize * textScale, colors[(centerColorIndex + 1) % 2][0]);
                    }
                }
            }
            raylib::EndDrawing();
        }
    }
};