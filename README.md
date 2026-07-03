# Crosstones V4

A chess-like game played on a 13×13 board.

## Prerequisites

- Visual Studio 2022+ (with "Desktop development with C++")
- [vcpkg](https://vcpkg.io) installed at `C:\Program Files\vcpkg` (or set `VCPKG_ROOT` env var)
- [CMake](https://cmake.org) 3.20+

## Setup

```cmd
git clone <repo>
cd CrosstonesV4

rem Configure (downloads raylib via vcpkg)
cmake --preset debug

rem Build
cmake --build build --config Debug
```

## Run

```cmd
build\Debug\crosstones_executable.exe
```

The launcher window lets you pick a game mode, configure players (type + depth/time), and select rulesets.

### CLI mode

```cmd
crosstones_executable game --p1 ManualPlayer 0 --p2 Deepchad 4 --mode 010
crosstones_executable explore
crosstones_executable viewer
crosstones_executable test
```

Persistent settings are saved to `crosstones_config.txt`.

## Modes

| Mode | Description |
|---|---|
| **Game** | Play matches (human vs human, human vs bot, or bot vs bot) |
| **Explore** | Browse individual moves from preset positions with auto-advance |
| **Viewer** | Step through recorded games move by move |
| **Test** | Runs perft bulk tests |

## Project Structure

```
src/
  app/               — Raylib UI (launcher, board drawing, modes)
  game-suite/        — Game logic (Board, GameMaster, Move generation)
  globals/           — Shared types (Player, Piece, etc.)
  felix-bots/        — Felix bot implementations
  trix-bots/         — Trix bot implementations
  test-bots/         — Test bot implementations
  test-suite/        — Standalone test runner
```
