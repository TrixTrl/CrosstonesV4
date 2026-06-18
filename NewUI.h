#pragma once

#include "fix_win32_compatibility.h"
namespace raylib
{
#include "raylib.h"
}

// using namespace std;

const int cellSize = 50;

class RaylibUI
{

public:
    static void init()
    {
        raylib::InitWindow(13 * cellSize, 13 * cellSize, "CrosstonesV4");
        raylib::SetTargetFPS(60);
    }

    static void
    draw()
    {
        while (!raylib::WindowShouldClose())
        {
            raylib::BeginDrawing();
            raylib::ClearBackground(raylib::BLACK);
            for (int j = 0; j < 13; j++)
            {
                for (int i = 0; i < 13; i++)
                {
                    raylib::DrawRectangle(i * cellSize, j * cellSize, cellSize, cellSize, raylib::Color{(unsigned char)(i * 5), (unsigned char)(j * 5), (unsigned char)(0), 255});
                }
            }
            raylib::EndDrawing();
        }
    }
};