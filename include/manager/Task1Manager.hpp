#ifndef TASK1_MANAGER_HPP
#define TASK1_MANAGER_HPP

#include <string>
#include <random>
#include "functions/MatchScheduler.hpp"
#include "functions/TournamentManager.hpp"
#include "structures/PriorityQueue.hpp"
#include "structures/DynamicArray.hpp"
#include "dto/Player.hpp"
#include "dto/Match.hpp"
#include "dto/Result.hpp"
#include "dto/Tournament.hpp"
#include "general/Enum.hpp"

class Task1Manager {
private:
    TournamentManager& tournamentManager;
    MatchScheduler matchScheduler;
    DynamicArray<Match> allMatches;
    DynamicArray<Player> advancedPlayers;
    std::string currentTournamentId;

public:
    // Constructor
    Task1Manager(TournamentManager& tm);
    
    // Core scheduling functions
    void startMatchScheduling();
    DynamicArray<Match> scheduleQualifiers(const DynamicArray<Player>& players);
    DynamicArray<Match> scheduleNextStage(TournamentStage stage, const DynamicArray<Player>& winners);
    
    // Player management
    DynamicArray<Player> getWildcardPlayers(const DynamicArray<Player>& players);
    DynamicArray<Player> simulateMatchResults(const DynamicArray<Match>& matches, const DynamicArray<Player>& players);
    
    // Tournament progression
    void advanceToNextStage(const DynamicArray<Match>& currentMatches);
    bool canAdvanceToNextStage(TournamentStage currentStage, int playerCount);
    TournamentStage getNextStage(TournamentStage currentStage) const;
    int getRequiredPlayersForStage(TournamentStage stage);
    
    // Visualization and reporting
    void displayCurrentBracket() const;
    void displayStageResults(TournamentStage stage) const;
    void displayPlayerProgression() const;
    void displayTournamentSummary() const;
    
    // File operations
    void saveMatchesToFile(const DynamicArray<Match>& matches, const std::string& filename = "data/matches.json");
    DynamicArray<Match> loadMatchesFromFile(const std::string& filename = "data/matches.json");
    void saveResultToFile(const Result& result, const std::string& filename = "data/results.json");
    
    // Menu and interaction
    void displayMenu() const;
    int handleMenuChoice();
    void runMatchSchedulingSystem();
    
    // Utility functions
    void clearAllMatches();
    void clearMemoryOnly();
    DynamicArray<Match> getMatchesByStage(TournamentStage stage) const;
    int getTotalMatchesCount() const;
    bool hasScheduledMatches() const;
    Champion getRandomChampion(std::mt19937& gen) const;
    std::string generateUniqueResultId() const;
    DynamicArray<Player> simulateGroupStageAndGetWinners(const DynamicArray<Match>& matches, const DynamicArray<Player>& players);
    
    // Automatic stage advancement
    TournamentStage determineCurrentStage() const;
    bool automaticAdvanceToNextStage(TournamentStage currentStage, TournamentStage nextStage);
    void updateTournamentStage(TournamentStage newStage);
    bool isStageCompleted(TournamentStage stage) const;
    
    // Getters
    const DynamicArray<Match>& getAllMatches() const { return allMatches; }
    const DynamicArray<Player>& getAdvancedPlayers() const { return advancedPlayers; }
    const std::string& getCurrentTournamentId() const { return currentTournamentId; }
};

#endif
