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
    void onTick(float) override;
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

    int selectedGameMode = -1;
    int lastSelectedMode = 0;

    void initDefaultRoot();
    void loadAndBuildTree();
    void buildTreeFromOpenings(const std::vector<std::pair<std::string, std::vector<int>>>& openings);
    void initBoardAtRoot(int gameMode);
    void syncBoard();

    void buildMoveList();
    void buildMoveHistory();
    void playMove(int candidateIdx);
    void navigateBackTo(int historyRow);
    void undoToMoveCount(int targetMoves);

    std::string getOpeningName() const;

    float navCooldown = 0;
    bool keyboardHoverActive = false;

    int historyPreviewPly = -2;
    MoveInfo historyPreviewInfo;
};
