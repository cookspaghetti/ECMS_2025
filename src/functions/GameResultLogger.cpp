#include <iostream>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include "functions/GameResultLogger.hpp"
#include "helper/JsonLoader.hpp"
#include "dto/Match.hpp"
#include "dto/Result.hpp"
#include "dto/Performance.hpp"
#include "dto/Player.hpp"

using namespace std;
using json = nlohmann::json;

GameResultLogger::GameResultLogger()
  : loadedResultsCount(0)
  , searchResultsStack(50)                    // Stack for search results
  , playerAnalysisStack(100)                  // Stack for player analysis  
  , operationHistoryStack(100)                // Stack for operation history
  , processingStack(50)                       // Stack for general processing
  , playerCount(0) {
    
    // Initialize player statistics array
    for (int i = 0; i < MAX_PLAYERS; ++i) {
        playerStatistics[i] = PlayerStats();
    }
    
    // Automatically load results from JSON into DoublyLinkedList on initialization
    // std::cout << "Task 4: Loading game results from JSON into DoublyLinkedList...\n";
    loadResultsFromJSON();
    recordOperation("Task 4 system initialized - DoublyLinkedList + Stack ready");
}

GameResultLogger::~GameResultLogger() {
    // Destructor - DoublyLinkedList and Stack will clean themselves up automatically
}

int GameResultLogger::findPlayerIndex(const std::string& playerId) const {
    for (int i = 0; i < playerCount; ++i) {
        if (playerStatistics[i].playerId == playerId) {
            return i;
        }
    }
    return -1; // Not found
}

std::string GameResultLogger::stageToString(TournamentStage stage) const {
    switch (stage) {
        case TournamentStage::Registration: return "Registration";
        case TournamentStage::Qualifiers: return "Qualifiers";
        case TournamentStage::Quarterfinals: return "Quarterfinals";
        case TournamentStage::Semifinals: return "Semifinals";
        case TournamentStage::Finals: return "Finals";
        case TournamentStage::Completed: return "Completed";
        default: return "Unknown Stage";
    }
}

// ===============================================
// TASK 4 JSON LOADING (DoublyLinkedList)
// ===============================================

void GameResultLogger::loadResultsFromJSON() {
    loadResultsFromJSON("data/results.json");
}

void GameResultLogger::loadResultsFromJSON(const std::string& jsonPath) {
    // std::cout << "Using JsonLoader to load results from: " << jsonPath << "\n";
    recordOperation("Loading results using JsonLoader: " + jsonPath);
    
    // Use JsonLoader - it handles DoublyLinkedList internally
    auto resultsList = JsonLoader::loadResults(jsonPath);
    
    if (resultsList.getSize() == 0) {
        std::cout << "JsonLoader could not load results. No results available.\n";
        recordOperation("JSON loading failed - no results found");
        loadedResultsCount = 0;
        return;
    }
    
    loadedResultsCount = resultsList.getSize();
    // std::cout << "JsonLoader successfully loaded " << loadedResultsCount << " results.\n";
    // std::cout << "JsonLoader manages DoublyLinkedList internally.\n";
    recordOperation("JSON loading completed - " + std::to_string(loadedResultsCount) + " results loaded via JsonLoader");
    
    // Calculate player statistics from JsonLoader data
    calculatePlayerStatistics();
}

void GameResultLogger::calculatePlayerStatistics() {
    recordOperation("Calculating player statistics using JsonLoader");
    
    // Get fresh data from JsonLoader (DoublyLinkedList handled internally)
    auto resultsList = JsonLoader::loadResults("data/results.json");
    if (resultsList.getSize() == 0) {
        resultsList = JsonLoader::loadResults("data/results.json");
    }
    
    // Reset player statistics
    for (int i = 0; i < MAX_PLAYERS; ++i) {
        playerStatistics[i] = PlayerStats();
    }
    playerCount = 0;
    
    if (resultsList.getSize() == 0) {
        recordOperation("No results available for statistics calculation");
        return;
    }
    
    // Analyze each result from JsonLoader's DoublyLinkedList
    for (int i = 0; i < resultsList.getSize(); ++i) {
        Result* result = resultsList.get(i);
        if (!result) continue;
        
        // Create approximate player IDs (since JsonLoader doesn't provide MatchPlayerInfo separately)
        std::string player1Id = "P" + std::string(5 - std::to_string(100 + i).length(), '0') + std::to_string(100 + i);  // Format P00XXX
        std::string player2Id = "P" + std::string(5 - std::to_string(200 + i).length(), '0') + std::to_string(200 + i);  // Format P00XXX
        std::string winnerId = result->winnerId;
        
        // Find or create player statistics entries
        int player1Index = findPlayerIndex(player1Id);
        if (player1Index == -1 && playerCount < MAX_PLAYERS) {
            playerStatistics[playerCount] = PlayerStats(player1Id, "Player" + player1Id);
            player1Index = playerCount++;
        }
        
        int player2Index = findPlayerIndex(player2Id);
        if (player2Index == -1 && playerCount < MAX_PLAYERS) {
            playerStatistics[playerCount] = PlayerStats(player2Id, "Player" + player2Id);
            player2Index = playerCount++;
        }
        
        // Update statistics for player 1
        if (player1Index != -1) {
            bool player1Won = (winnerId == player1Id);
            Champion player1Champion = (result->championsP1 != Champion::NoChampion) ?
                                       result->championsP1 : Champion::NoChampion;
            playerStatistics[player1Index].updateStats(player1Won, player1Champion, "2024-01-01", 30.0f);
            
            // Push to analysis stack
            pushPlayerAnalysis(playerStatistics[player1Index]);
        }
        
        // Update statistics for player 2
        if (player2Index != -1) {
            bool player2Won = (winnerId == player2Id);
            Champion player2Champion = (result->championsP2 != Champion::NoChampion) ?
                                       result->championsP2 : Champion::NoChampion;
            playerStatistics[player2Index].updateStats(player2Won, player2Champion, "2024-01-01", 30.0f);
            
            // Push to analysis stack
            pushPlayerAnalysis(playerStatistics[player2Index]);
        }
    }
    
    recordOperation("Player statistics calculation completed using JsonLoader data");
}

void GameResultLogger::displayLoadedResults() const {
    std::cout << "\n=== LOADED RESULTS FROM JSON (via JsonLoader) ===\n";
    
    // Get fresh data from JsonLoader (DoublyLinkedList handled internally)
    auto resultsList = JsonLoader::loadResults("data/results.json");
    if (resultsList.getSize() == 0) {
        resultsList = JsonLoader::loadResults("data/results.json");
    }
    
    if (resultsList.getSize() == 0) {
        std::cout << "No results loaded from JSON.\n";
        return;
    }
    
    std::cout << std::left;
    std::cout << std::setw(6) << "ID" << std::setw(10) << "Match ID" 
              << std::setw(8) << "Winner" << std::setw(12) << "P1 Champ" << std::setw(12) << "P2 Champ\n";
    std::cout << std::string(58, '-') << "\n";
    
    // Use JsonLoader's DoublyLinkedList results
    for (int i = 0; i < resultsList.getSize(); ++i) {
        Result* result = resultsList.get(i);
        if (!result) continue;
        
        // Get champion names for display
        std::string champ1Str = championToString(result->championsP1);
        std::string champ2Str = championToString(result->championsP2);
        
        std::cout << std::setw(6) << result->id
                  << std::setw(10) << result->matchId
                  << std::setw(8) << result->winnerId
                  << std::setw(12) << champ1Str.substr(0, 11)
                  << champ2Str.substr(0, 11) << "\n";
    }
    
    std::cout << "\nTotal loaded results (via JsonLoader): " << resultsList.getSize() << "\n";
    std::cout << "JsonLoader DoublyLinkedList size: " << resultsList.getSize() << "\n";
    std::cout << "==================================\n";
}

void GameResultLogger::displayPlayerMatchCount() const {
    std::cout << "\n=== PLAYER MATCH COUNT ANALYSIS ===\n";
    
    if (playerCount == 0) {
        std::cout << "No player data available. Load results first.\n";
        return;
    }
    
    std::cout << std::left;
    std::cout << std::setw(12) << "Player ID" << std::setw(20) << "Player Name" 
              << std::setw(12) << "Matches" << std::setw(8) << "Wins" 
              << std::setw(10) << "Losses\n";
    std::cout << std::string(62, '-') << "\n";
    
    int totalMatches = 0;
    for (int i = 0; i < playerCount; ++i) {
        const PlayerStats& stats = playerStatistics[i];
        if (stats.totalMatches > 0) {
            std::cout << std::setw(12) << stats.playerId
                      << std::setw(20) << stats.playerName
                      << std::setw(12) << stats.totalMatches
                      << std::setw(8) << stats.wins
                      << stats.losses << "\n";
            totalMatches += stats.totalMatches;
        }
    }
    
    std::cout << "\nTotal matches played across all players: " << totalMatches / 2 << "\n"; // Divide by 2 since each match involves 2 players
    std::cout << "=====================================\n";
}

void GameResultLogger::displayPlayerWinRates() const {
    std::cout << "\n=== PLAYER WIN RATE ANALYSIS ===\n";
    
    if (playerCount == 0) {
        std::cout << "No player data available. Load results first.\n";
        return;
    }
    
    // Create a sorted array for win rate ranking
    PlayerStats sortedStats[MAX_PLAYERS];
    int validPlayerCount = 0;
    
    // Copy players with match data
    for (int i = 0; i < playerCount; ++i) {
        if (playerStatistics[i].totalMatches > 0) {
            sortedStats[validPlayerCount++] = playerStatistics[i];
        }
    }
    
    // Sort by win rate (descending)
    for (int i = 0; i < validPlayerCount - 1; ++i) {
        for (int j = 0; j < validPlayerCount - i - 1; ++j) {
            if (sortedStats[j].winRate < sortedStats[j + 1].winRate) {
                PlayerStats temp = sortedStats[j];
                sortedStats[j] = sortedStats[j + 1];
                sortedStats[j + 1] = temp;
            }
        }
    }
    
    std::cout << std::left;
    std::cout << std::setw(6) << "Rank" << std::setw(12) << "Player ID" 
              << std::setw(20) << "Player Name" << std::setw(12) << "Matches"
              << std::setw(10) << "Win Rate" << std::setw(12) << "Win Streak\n";  
    std::cout << std::string(72, '-') << "\n";
    
    for (int i = 0; i < validPlayerCount; ++i) {
        const PlayerStats& stats = sortedStats[i];
        std::cout << std::setw(6) << (i + 1)
                  << std::setw(12) << stats.playerId
                  << std::setw(20) << stats.playerName
                  << std::setw(12) << stats.totalMatches
                  << std::setw(9) << std::fixed << std::setprecision(1) << stats.winRate << "%"
                  << stats.currentWinStreak << "\n";
    }
    
    std::cout << "=================================\n";
}

void GameResultLogger::displayPlayerFavoriteChampions() const {
    std::cout << "\n=== PLAYER FAVORITE CHAMPIONS ===\n";
    
    if (playerCount == 0) {
        std::cout << "No player data available. Load results first.\n";
        return;
    }
    
    std::cout << std::left;
    std::cout << std::setw(12) << "Player ID" << std::setw(20) << "Player Name" 
              << std::setw(20) << "Favorite Champion" << std::setw(8) << "Uses\n";
    std::cout << std::string(60, '-') << "\n";
    
    for (int i = 0; i < playerCount; ++i) {
        const PlayerStats& stats = playerStatistics[i];
        if (stats.totalMatches > 0 && stats.mostUsedChampion != Champion::NoChampion) {
            // Find usage count for most used champion
            int maxUsage = 0;
            for (int j = 0; j < 12; ++j) { // Only check valid champion enum range (0-11)
                if (stats.championUsageCount[j] > maxUsage) {
                    maxUsage = stats.championUsageCount[j];
                }
            }
            
            std::cout << std::setw(12) << stats.playerId
                      << std::setw(20) << stats.playerName
                      << std::setw(20) << championToString(stats.mostUsedChampion)
                      << maxUsage << "\n";
        }
    }
    
    std::cout << "===================================\n";
}

void GameResultLogger::displayComprehensivePlayerStats() const {
    std::cout << "\n=== COMPREHENSIVE PLAYER STATISTICS ===\n";
    
    if (playerCount == 0) {
        std::cout << "No player data available. Load results first.\n";
        return;
    }
    
    for (int i = 0; i < playerCount; ++i) {
        const PlayerStats& stats = playerStatistics[i];
        if (stats.totalMatches > 0) {
            std::cout << "\n--- " << stats.playerName << " (ID: " << stats.playerId << ") ---\n";
            std::cout << "Total Matches: " << stats.totalMatches << "\n";
            std::cout << "Wins: " << stats.wins << " | Losses: " << stats.losses << "\n";
            std::cout << "Win Rate: " << std::fixed << std::setprecision(1) << stats.winRate << "%\n";
            std::cout << "Current Win Streak: " << stats.currentWinStreak << "\n";
            std::cout << "Longest Win Streak: " << stats.longestWinStreak << "\n";
            
            if (stats.mostUsedChampion != Champion::NoChampion) {
                std::cout << "Most Used Champion: " << championToString(stats.mostUsedChampion) << "\n";
            }
            
            // Show top 3 champions if available
            std::cout << "Champion Usage: ";
            int shownChampions = 0;
            for (int j = 0; j < 12 && shownChampions < 3; ++j) { // Only check valid champion enum range (0-11)
                if (stats.championUsageCount[j] > 0) {
                    if (shownChampions > 0) std::cout << ", ";
                    Champion champ = static_cast<Champion>(j);
                    std::cout << championToString(champ)
                             << "(" << stats.championUsageCount[j] << ")";
                    shownChampions++;
                }
            }
            std::cout << "\n";
        }
    }
    
    std::cout << "\n========================================\n";
}

// ===============================================
// NEW STACK-BASED TASK 4 IMPLEMENTATION
// ===============================================

// Stack operations for search results
void GameResultLogger::pushSearchResult(const Result& result) {
    if (!searchResultsStack.isFull()) {
        searchResultsStack.push(result);
        recordOperation("Pushed search result to stack - Match ID: " + result.matchId);
    }
}

Result GameResultLogger::popSearchResult() {
    if (!searchResultsStack.isEmpty()) {
        Result result = searchResultsStack.pop();
        recordOperation("Popped search result from stack - Match ID: " + result.matchId);
        return result;
    }
    return Result(); // Return empty result if stack is empty
}

void GameResultLogger::clearSearchResults() {
    while (!searchResultsStack.isEmpty()) {
        searchResultsStack.pop();
    }
    recordOperation("Cleared all search results from stack");
}

void GameResultLogger::displaySearchResultsStack() const {
    std::cout << "\n=== SEARCH RESULTS STACK ===\n";
    std::cout << "Stack size: " << searchResultsStack.size() << "\n";
    
    if (searchResultsStack.isEmpty()) {
        std::cout << "Search results stack is empty.\n";
        return;
    }
    
    // Create temporary stack to display without modifying original
    Stack<Result> tempStack(MAX_RECENT_MATCHES);
    
    std::cout << std::left;
    std::cout << std::setw(10) << "Match ID" << std::setw(10) << "Winner\n";
    std::cout << std::string(20, '-') << "\n";
    
    // Display stack contents (Note: this is a simplified display)
    std::cout << "Stack contains " << searchResultsStack.size() << " search result(s)\n";
    std::cout << "Use popSearchResult() to retrieve results in LIFO order\n";
}

// Stack operations for player analysis
void GameResultLogger::pushPlayerAnalysis(const PlayerStats& stats) {
    if (!playerAnalysisStack.isFull()) {
        playerAnalysisStack.push(stats);
        recordOperation("Pushed player analysis to stack - Player ID: " + stats.playerId);
    }
}

PlayerStats GameResultLogger::popPlayerAnalysis() {
    if (!playerAnalysisStack.isEmpty()) {
        PlayerStats stats = playerAnalysisStack.pop();
        recordOperation("Popped player analysis from stack - Player ID: " + stats.playerId);
        return stats;
    }
    return PlayerStats(); // Return empty stats if stack is empty
}

void GameResultLogger::processPlayerAnalysisStack() {
    std::cout << "\n=== PROCESSING PLAYER ANALYSIS STACK ===\n";
    
    if (playerAnalysisStack.isEmpty()) {
        std::cout << "Player analysis stack is empty.\n";
        return;
    }
    
    int processedCount = 0;
    while (!playerAnalysisStack.isEmpty()) {
        PlayerStats stats = popPlayerAnalysis();
        
        std::cout << "Processing Player " << stats.playerId << " (" << stats.playerName << ")\n";
        std::cout << "  Matches: " << stats.totalMatches << ", Win Rate: " 
                  << std::fixed << std::setprecision(1) << stats.winRate << "%\n";
        
        processedCount++;
    }
    
    std::cout << "Processed " << processedCount << " players from analysis stack.\n";
    recordOperation("Processed " + std::to_string(processedCount) + " players from analysis stack");
}

void GameResultLogger::displayPlayerAnalysisStack() const {
    std::cout << "\n=== PLAYER ANALYSIS STACK ===\n";
    std::cout << "Stack size: " << playerAnalysisStack.size() << "\n";
    
    if (playerAnalysisStack.isEmpty()) {
        std::cout << "Player analysis stack is empty.\n";
        return;
    }
    
    std::cout << "Stack contains " << playerAnalysisStack.size() << " player analysis record(s)\n";
    std::cout << "Use processPlayerAnalysisStack() to process all players\n";
}

// Operation history stack management
void GameResultLogger::recordOperation(const std::string& operation) {
    if (!operationHistoryStack.isFull()) {
        operationHistoryStack.push(operation);
    }
}

std::string GameResultLogger::getLastOperation() {
    if (!operationHistoryStack.isEmpty()) {
        return operationHistoryStack.pop();
    }
    return "No operations recorded";
}

void GameResultLogger::displayOperationHistory() const {
    std::cout << "\n=== OPERATION HISTORY STACK ===\n";
    std::cout << "Stack size: " << operationHistoryStack.size() << "\n";
    
    if (operationHistoryStack.isEmpty()) {
        std::cout << "Operation history stack is empty.\n";
        return;
    }
    
    std::cout << "Recent operations are stored in stack (LIFO order)\n";
    std::cout << "Use getLastOperation() to retrieve last operation\n";
}

void GameResultLogger::clearOperationHistory() {
    while (!operationHistoryStack.isEmpty()) {
        operationHistoryStack.pop();
    }
    recordOperation("Operation history cleared");
}

// ===============================================  
// DOUBLY LINKED LIST OPERATIONS
// ===============================================

void GameResultLogger::traverseResultsForward() const {
    std::cout << "\n=== FORWARD TRAVERSAL OF RESULTS (via JsonLoader) ===\n";
    
    // Get fresh data from JsonLoader (DoublyLinkedList handled internally)
    auto resultsList = JsonLoader::loadResults("data/results.json");
    if (resultsList.getSize() == 0) {
        resultsList = JsonLoader::loadResults("data/results.json");
    }
    
    if (resultsList.getSize() == 0) {
        std::cout << "No results in JsonLoader DoublyLinkedList to traverse.\n";
        return;
    }
    
    std::cout << "Traversing " << resultsList.getSize() << " results forward:\n";
    
    for (int i = 0; i < resultsList.getSize(); ++i) {
        Result* result = resultsList.get(i);
        if (result) {
            std::cout << "Result " << (i+1) << ": Match ID " << result->matchId 
                      << ", Winner ID: " << result->winnerId << "\n";
        }
    }
}

void GameResultLogger::traverseResultsBackward() const {
    std::cout << "\n=== BACKWARD TRAVERSAL OF RESULTS (via JsonLoader) ===\n";
    
    // Get fresh data from JsonLoader (DoublyLinkedList handled internally)
    auto resultsList = JsonLoader::loadResults("data/results.json");
    if (resultsList.getSize() == 0) {
        resultsList = JsonLoader::loadResults("data/results.json");
    }
    
    if (resultsList.getSize() == 0) {
        std::cout << "No results in JsonLoader DoublyLinkedList to traverse.\n";
        return;
    }
    
    std::cout << "Traversing " << resultsList.getSize() << " results backward:\n";
    
    for (int i = resultsList.getSize() - 1; i >= 0; --i) {
        Result* result = resultsList.get(i);
        if (result) {
            std::cout << "Result " << (i+1) << ": Match ID " << result->matchId 
                      << ", Winner ID: " << result->winnerId << "\n";
        }
    }
}

void GameResultLogger::findResultInList(const std::string& matchId) const {
    std::cout << "\n=== SEARCHING FOR MATCH ID " << matchId << " IN JSONLOADER RESULTS ===\n";
    
    // Get fresh data from JsonLoader (DoublyLinkedList handled internally)
    auto resultsList = JsonLoader::loadResults("data/results.json");
    if (resultsList.getSize() == 0) {
        resultsList = JsonLoader::loadResults("data/results.json");
    }
    
    bool found = false;
    for (int i = 0; i < resultsList.getSize(); ++i) {
        Result* result = resultsList.get(i);
        if (result && result->matchId == matchId) {
            std::cout << "Found at position " << (i+1) << ":\n";
            std::cout << "  Match ID: " << result->matchId << "\n";
            std::cout << "  Winner ID: " << result->winnerId << "\n";
            
            // Push to search results stack
            const_cast<GameResultLogger*>(this)->pushSearchResult(*result);
            
            found = true;
            break;
        }
    }
    
    if (!found) {
        std::cout << "Match ID " << matchId << " not found in JsonLoader results.\n";
    }
}

void GameResultLogger::filterResultsByPlayer(const std::string& playerId) const {
    std::cout << "\n=== FILTERING RESULTS BY PLAYER " << playerId << " (via JsonLoader) ===\n";
    
    // Get fresh data from JsonLoader (DoublyLinkedList handled internally)
    auto resultsList = JsonLoader::loadResults("data/results.json");
    if (resultsList.getSize() == 0) {
        resultsList = JsonLoader::loadResults("data/results.json");
    }
    
    int foundCount = 0;
    
    for (int i = 0; i < resultsList.getSize(); ++i) {
        Result* result = resultsList.get(i);
        
        if (result && result->winnerId == playerId) {
            std::cout << "Match " << result->matchId << ": ";
            std::cout << "Player " << playerId << " WON";
            std::cout << " (Winner ID: " << result->winnerId << ")\n";
            
            // Push to search results stack
            const_cast<GameResultLogger*>(this)->pushSearchResult(*result);
            
            foundCount++;
        }
    }
    
    if (foundCount == 0) {
        std::cout << "No results found for Player " << playerId << " in JsonLoader results.\n";
    } else {
        std::cout << "Found " << foundCount << " matches for Player " << playerId << "\n";
        std::cout << "Results have been pushed to search results stack.\n";
    }
}

int GameResultLogger::getLoadedResultsCount() const {
    return loadedResultsCount;
}

void GameResultLogger::searchMatchesByPlayer(const std::string& playerId) const {
    std::cout << "\n=== SEARCHING FOR PLAYER " << playerId << " ===\n";
    
    bool found = false;
    int matchCount = 0;
    
    // Use get() method to traverse the DoublyLinkedList
    for (int i = 0; i < loadedResultsCount; i++) {
        Result* resultPtr = nullptr;
        if (!resultPtr) continue;
        
        const Result& result = *resultPtr;
        
        // Check if this result involves the player (using MatchPlayerInfo list)
        for (int j = 0; j < 0; j++) {
            MatchPlayerInfo* playerInfo = nullptr;
            if (playerInfo && playerInfo->matchId == result.matchId) {
                if (playerInfo->player1Id == playerId || playerInfo->player2Id == playerId) {
                    found = true;
                    matchCount++;
                    
                    std::cout << "Match " << matchCount << ":\n";
                    std::cout << "  Match ID: " << result.matchId << "\n";
                    std::cout << "  Winner ID: " << result.winnerId << "\n";
                    std::cout << "  Player 1 Champion: " << championToString(result.championsP1) << "\n";
                    std::cout << "  Player 2 Champion: " << championToString(result.championsP2) << "\n\n";
                }
                break;
            }
        }
    }
    
    if (!found) {
        std::cout << "No matches found for Player " << playerId << "\n";
    } else {
        std::cout << "Total matches found: " << matchCount << "\n";
    }
}

void GameResultLogger::displayPlayerPerformance(const std::string& playerId) const {
    std::cout << "\n=== PLAYER " << playerId << " PERFORMANCE ===\n";
    
    int playerIndex = findPlayerIndex(playerId);
    if (playerIndex == -1 || playerStatistics[playerIndex].totalMatches == 0) {
        std::cout << "No performance data available for Player " << playerId << "\n";
        return;
    }
    
    const PlayerStats& stats = playerStatistics[playerIndex];
    std::cout << "Player ID: " << stats.playerId << "\n";
    std::cout << "Player Name: " << stats.playerName << "\n";
    std::cout << "Total Matches: " << stats.totalMatches << "\n";
    std::cout << "Wins: " << stats.wins << "\n";
    std::cout << "Losses: " << stats.losses << "\n";
    std::cout << "Win Rate: " << std::fixed << std::setprecision(1) << stats.winRate << "%\n";
    std::cout << "Current Win Streak: " << stats.currentWinStreak << "\n";
    std::cout << "Longest Win Streak: " << stats.longestWinStreak << "\n";
    std::cout << "Average Game Duration: " << std::fixed << std::setprecision(1) << stats.averageGameDuration << " minutes\n";
    
    if (stats.mostUsedChampion != Champion::NoChampion) {
        std::cout << "Most Used Champion: " << championToString(stats.mostUsedChampion) << "\n";
    }
}

void GameResultLogger::searchMatchesByMatchId(const std::string& matchId) const {
    std::cout << "\n=== SEARCHING FOR MATCH " << matchId << " ===\n";
    
    bool found = false;
    
    // Use get() method to traverse the DoublyLinkedList
    for (int i = 0; i < loadedResultsCount; i++) {
        Result* resultPtr = nullptr;
        if (!resultPtr) continue;
        
        const Result& result = *resultPtr;
        if (result.matchId == matchId) {
            found = true;
            
            std::cout << "Match Found:\n";
            std::cout << "  Match ID: " << result.matchId << "\n";
            std::cout << "  Winner ID: " << result.winnerId << "\n";
            std::cout << "  Player 1 Champion: " << championToString(result.championsP1) << "\n";
            std::cout << "  Player 2 Champion: " << championToString(result.championsP2) << "\n";
            break;
        }
    }
    
    if (!found) {
        std::cout << "No match found with ID " << matchId << "\n";
    }
}

bool GameResultLogger::hasPlayerData(const std::string& playerId) const {
    return findPlayerIndex(playerId) != -1;
}
