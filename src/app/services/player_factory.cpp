#include "player_factory.h"
#include "felix-bots/Deepchad.h"
#include "trix-bots/TheFirst.h"
#include "trix-bots/Hydra.h"
#include "trix-bots/AlphaCruncher.h"
#include "game-suite/ManualPlayer.h"
#include "data/GamePosition.h"

namespace services {

const std::vector<PlayerTypeInfo>& availablePlayerTypes() {
    static const std::vector<PlayerTypeInfo> types = {
        {"Deepchad", "Deepchad"},
        {"AlphaCruncher", "AlphaCruncher"},
        {"Hydra", "Hydra"},
        {"TheFirst", "TheFirst"},
        {"ManualPlayer", "ManualPlayer"},
    };
    return types;
}

Player* createPlayer(const std::string& typeId, int param,
                      GamePosition* manualBoardTarget, ManualPlayer** outManual) {
    *outManual = nullptr;
    if (typeId == "Deepchad") return new Deepchad(param);
    if (typeId == "AlphaCruncher") return new AlphaCruncher(param);
    if (typeId == "Hydra") return new Hydra(param, 14);
    if (typeId == "TheFirst") return new TheFirst(param);
    if (typeId == "ManualPlayer") {
        auto* mp = new ManualPlayer(&manualBoardTarget->pieces);
        mp->onDisplayChanged = []{};
        *outManual = mp;
        return mp;
    }
    return new Deepchad(3);
}

}
