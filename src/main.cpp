
#include <sc2api/sc2_coordinator.h>
#include <sc2api/sc2_gametypes.h>
#include <sc2utils/sc2_arg_parser.h>
#include <sc2utils/sc2_manage_process.h>
#include <iostream>
#include "Bot.h"

// ladder code is from suvorov-bot
using namespace sc2;
#ifdef DEBUG
int main(int argc, char* argv[]) {
	Coordinator coordinator;
	coordinator.LoadSettings(argc, argv);

	Bot bot;
	coordinator.SetParticipants({
		CreateParticipant(Race::Terran, &bot, "Monte"),
		CreateComputer(Race::Protoss)
	});

    coordinator.SetRealtime(true);
    
	coordinator.LaunchStarcraft();

    coordinator.StartGame(argv[1]);

	while (coordinator.Update()) {
    }

    return 0;
}

#else
namespace {

struct Options {
    Options(): GamePort(0), StartPort(0), ComputerOpponent(false) {
    }

    int32_t GamePort;
    int32_t StartPort;
    std::string ServerAddress;
    std::string OpponentId;
    bool ComputerOpponent;
    sc2::Difficulty ComputerDifficulty;
    sc2::Race ComputerRace;
};

void ParseArguments(int argc, char* argv[], Options* options_) {
    sc2::ArgParser arg_parser(argv[0]);
    arg_parser.AddOptions({
            {"-g", "--GamePort", "Port of client to connect to", false},
            {"-o", "--StartPort", "Starting server port", false},
            {"-l", "--LadderServer", "Ladder server address", false},
            {"-x", "--OpponentId", "PlayerId of opponent", false},
            {"-c", "--ComputerOpponent", "If we set up a computer opponent", false},
            {"-a", "--ComputerRace", "Race of computer oppent", false},
            {"-d", "--ComputerDifficulty", "Difficulty of computer opponent", false}
        });

    arg_parser.Parse(argc, argv);

    std::string GamePortStr;
    if (arg_parser.Get("GamePort", GamePortStr))
        options_->GamePort = atoi(GamePortStr.c_str());

    std::string StartPortStr;
    if (arg_parser.Get("StartPort", StartPortStr))
        options_->StartPort = atoi(StartPortStr.c_str());

    std::string OpponentId;
    if (arg_parser.Get("OpponentId", OpponentId))
        options_->OpponentId = OpponentId;

    arg_parser.Get("LadderServer", options_->ServerAddress);

    std::string CompOpp;
    if (arg_parser.Get("ComputerOpponent", CompOpp)) {
        options_->ComputerOpponent = true;
        std::string CompRace;
    }
}

}  // namespace

int main(int argc, char* argv[]) {
    Options options;
    ParseArguments(argc, argv, &options);

    sc2::Coordinator coordinator;
    Bot bot;

    size_t num_agents;
    if (options.ComputerOpponent) {
        num_agents = 1;
        coordinator.SetParticipants({
            CreateParticipant(sc2::Race::Terran, &bot, "Monte"),
            CreateComputer(Race::Protoss)
            });
    } else {
        num_agents = 2;
        coordinator.SetParticipants({
            CreateParticipant(sc2::Race::Terran, &bot, "Monte")
            });
    }

    std::cout << "Connecting to port " << options.GamePort << std::endl;
    coordinator.Connect(options.GamePort);
    coordinator.SetupPorts(num_agents, options.StartPort, false);
    coordinator.SetRawAffectsSelection(true);
    coordinator.JoinGame();
    coordinator.SetTimeoutMS(10000);
    std::cout << "Successfully joined game" << std::endl;

    while (coordinator.Update()) {
    }

    return 0;
}
#endif
