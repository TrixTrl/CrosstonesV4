#pragma once
#include <string>
#include <vector>

class Player;
class ManualPlayer;
struct GamePosition;

// Seam between the UI (GameApp) and the bot implementations (felix-bots/
// trix-bots). GameApp used to #include and `new` every bot class directly;
// this is the only file that does so now.
namespace services {

struct PlayerTypeInfo {
    std::string id;
    std::string displayName;
};

const std::vector<PlayerTypeInfo>& availablePlayerTypes();

// outManual is set iff the created player is a ManualPlayer (the caller
// needs this to wire board input to it). manualBoardTarget is only read
// when typeId == "ManualPlayer".
Player* createPlayer(const std::string& typeId, int param,
                      GamePosition* manualBoardTarget, ManualPlayer** outManual);

}
