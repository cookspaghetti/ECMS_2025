#ifndef MATCH_SCHEDULER_HPP
#define MATCH_SCHEDULER_HPP

#include <string>
#include "dto/Player.hpp"
#include "dto/Match.hpp"
#include "general/Enum.hpp"
#include "structures/PriorityQueue.hpp"
#include "structures/DynamicArray.hpp"
#include "structures/PlayerPair.hpp"
#include "helper/JsonLoader.hpp"

class MatchScheduler {
private:
    static int nextMatchIdCounter;
    const std::string filename = "data/checked_in_players.json";

public:
    // Load players from JSON file
    DynamicArray<Player> loadPlayersFromFile();

    // Core matching functions
    static DynamicArray<Match> scheduleQualifierMatches(const DynamicArray<Player>& players, const std::string& tournamentId);
    static DynamicArray<Match> scheduleStageMatches(const DynamicArray<Player>& players, TournamentStage stage, const std::string& tournamentId);
    
    // Rule-based matching logic
    static DynamicArray<PlayerPair> pairPlayersByPoints(const DynamicArray<Player>& players);
    static DynamicArray<PlayerPair> randomPairing(const DynamicArray<Player>& players);
    
    // Player filtering and utilities
    static DynamicArray<Player> filterWildcards(const DynamicArray<Player>& players);
    static DynamicArray<Player> filterNonWildcards(const DynamicArray<Player>& players);
    static DynamicArray<Player> filterEarlyBirds(const DynamicArray<Player>& players);
    
    // Group-based qualifier functions
    static DynamicArray<DynamicArray<Player>> createGroupsByPoints(const DynamicArray<Player>& players, int numGroups);
    static DynamicArray<Match> scheduleRoundRobinMatches(const DynamicArray<Player>& groupPlayers, int groupNumber, 
                                                        const std::string& tournamentId, const std::string& date);
    static DynamicArray<Player> determineGroupWinners(const DynamicArray<Player>& groupPlayers, const DynamicArray<Match>& groupMatches);
    static DynamicArray<Match> scheduleTiebreakerMatches(const DynamicArray<Player>& tiedPlayers, int groupNumber,
                                                        const std::string& tournamentId, const std::string& date);
    
    // Validation functions
    static bool canPairPlayers(const Player& p1, const Player& p2, TournamentStage stage);
    static bool hasEarlyBirdConflict(const Player& p1, const Player& p2);
    
    // Match creation utilities
    static Match createMatch(const std::string& matchId, const std::string& tournamentId, TournamentStage stage, 
                           const Player& player1, const Player& player2, 
                           const std::string& date = "", const std::string& time = "");
    
    // Bracket visualization
    static void displayMatchesByStage(const DynamicArray<Match>& matches, TournamentStage stage);
    static void displayTournamentBracket(const DynamicArray<Match>& matches);
    
    // Utility functions
    static std::string stageToString(TournamentStage stage);
    static std::string formatDateTime();
    static std::string getNextMatchId();
};

#endif
