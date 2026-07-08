#pragma once
#include <string>
#include <vector>

class Player;
class ManualPlayer;
struct GamePosition;


namespace services {

struct PlayerTypeInfo {
    std::string id;
    std::string displayName;
};

const std::vector<PlayerTypeInfo>& availablePlayerTypes();

// outManual is set if the created player is a ManualPlayer. manualBoardTarget 
// is only read when typeId == "ManualPlayer".
Player* createPlayer(const std::string& typeId, int param,
                      GamePosition* manualBoardTarget, ManualPlayer** outManual);

}
