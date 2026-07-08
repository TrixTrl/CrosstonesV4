#pragma once
#include "app/App.h"
#include "app/ui/components/board.h"
#include "app/core/move_tree.h"
#include "app/ui/components/move_list.h"
#include "app/ui/components/move_history.h"
#include "app/ui/components/scroll_area.h"
#include "app/features/analysis_stub.h"
#include "game-suite/Board.h"
#include "game-suite/RichMoveGenerator.h"
#include "game-suite/KsnFormatter.h"

class OpeningExplorer : public App {
protected:
    void onStart() override;
    void onTick(float, const InputState&) override;
    void onDraw(Rectangle) override;
    void onDrawOverlay(Rectangle) override;

private:
    ui::MoveList moveList;
    ui::MoveHistory moveHistory;
    ui::ScrollArea scrollArea;
    ui::AnalysisStub analysis;
    ui::Board boardView;
    EvalProvider defaultEval;

    ::Board board;
    std::string rootPos;
    std::string gameModeName;

    MoveNode root;
    MoveNode* currentNode = nullptr;
    std::vector<KsnMove> cachedKsnMoves;

    struct NavStep {
        std::vector<::Board::xMove> move;
        MoveInfo info;
        MoveNode* node = nullptr;
        std::string ksn;
    };
    std::vector<NavStep> navPath;

    enum class ExplorerPhase { ChoosingMode, Exploring };
    ExplorerPhase phase = ExplorerPhase::ChoosingMode;
    int selectedGameMode = -1;
    int lastSelectedMode = 0;

    void initDefaultRoot();
    void loadAndBuildTree();
    void buildTreeFromOpenings(const std::vector<std::pair<std::string, std::vector<int>>>& openings);
    void initBoardAtRoot(int gameMode);
    void syncBoard();

    void buildMoveList();
    void buildMoveHistory();
    // The only place that calls buildMoveList()+buildMoveHistory() — every
    // board/nav mutation calls this once at its exit point instead of
    // copy-pasting the pair (previously duplicated at 7 call sites).
    void onBoardStateChanged();
    void playMove(int candidateIdx);
    void navigateBackTo(int historyRow);
    void undoToMoveCount(int targetMoves);

    std::string getOpeningName() const;

    // Rect math shared by onTick (hit-testing) and onDrawOverlay (drawing) —
    // computed once per frame from each call site instead of two
    // independently maintained copies of the same formulas.
    struct HitRects {
        Rectangle right;
        int histX, histY, histW, colSplit, histEnd;
        int listHeaderY, listX, listY, listW, listAreaBot;
    };
    HitRects computeHitRects();

    float navCooldown = 0;
    bool keyboardHoverActive = false;

    int historyPreviewPly = -2;
    MoveInfo historyPreviewInfo;
};
