#include "LauncherMenu.h"
#include "raylib.h"
#include "FontManager.h"
#include <string>
#include <algorithm>

static const Color BG           = {25, 25, 40, 255};
static const Color BTN_NORMAL   = {50, 50, 75, 255};
static const Color BTN_HOVER    = {70, 70, 100, 255};
static const Color BTN_BORDER   = {100, 100, 130, 255};
static const Color INPUT_BG     = {35, 35, 55, 255};
static const Color INPUT_BORDER = {120, 120, 160, 255};
static const Color DROPDOWN_BG  = {45, 45, 70, 255};
static const Color DROPDOWN_HVR = {70, 70, 100, 255};

LauncherMenu::Button LauncherMenu::makeButton(float x, float y, float w, float h,
                                                const std::string& label) {
    return Button{{x, y, w, h}, label};
}

bool LauncherMenu::buttonClicked(const Button& btn, Vector2 mouse, bool clicked) {
    return clicked && CheckCollisionPointRec(mouse, btn.rect);
}

void LauncherMenu::drawButton(const Button& btn) {
    Color c = btn.hovered ? BTN_HOVER : BTN_NORMAL;
    DrawRectangleRec(btn.rect, c);
    DrawRectangleLinesEx(btn.rect, 1, BTN_BORDER);
    DrawAppText(btn.label.c_str(),
                btn.rect.x + btn.rect.width / 2 - MeasureAppText(btn.label.c_str(), 16) / 2,
                btn.rect.y + btn.rect.height / 2 - 8, 16, WHITE);
}

void LauncherMenu::drawArrowButton(const Button& btn) {
    Color c = btn.hovered ? BTN_HOVER : BTN_NORMAL;
    DrawRectangleRec(btn.rect, c);
    DrawRectangleLinesEx(btn.rect, 1, BTN_BORDER);
    DrawAppText(btn.label.c_str(),
                btn.rect.x + btn.rect.width / 2 - MeasureAppText(btn.label.c_str(), 16) / 2,
                btn.rect.y + btn.rect.height / 2 - 8, 16, WHITE);
}

void LauncherMenu::onStart() {
    playerTypes = {"Deepchad", "AlphaCruncher", "Hydra", "TheFirst", "ManualPlayer"};

    auto findType = [&](const std::string& name) -> int {
        for (size_t i = 0; i < playerTypes.size(); i++)
            if (playerTypes[i] == name) return (int)i;
        return -1;
    };

    p1TypeIdx = findType(config->game.player1.type);
    if (p1TypeIdx < 0) p1TypeIdx = 0;
    p2TypeIdx = findType(config->game.player2.type);
    if (p2TypeIdx < 0) p2TypeIdx = 1;
    p1Param = config->game.player1.param;
    p2Param = config->game.player2.param;
    gameModeIdx = config->game.gameModeBits;

    p1InputText = std::to_string(p1Param);
    p2InputText = std::to_string(p2Param);
    activeField = 0;
    cursorBlink = 0;
    gmDropdownOpen = false;

    resultConfig.mode = AppMode::None;
}

void LauncherMenu::onFrame() {
    Vector2 mouse = GetMousePosition();
    bool click = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    cursorBlink++;

    const char* gameModeNames[] = {
        "Phoenix", "Monkey", "Lotus", "Kylin",
        "Tortoise", "Orchid", "Crane", "Dragon"
    };
    const int numGameModes = 8;

    // Build mode buttons dynamically from registry, wrapping into rows
    const auto& apps = AppRegistry::entries();
    const float btnStartX = 50;
    const float btnStartY = 70;
    const float btnW = 160;
    const float btnH = 40;
    const float btnGap = 20;
    const int cols = (int)((windowWidth - btnStartX - 30) / (btnW + btnGap));
    const int btnRows = ((int)apps.size() + cols - 1) / cols;
    const float modeYOffset = (btnRows > 0 ? btnRows - 1 : 0) * (btnH + btnGap);
    std::vector<Button> modeBtns;
    for (size_t i = 0; i < apps.size(); i++) {
        int row = (int)(i / cols);
        int col = (int)(i % cols);
        modeBtns.push_back(makeButton(
            btnStartX + col * (btnW + btnGap),
            btnStartY + row * (btnH + btnGap),
            btnW, btnH, apps[i].name.data()));
    }

    const Rectangle p1InputRect = {90, 180 + modeYOffset, 100, 30};
    const Rectangle p2InputRect = {470, 180 + modeYOffset, 100, 30};
    const Rectangle gmDropdownRect = { 250, 240 + modeYOffset, 300, 30 };

    // Player type arrow buttons
    Button p1Prev = makeButton(50, 145 + modeYOffset, 30, 30, "<");
    Button p1Next = makeButton(310, 145 + modeYOffset, 30, 30, ">");
    Button p2Prev = makeButton(430, 145 + modeYOffset, 30, 30, "<");
    Button p2Next = makeButton(690, 145 + modeYOffset, 30, 30, ">");

    Button runUtilFunction = makeButton(10, 300, 100, 30, "Run");

    if (!gmDropdownOpen && activeField == 0) {
        for (auto& btn : modeBtns)
            btn.hovered = CheckCollisionPointRec(mouse, btn.rect);
        p1Prev.hovered = CheckCollisionPointRec(mouse, p1Prev.rect);
        p1Next.hovered = CheckCollisionPointRec(mouse, p1Next.rect);
        p2Prev.hovered = CheckCollisionPointRec(mouse, p2Prev.rect);
        p2Next.hovered = CheckCollisionPointRec(mouse, p2Next.rect);
        runUtilFunction.hovered = CheckCollisionPointRec(mouse, runUtilFunction.rect);
    }

    // --- Handle clicks ---
    auto selectMode = [&](AppMode mode) {
        selectedMode = mode;
        resultConfig = *config;
        resultConfig.mode = mode;
        resultConfig.game.player1.type = playerTypes[p1TypeIdx];
        resultConfig.game.player1.param = p1Param;
        resultConfig.game.player2.type = playerTypes[p2TypeIdx];
        resultConfig.game.player2.param = p2Param;
        resultConfig.game.gameModeBits = gameModeIdx;
        stop();
    };

    if (activeField == 0 && !gmDropdownOpen) {
        for (size_t i = 0; i < modeBtns.size(); i++)
            if (buttonClicked(modeBtns[i], mouse, click))
                selectMode(apps[i].mode);
        if (buttonClicked(p1Prev, mouse, click)) p1TypeIdx = (p1TypeIdx - 1 + (int)playerTypes.size()) % playerTypes.size();
        if (buttonClicked(p1Next, mouse, click)) p1TypeIdx = (p1TypeIdx + 1) % playerTypes.size();
        if (buttonClicked(p2Prev, mouse, click)) p2TypeIdx = (p2TypeIdx - 1 + (int)playerTypes.size()) % playerTypes.size();
        if (buttonClicked(p2Next, mouse, click)) p2TypeIdx = (p2TypeIdx + 1) % playerTypes.size();
        if (buttonClicked(runUtilFunction, mouse, click)) Utils::runCode();
    }

    // --- Text input field interaction ---
    bool clickedInput1 = click && CheckCollisionPointRec(mouse, p1InputRect);
    bool clickedInput2 = click && CheckCollisionPointRec(mouse, p2InputRect);

    if (click) {
        if (clickedInput1) {
            activeField = 1;
        } else if (clickedInput2) {
            activeField = 2;
        } else if (activeField != 0 && !CheckCollisionPointRec(mouse, p1InputRect) && !CheckCollisionPointRec(mouse, p2InputRect)) {
            if (activeField == 1) {
                if (!p1InputText.empty()) p1Param = std::clamp(std::stoi(p1InputText), 0, 999);
                else p1Param = 0;
                p1InputText = std::to_string(p1Param);
            } else if (activeField == 2) {
                if (!p2InputText.empty()) p2Param = std::clamp(std::stoi(p2InputText), 0, 999);
                else p2Param = 0;
                p2InputText = std::to_string(p2Param);
            }
            activeField = 0;
        }
    }

    // --- Text input keyboard handling ---
    auto handleTextField = [&](std::string& text, int& param) {
        int c = GetCharPressed();
        while (c > 0) {
            if (c >= '0' && c <= '9' && text.length() < 3) {
                if (text == "0") text.clear();
                text += (char)c;
            }
            c = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && !text.empty()) {
            text.pop_back();
        }
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_TAB)) {
            if (!text.empty()) param = std::clamp(std::stoi(text), 0, 999);
            else param = 0;
            text = std::to_string(param);
            if (IsKeyPressed(KEY_TAB)) activeField = (activeField == 1) ? 2 : 2;
            else activeField = 0;
        }
        if (IsKeyPressed(KEY_ESCAPE)) {
            text = std::to_string(param);
            activeField = 0;
        }
    };

    if (activeField == 1) {
        handleTextField(p1InputText, p1Param);
    } else if (activeField == 2) {
        handleTextField(p2InputText, p2Param);
    }

    // --- Game mode dropdown interaction ---
    hoveredGmIdx = -1;
    if (gmDropdownOpen) {
        for (int p = 0; p < numGameModes; p++) {
            Rectangle optRect = { gmDropdownRect.x, gmDropdownRect.y + gmDropdownRect.height + p * gmDropdownRect.height, gmDropdownRect.width, gmDropdownRect.height };
            if (CheckCollisionPointRec(mouse, optRect)) {
                hoveredGmIdx = p;
                if (click) {
                    gameModeIdx = p;
                    gmDropdownOpen = false;
                }
                break;
            }
        }
        if (click && hoveredGmIdx == -1 && !CheckCollisionPointRec(mouse, gmDropdownRect)) {
            gmDropdownOpen = false;
        }
    } else {
        if (click && CheckCollisionPointRec(mouse, gmDropdownRect)) {
            gmDropdownOpen = true;
        }
    }

    if (activeField == 0 && IsKeyPressed(KEY_TAB) && !gmDropdownOpen) {
        gameModeIdx = (gameModeIdx + 1) % numGameModes;
    }

    // --- Draw ---
    BeginDrawing();
    ClearBackground(BG);

    drawText("Crosstones V4", 50, 20, 24, WHITE);
    drawText("Select a mode:", 50, 50, 16, LIGHTGRAY);
    for (auto& btn : modeBtns)
        drawButton(btn);

    // Player 1
    drawText("Player 1 (White):", 50, 120 + modeYOffset, 16, WHITE);
    drawArrowButton(p1Prev);
    std::string p1Label = playerTypes[p1TypeIdx];
    drawText(p1Label.c_str(), 90, 152 + modeYOffset, 16, WHITE);
    drawArrowButton(p1Next);

    DrawRectangleRec(p1InputRect, activeField == 1 ? DROPDOWN_BG : INPUT_BG);
    DrawRectangleLinesEx(p1InputRect, 1, activeField == 1 ? INPUT_BORDER : BTN_BORDER);
    std::string p1Display = p1InputText.empty() ? "0" : p1InputText;
    if (activeField == 1 && (cursorBlink / 15) % 2 == 0) p1Display += "|";
    drawText(p1Display.c_str(), (int)p1InputRect.x + 8, (int)p1InputRect.y + 7, 16, WHITE);
    drawText("depth/time", 200, 188 + modeYOffset, 14, GRAY);

    // Player 2
    drawText("Player 2 (Black):", 430, 120 + modeYOffset, 16, WHITE);
    drawArrowButton(p2Prev);
    std::string p2Label = playerTypes[p2TypeIdx];
    drawText(p2Label.c_str(), 470, 152 + modeYOffset, 16, WHITE);
    drawArrowButton(p2Next);

    DrawRectangleRec(p2InputRect, activeField == 2 ? DROPDOWN_BG : INPUT_BG);
    DrawRectangleLinesEx(p2InputRect, 1, activeField == 2 ? INPUT_BORDER : BTN_BORDER);
    std::string p2Display = p2InputText.empty() ? "0" : p2InputText;
    if (activeField == 2 && (cursorBlink / 15) % 2 == 0) p2Display += "|";
    drawText(p2Display.c_str(), (int)p2InputRect.x + 8, (int)p2InputRect.y + 7, 16, WHITE);
    drawText("depth/time", 580, 188 + modeYOffset, 14, GRAY);

    // Game Rules (dropdown)
    drawText("Game Rules:", 50, 245 + modeYOffset, 16, WHITE);
    DrawRectangleRec(gmDropdownRect, gmDropdownOpen ? DROPDOWN_BG : DROPDOWN_BG);
    DrawRectangleLinesEx(gmDropdownRect, 1, BTN_BORDER);
    std::string gmName = gameModeNames[gameModeIdx];
    DrawAppText(gmName.c_str(), gmDropdownRect.x + 10, gmDropdownRect.y + 7, 16, WHITE);
    DrawAppText(gmDropdownOpen ? "^" : "v", gmDropdownRect.x + gmDropdownRect.width - 20, gmDropdownRect.y + 7, 16, WHITE);

    if (gmDropdownOpen) {
        for (int p = 0; p < numGameModes; p++) {
            Rectangle optRect = { gmDropdownRect.x, gmDropdownRect.y + gmDropdownRect.height + p * gmDropdownRect.height, gmDropdownRect.width, gmDropdownRect.height };
            DrawRectangleRec(optRect, hoveredGmIdx == p ? DROPDOWN_HVR : DROPDOWN_BG);
            DrawRectangleLinesEx(optRect, 1, BTN_BORDER);
            DrawAppText(gameModeNames[p], optRect.x + 10, optRect.y + 7, 14, hoveredGmIdx == p ? WHITE : LIGHTGRAY);
        }
    }

    drawText("ESC: exit", 50, 500 + modeYOffset, 14, GRAY);

    EndDrawing();
}

void LauncherMenu::onStop() {
    // Save settings for next launch
    AppConfig saveCfg = resultConfig;
    saveCfg.mode = AppMode::None; // always show launcher next time
    saveCfg.toFile("crosstones_config.txt");
}
