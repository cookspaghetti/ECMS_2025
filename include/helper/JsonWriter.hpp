#ifndef JSONWRITER_HPP
#define JSONWRITER_HPP

#include "dto/Player.hpp"
#include "dto/Match.hpp"
#include "dto/Performance.hpp"
#include "dto/Result.hpp"
#include "dto/Spectator.hpp"
#include "dto/Tournament.hpp"
#include "structures/DoublyLinkedList.hpp"
#include "structures/PriorityQueue.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

class JsonWriter {
public:
    // Player operations
    static bool appendPlayer(const Player& player, const std::string& filename = "data/players.json");
    static bool writeAllCheckedInPlayer(PriorityQueue<Player>& checkInQueue, const std::string& filename = "data/check_in.json");
    static bool writeAllPlayers(const DoublyLinkedList<Player>& players, const std::string& filename = "data/players.json");
    
    // Match operations
    static bool appendMatch(const Match& match, const std::string& filename = "data/matches.json");
    static bool writeAllMatches(const DoublyLinkedList<Match>& matches, const std::string& filename = "data/matches.json");
    
    // Performance operations
    static bool appendPerformance(const Performance& performance, const std::string& filename = "data/performances.json");
    static bool writeAllPerformances(const DoublyLinkedList<Performance>& performances, const std::string& filename = "data/performances.json");
    
    // Result operations
    static bool appendResult(const Result& result, const std::string& filename = "data/results.json");
    static bool writeAllResults(const DoublyLinkedList<Result>& results, const std::string& filename = "data/results.json");
    
    // Spectator operations
    static bool appendSpectator(const Spectator& spectator, const std::string& filename = "data/spectators.json");
    static bool writeAllSpectators(const DoublyLinkedList<Spectator>& spectators, const std::string& filename = "data/spectators.json");
    
    // Tournament operations
    static bool appendTournament(const Tournament& tournament, const std::string& filename = "data/tournaments.json");
    static bool writeAllTournaments(const DoublyLinkedList<Tournament>& tournaments, const std::string& filename = "data/tournaments.json");
    static bool writeAllTournaments(const std::vector<Tournament>& tournaments, const std::string& filename = "data/tournaments.json");

    // Champion conversion helper
    static std::string championToString(Champion champion);
    static std::string genderToString(Gender gender);
    static std::string spectatorTypeToString(SpectatorType type);
    static std::string matchTypeToString(MatchType type);
    static std::string tournamentStageToString(TournamentStage stage);
    static std::string tournamentCategoryToString(TournamentCategory category);
private:
    // Helper functions
    static bool createFileIfNotExists(const std::string& filename);
    static bool createDirectoryIfNotExists(const std::string& path);
    
    // Conversion functions
    static nlohmann::json playerToJson(const Player& player);
    static nlohmann::json matchToJson(const Match& match);
    static nlohmann::json performanceToJson(const Performance& performance);
    static nlohmann::json resultToJson(const Result& result);
    static nlohmann::json spectatorToJson(const Spectator& spectator);
    static nlohmann::json tournamentToJson(const Tournament& tournament);
    
};

#endif
