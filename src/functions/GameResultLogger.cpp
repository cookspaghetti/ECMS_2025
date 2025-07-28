#include <iostream>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include "functions/GameResultLogger.hpp"
#include "dto/Match.hpp"
#include "dto/Result.hpp"
#include "dto/Performance.hpp"
#include "dto/Player.hpp"

using namespace std;
using json = nlohmann::json;

GameResultLogger::GameResultLogger()
  : recentMatches(MAX_RECENT_MATCHES)
  , loadedResultsCount(0)
  , playerCount(0)
  , performanceRankings(MAX_PLAYERS)
  , undoStack(MAX_RECENT_MATCHES) {
    // Initialize player statistics array
    for (int i = 0; i < MAX_PLAYERS; ++i) {
        playerStatistics[i] = PlayerStats();
    }
    
    // Initialize loaded results array
    for (int i = 0; i < MAX_RESULTS; ++i) {
        loadedResults[i] = Result();
    }
    
    // Seed random number generator for match duration simulation
    srand(static_cast<unsigned int>(time(nullptr)));
}

GameResultLogger::~GameResultLogger() {
    // Destructor - LinkedHistory will clean itself up automatically
}

int GameResultLogger::findPlayerIndex(int playerId) const {
    for (int i = 0; i < playerCount; ++i) {
        if (playerStatistics[i].playerId == playerId) {
            return i;
        }
    }
    return -1; // Not found
}

std::string GameResultLogger::stageToString(TournamentStage stage) const {
    switch (stage) {
        case TournamentStage::Qualifiers: return "Qualifiers";
        case TournamentStage::GroupStage: return "Group Stage";
        case TournamentStage::KnockoutStage: return "Knockout Stage";
        case TournamentStage::Quarterfinals: return "Quarterfinals";
        case TournamentStage::Semifinals: return "Semifinals";
        case TournamentStage::Finals: return "Finals";
        default: return "Unknown Stage";
    }
}

std::string GameResultLogger::matchTypeToString(MatchType type) const {
    switch (type) {
        case MatchType::BestOf1: return "Best of 1";
        case MatchType::BestOf3: return "Best of 3";
        case MatchType::BestOf5: return "Best of 5";
        default: return "Unknown Type";
    }
}

float GameResultLogger::generateRandomDuration(MatchType type) const {
    // Simulate realistic match durations based on match type
    switch (type) {
        case MatchType::BestOf1: return 25.0f + (rand() % 20); // 25-45 minutes
        case MatchType::BestOf3: return 45.0f + (rand() % 30); // 45-75 minutes  
        case MatchType::BestOf5: return 75.0f + (rand() % 45); // 75-120 minutes
        default: return 30.0f;
    }
}

void GameResultLogger::logMatchResult(const Match& match, const Result& result, 
                                           const std::string& player1Name, const std::string& player2Name) {
    // Generate match duration
    float duration = generateRandomDuration(match.matchType);
    
    // Create match summary
    std::string winnerName = (result.winnerId == match.player1) ? player1Name : player2Name;
    MatchSummary summary(match.id, match.date, player1Name, player2Name, 
                        result.score, winnerName, match.stage, match.matchType, duration);
    
    // Add to recent matches (circular queue)
    recentMatches.enqueue(summary);
    
    // Add to complete history (linked list)
    completeHistory.addRecord(summary);
    
    // Push to undo stack
    undoStack.push(summary);
    
    // Update player statistics
    int player1Index = findPlayerIndex(match.player1);
    int player2Index = findPlayerIndex(match.player2);
    
    // If players don't exist, create new entries
    if (player1Index == -1 && playerCount < MAX_PLAYERS) {
        playerStatistics[playerCount] = PlayerStats(match.player1, player1Name);
        player1Index = playerCount++;
    }
    if (player2Index == -1 && playerCount < MAX_PLAYERS) {
        playerStatistics[playerCount] = PlayerStats(match.player2, player2Name);
        player2Index = playerCount++;
    }
    
    // Update statistics for both players
    if (player1Index != -1) {
        bool player1Won = (result.winnerId == match.player1);
        Champion player1Champion = (result.championsP1[0] != Champion::NoChampion) ? 
                                   result.championsP1[0] : Champion::NoChampion;
        playerStatistics[player1Index].updateStats(player1Won, player1Champion, match.date, duration);
    }
    
    if (player2Index != -1) {
        bool player2Won = (result.winnerId == match.player2);
        Champion player2Champion = (result.championsP2[0] != Champion::NoChampion) ? 
                                   result.championsP2[0] : Champion::NoChampion;
        playerStatistics[player2Index].updateStats(player2Won, player2Champion, match.date, duration);
    }
    
    std::cout << "✓ Match result logged successfully!\n";
    std::cout << "  Match ID: " << match.id << " | Winner: " << winnerName << " | Duration: " 
              << std::fixed << std::setprecision(1) << duration << " minutes\n";
}

void GameResultLogger::displayRecentResults(int count) const {
    std::cout << "\n=== RECENT MATCH RESULTS ===\n";
    
    if (recentMatches.isEmpty()) {
        std::cout << "No recent matches found.\n";
        return;
    }
    
    // Create a temporary queue to iterate without modifying original
    CircularQueue<MatchSummary> temp = recentMatches;
    Queue<MatchSummary> displayQueue(count);
    
    // Extract matches to display queue
    int displayCount = 0;
    while (!temp.isEmpty() && displayCount < count) {
        displayQueue.enqueue(temp.dequeue());
        displayCount++;
    }
    
    std::cout << std::left;
    std::cout << std::setw(8) << "ID" << std::setw(12) << "Date" 
              << std::setw(15) << "Player 1" << std::setw(15) << "Player 2"
              << std::setw(10) << "Score" << std::setw(15) << "Winner"
              << std::setw(12) << "Stage" << std::setw(10) << "Duration\n";
    std::cout << std::string(97, '-') << "\n";
    
    while (!displayQueue.isEmpty()) {
        MatchSummary match = displayQueue.dequeue();
        std::cout << std::setw(8) << match.matchId 
                  << std::setw(12) << match.date
                  << std::setw(15) << match.player1Name.substr(0, 14)
                  << std::setw(15) << match.player2Name.substr(0, 14)
                  << std::setw(10) << match.score
                  << std::setw(15) << match.winnerName.substr(0, 14)
                  << std::setw(12) << stageToString(match.stage).substr(0, 11)
                  << std::fixed << std::setprecision(1) << match.duration << "m\n";
    }
    
    std::cout << "============================\n";
}

void GameResultLogger::displayPlayerPerformance(int playerId) const {
    int playerIndex = findPlayerIndex(playerId);
    if (playerIndex == -1) {
        std::cout << "Player with ID " << playerId << " not found.\n";
        return;
    }
    
    const PlayerStats& stats = playerStatistics[playerIndex];
    
    std::cout << "\n=== PLAYER PERFORMANCE ===\n";
    std::cout << "Player: " << stats.playerName << " (ID: " << stats.playerId << ")\n";
    std::cout << std::string(40, '-') << "\n";
    std::cout << "Total Matches: " << stats.totalMatches << "\n";
    std::cout << "Wins: " << stats.wins << " | Losses: " << stats.losses << "\n";
    std::cout << "Win Rate: " << std::fixed << std::setprecision(1) << stats.winRate << "%\n";
    std::cout << "Current Win Streak: " << stats.currentWinStreak << "\n";
    std::cout << "Longest Win Streak: " << stats.longestWinStreak << "\n";
    std::cout << "Most Used Champion: " << "Champion#" << static_cast<int>(stats.mostUsedChampion) << "\n";
    std::cout << "Average Game Duration: " << std::fixed << std::setprecision(1) 
              << stats.averageGameDuration << " minutes\n";
    std::cout << "Last Match: " << stats.lastMatchDate << "\n";
    std::cout << "==========================\n";
}

void GameResultLogger::displayAllPlayerRankings() const {
    if (playerCount == 0) {
        std::cout << "No player data available.\n";
        return;
    }
    
    // Create a copy of player stats for sorting
    PlayerStats sortedStats[MAX_PLAYERS];
    int actualCount = 0;
    
    // Copy players with match data
    for (int i = 0; i < playerCount; ++i) {
        if (playerStatistics[i].totalMatches > 0) {
            sortedStats[actualCount++] = playerStatistics[i];
        }
    }
    
    // Simple bubble sort by win rate (descending)
    for (int i = 0; i < actualCount - 1; ++i) {
        for (int j = 0; j < actualCount - i - 1; ++j) {
            if (sortedStats[j].winRate < sortedStats[j + 1].winRate) {
                PlayerStats temp = sortedStats[j];
                sortedStats[j] = sortedStats[j + 1];
                sortedStats[j + 1] = temp;
            }
        }
    }
    
    std::cout << "\n=== PLAYER RANKINGS ===\n";
    std::cout << std::left;
    std::cout << std::setw(5) << "Rank" << std::setw(20) << "Player Name" 
              << std::setw(10) << "Matches" << std::setw(10) << "Win Rate"
              << std::setw(8) << "Wins" << std::setw(8) << "Losses"
              << std::setw(15) << "Win Streak\n";
    std::cout << std::string(76, '-') << "\n";
    
    for (int i = 0; i < actualCount; ++i) {
        const PlayerStats& stats = sortedStats[i];
        std::cout << std::setw(5) << (i + 1)
                  << std::setw(20) << stats.playerName.substr(0, 19)
                  << std::setw(10) << stats.totalMatches
                  << std::setw(9) << std::fixed << std::setprecision(1) << stats.winRate << "%"
                  << std::setw(8) << stats.wins
                  << std::setw(8) << stats.losses
                  << stats.currentWinStreak << "\n";
    }
    
    std::cout << "=======================\n";
}

void GameResultLogger::displayMatchHistory(int count) const {
    std::cout << "\n=== MATCH HISTORY ===\n";
    
    if (completeHistory.isEmpty()) {
        std::cout << "No match history available.\n";
        return;
    }
    
    HistoryNode<MatchSummary>* current = completeHistory.getHead();
    int displayed = 0;
    
    std::cout << std::left;
    std::cout << std::setw(8) << "ID" << std::setw(12) << "Date"
              << std::setw(18) << "Match" << std::setw(10) << "Score"
              << std::setw(15) << "Winner" << std::setw(12) << "Type\n";
    std::cout << std::string(75, '-') << "\n";
    
    while (current && displayed < count) {
        const MatchSummary& match = current->data;
        std::cout << std::setw(8) << match.matchId
                  << std::setw(12) << match.date
                  << std::setw(18) << (match.player1Name.substr(0,8) + " vs " + match.player2Name.substr(0,8))
                  << std::setw(10) << match.score
                  << std::setw(15) << match.winnerName.substr(0, 14)
                  << matchTypeToString(match.matchType) << "\n";
        current = current->next;
        displayed++;
    }
    
    std::cout << "\nTotal matches in history: " << completeHistory.getSize() << "\n";
    std::cout << "=====================\n";
}

void GameResultLogger::searchMatchesByPlayer(int playerId) const {
    std::cout << "\n=== MATCHES FOR PLAYER ID " << playerId << " ===\n";

    if (loadedResultsCount == 0) {
        std::cout << "No loaded results. Please load results first.\n";
        return;
    }

    // Header
    std::cout << std::left
              << std::setw(8)  << "ResID"
              << std::setw(10) << "MatchID"
              << std::setw(10) << "Score"
              << std::setw(10) << "Result\n";
    std::cout << std::string(38, '-') << "\n";

    bool found = false;
    for (int i = 0; i < loadedResultsCount; ++i) {
        // Compare the actual numeric IDs
        if (matchPlayerInfo[i].player1Id == playerId ||
            matchPlayerInfo[i].player2Id == playerId) {
            
            const Result& res = loadedResults[i];
            bool    win = (res.winnerId == playerId);
            std::cout << std::setw(8)  << res.id
                      << std::setw(10) << res.matchId
                      << std::setw(10) << res.score
                      << std::setw(10) << (win ? "WIN" : "LOSS") << "\n";
            found = true;
        }
    }

    if (!found) {
        std::cout << "No matches found for player ID " << playerId << "\n";
    }
    std::cout << "=====================================\n";
}


void GameResultLogger::searchMatchesByDate(const string& date) const {
    std::cout << "\n=== MATCHES ON " << date << " ===\n";
    
    if (completeHistory.isEmpty()) {
        std::cout << "No match history available.\n";
        return;
    }
    
    HistoryNode<MatchSummary>* current = completeHistory.getHead();
    int matchCount = 0;
    
    std::cout << std::left;
    std::cout << std::setw(8) << "ID" << std::setw(18) << "Match" << std::setw(10) << "Score"
              << std::setw(15) << "Winner" << std::setw(12) << "Stage" << std::setw(10) << "Duration\n";
    std::cout << std::string(73, '-') << "\n";
    
    while (current) {
        const MatchSummary& match = current->data;
        
        if (match.date == date) {
            std::cout << std::setw(8) << match.matchId
                      << std::setw(18) << (match.player1Name.substr(0,8) + " vs " + match.player2Name.substr(0,8))
                      << std::setw(10) << match.score
                      << std::setw(15) << match.winnerName.substr(0, 14)
                      << std::setw(12) << stageToString(match.stage).substr(0, 11)
                      << std::fixed << std::setprecision(1) << match.duration << "m\n";
            matchCount++;
        }
        
        current = current->next;
    }
    
    if (matchCount == 0) {
        std::cout << "No matches found on " << date << "\n";
    } else {
        std::cout << "\nTotal matches found: " << matchCount << "\n";
    }
    
    std::cout << "================================\n";
}

void GameResultLogger::searchMatchesByMatchId(int matchId) const {
    std::cout << "\n=== SEARCHING FOR MATCH ID " << matchId << " ===\n";
    
    // Search in loaded results first (more detailed information)
    bool foundInLoaded = false;
    for (int i = 0; i < loadedResultsCount; i++) {
        if (loadedResults[i].matchId == matchId) {
            std::cout << "\n--- MATCH DETAILS FROM JSON DATA ---\n";
            std::cout << "Match ID: " << loadedResults[i].matchId << "\n";
            std::cout << "Result ID: " << loadedResults[i].id << "\n";
            std::cout << "Player 1 ID: " << matchPlayerInfo[i].player1Id << "\n";
            std::cout << "Player 2 ID: " << matchPlayerInfo[i].player2Id << "\n";
            std::cout << "Winner ID: " << loadedResults[i].winnerId << "\n";
            std::cout << "Score: " << loadedResults[i].score << "\n";
            std::cout << "Games Played: " << loadedResults[i].gamesPlayed << "\n";
            
            // Display champions used
            std::cout << "\n--- CHAMPIONS USED ---\n";
            std::cout << "Player " << matchPlayerInfo[i].player1Id << " Champions: ";
            for (int j = 0; j < Result::TEAM_SIZE; j++) {
                if (loadedResults[i].championsP1[j] != Champion::NoChampion) {
                    std::cout << "Champion#" << static_cast<int>(loadedResults[i].championsP1[j]);
                    if (j < Result::TEAM_SIZE - 1 && loadedResults[i].championsP1[j+1] != Champion::NoChampion) {
                        std::cout << ", ";
                    }
                }
            }
            std::cout << "\nPlayer " << matchPlayerInfo[i].player2Id << " Champions: ";
            for (int j = 0; j < Result::TEAM_SIZE; j++) {
                if (loadedResults[i].championsP2[j] != Champion::NoChampion) {
                    std::cout << "Champion#" << static_cast<int>(loadedResults[i].championsP2[j]);
                    if (j < Result::TEAM_SIZE - 1 && loadedResults[i].championsP2[j+1] != Champion::NoChampion) {
                        std::cout << ", ";
                    }
                }
            }
            std::cout << "\n";
            foundInLoaded = true;
            break;
        }
    }
    
    // Also search in match history for additional info
    HistoryNode<MatchSummary>* current = completeHistory.getHead();
    bool foundInHistory = false;
    
    while (current) {
        const MatchSummary& match = current->data;
        if (match.matchId == matchId) {
            if (!foundInLoaded) {
                std::cout << "\n--- MATCH SUMMARY FROM HISTORY ---\n";
            } else {
                std::cout << "\n--- ADDITIONAL MATCH INFO ---\n";
            }
            std::cout << "Match ID: " << match.matchId << "\n";
            std::cout << "Date: " << match.date << "\n";
            std::cout << "Players: " << match.player1Name << " vs " << match.player2Name << "\n";
            std::cout << "Score: " << match.score << "\n";
            std::cout << "Winner: " << match.winnerName << "\n";
            std::cout << "Stage: " << stageToString(match.stage) << "\n";
            std::cout << "Match Type: " << matchTypeToString(match.matchType) << "\n";
            std::cout << "Duration: " << std::fixed << std::setprecision(1) << match.duration << " minutes\n";
            foundInHistory = true;
            break;
        }
        current = current->next;
    }
    
    if (!foundInLoaded && !foundInHistory) {
        std::cout << "No match found with ID " << matchId << "\n";
        std::cout << "Available match IDs in loaded data: ";
        for (int i = 0; i < loadedResultsCount && i < 10; i++) {
            std::cout << loadedResults[i].matchId;
            if (i < loadedResultsCount - 1 && i < 9) std::cout << ", ";
        }
        if (loadedResultsCount > 10) std::cout << "...";
        std::cout << "\n";
    }
    
    std::cout << "=====================================\n";
}

void GameResultLogger::displayTournamentSummary(TournamentStage stage) const {
    std::cout << "\n=== " << stageToString(stage) << " SUMMARY ===\n";
    
    if (completeHistory.isEmpty()) {
        std::cout << "No match data available.\n";
        return;
    }
    
    HistoryNode<MatchSummary>* current = completeHistory.getHead();
    int matchCount = 0;
    float totalDuration = 0.0f;
    
    std::cout << std::left;
    std::cout << std::setw(8) << "ID" << std::setw(12) << "Date" 
              << std::setw(18) << "Match" << std::setw(10) << "Score"
              << std::setw(15) << "Winner" << std::setw(10) << "Duration\n";
    std::cout << std::string(73, '-') << "\n";
    
    while (current) {
        const MatchSummary& match = current->data;
        
        if (match.stage == stage) {
            std::cout << std::setw(8) << match.matchId
                      << std::setw(12) << match.date
                      << std::setw(18) << (match.player1Name.substr(0,8) + " vs " + match.player2Name.substr(0,8))
                      << std::setw(10) << match.score
                      << std::setw(15) << match.winnerName.substr(0, 14)
                      << std::fixed << std::setprecision(1) << match.duration << "m\n";
            
            matchCount++;
            totalDuration += match.duration;
        }
        
        current = current->next;
    }
    
    if (matchCount > 0) {
        float avgDuration = totalDuration / matchCount;
        std::cout << "\nStage Statistics:\n";
        std::cout << "  Total matches: " << matchCount << "\n";
        std::cout << "  Average duration: " << std::fixed << std::setprecision(1) << avgDuration << " minutes\n";
    } else {
        std::cout << "No matches found for " << stageToString(stage) << "\n";
    }
    
    std::cout << "==================================\n";
}

void GameResultLogger::undoLastResult() {
    if (undoStack.isEmpty()) {
        std::cout << "No matches to undo.\n";
        return;
    }
    
    MatchSummary lastMatch = undoStack.pop();
    std::cout << "⚠️  Undo functionality would remove match ID " << lastMatch.matchId 
              << " (" << lastMatch.player1Name << " vs " << lastMatch.player2Name << ")\n";
    std::cout << "Note: Full undo implementation would require reversing player statistics.\n";
}

void GameResultLogger::displayWinRateAnalysis() const {
    std::cout << "\n=== WIN RATE ANALYSIS ===\n";
    
    if (playerCount == 0) {
        std::cout << "No player data available for analysis.\n";
        return;
    }
    
    float totalWinRate = 0.0f;
    int playersWithMatches = 0;
    float highestWinRate = 0.0f;
    float lowestWinRate = 100.0f;
    std::string bestPlayer = "";
    std::string worstPlayer = "";
    
    for (int i = 0; i < playerCount; ++i) {
        const PlayerStats& stats = playerStatistics[i];
        if (stats.totalMatches > 0) {
            totalWinRate += stats.winRate;
            playersWithMatches++;
            
            if (stats.winRate > highestWinRate) {
                highestWinRate = stats.winRate;
                bestPlayer = stats.playerName;
            }
            
            if (stats.winRate < lowestWinRate) {
                lowestWinRate = stats.winRate;
                worstPlayer = stats.playerName;
            }
        }
    }
    
    if (playersWithMatches > 0) {
        float averageWinRate = totalWinRate / playersWithMatches;
        
        std::cout << "Players analyzed: " << playersWithMatches << "\n";
        std::cout << "Average win rate: " << std::fixed << std::setprecision(1) << averageWinRate << "%\n";
        std::cout << "Highest win rate: " << highestWinRate << "% (" << bestPlayer << ")\n";
        std::cout << "Lowest win rate: " << lowestWinRate << "% (" << worstPlayer << ")\n";
        
        // Win rate distribution
        int excellent = 0, good = 0, average = 0, poor = 0;
        for (int i = 0; i < playerCount; ++i) {
            const PlayerStats& stats = playerStatistics[i];
            if (stats.totalMatches > 0) {
                if (stats.winRate >= 80.0f) excellent++;
                else if (stats.winRate >= 60.0f) good++;
                else if (stats.winRate >= 40.0f) average++;
                else poor++;
            }
        }
        
        std::cout << "\nWin Rate Distribution:\n";
        std::cout << "  Excellent (80%+): " << excellent << " players\n";
        std::cout << "  Good (60-79%): " << good << " players\n";
        std::cout << "  Average (40-59%): " << average << " players\n";
        std::cout << "  Poor (<40%): " << poor << " players\n";
    }
    
    std::cout << "=========================\n";
}

void GameResultLogger::displayChampionUsageStats() const {
    cout << "\n=== CHAMPION USAGE STATISTICS ===\n";
    
    if (playerCount == 0) {
        cout << "No player data available for champion analysis.\n";
        return;
    }
    
    // Aggregate champion usage across all players
    int totalChampionUsage[52] = {0}; // Initialize all to 0
    
    for (int i = 0; i < playerCount; ++i) {
        const PlayerStats& stats = playerStatistics[i];
        if (stats.totalMatches > 0) {
            for (int j = 0; j < 52; ++j) {
                totalChampionUsage[j] += stats.championUsageCount[j];
            }
        }
    }
    
    // Find top 10 most used champions
    struct ChampionUsage {
        Champion champion;
        int usage;
        string name;
    };
    
    ChampionUsage topChampions[10];
    
    // Initialize top champions array
    for (int i = 0; i < 10; ++i) {
        topChampions[i] = {Champion::NoChampion, 0, "None"};
    }
    
    // Find top champions
    for (int i = 0; i < 52; ++i) {
        if (totalChampionUsage[i] > 0) {
            Champion currentChampion = static_cast<Champion>(i);
            int currentUsage = totalChampionUsage[i];
            string currentName = "Champion#" + std::to_string(i);
            
            // Insert into top 10 if usage is high enough
            for (int j = 0; j < 10; ++j) {
                if (currentUsage > topChampions[j].usage) {
                    // Shift lower champions down
                    for (int k = 9; k > j; --k) {
                        topChampions[k] = topChampions[k - 1];
                    }
                    // Insert current champion
                    topChampions[j] = {currentChampion, currentUsage, currentName};
                    break;
                }
            }
        }
    }
    
    cout << left;
    cout << setw(5) << "Rank" << setw(18) << "Champion" << setw(8) << "Usage" << "Percentage\n";
    cout << string(45, '-') << "\n";
    
    int totalUsage = 0;
    for (int i = 0; i < 52; ++i) {
        totalUsage += totalChampionUsage[i];
    }
    
    for (int i = 0; i < 10; ++i) {
        if (topChampions[i].usage > 0) {
            float percentage = (totalUsage > 0) ? 
                              (static_cast<float>(topChampions[i].usage) / totalUsage) * 100.0f : 0.0f;
            cout << setw(5) << (i + 1)
                 << setw(18) << topChampions[i].name
                 << setw(8) << topChampions[i].usage
                 << fixed << setprecision(1) << percentage << "%\n";
        }
    }
    
    cout << "\nTotal champion picks analyzed: " << totalUsage << "\n";
    cout << "===================================\n";
}

void GameResultLogger::displayMatchTypeAnalysis() const {
    cout << "\n=== MATCH TYPE ANALYSIS ===\n";
    
    if (completeHistory.isEmpty()) {
        cout << "No match data available for analysis.\n";
        return;
    }
    
    int bestOf1Count = 0, bestOf3Count = 0, bestOf5Count = 0;
    float bestOf1Duration = 0.0f, bestOf3Duration = 0.0f, bestOf5Duration = 0.0f;
    
    HistoryNode<MatchSummary>* current = completeHistory.getHead();
    
    while (current) {
        const MatchSummary& match = current->data;
        
        switch (match.matchType) {
            case MatchType::BestOf1:
                bestOf1Count++;
                bestOf1Duration += match.duration;
                break;
            case MatchType::BestOf3:
                bestOf3Count++;
                bestOf3Duration += match.duration;
                break;
            case MatchType::BestOf5:
                bestOf5Count++;
                bestOf5Duration += match.duration;
                break;
        }
        
        current = current->next;
    }
    
    int totalMatches = bestOf1Count + bestOf3Count + bestOf5Count;
    
    cout << left;
    cout << setw(15) << "Match Type" << setw(8) << "Count" << setw(12) << "Percentage" 
         << setw(15) << "Avg Duration\n";
    cout << string(50, '-') << "\n";
    
    if (bestOf1Count > 0) {
        float percentage = (static_cast<float>(bestOf1Count) / totalMatches) * 100.0f;
        float avgDuration = bestOf1Duration / bestOf1Count;
        cout << setw(15) << "Best of 1" << setw(8) << bestOf1Count 
             << setw(11) << fixed << setprecision(1) << percentage << "%"
             << setw(14) << avgDuration << "m\n";
    }
    
    if (bestOf3Count > 0) {
        float percentage = (static_cast<float>(bestOf3Count) / totalMatches) * 100.0f;
        float avgDuration = bestOf3Duration / bestOf3Count;
        cout << setw(15) << "Best of 3" << setw(8) << bestOf3Count 
             << setw(11) << fixed << setprecision(1) << percentage << "%"
             << setw(14) << avgDuration << "m\n";
    }
    
    if (bestOf5Count > 0) {
        float percentage = (static_cast<float>(bestOf5Count) / totalMatches) * 100.0f;
        float avgDuration = bestOf5Duration / bestOf5Count;
        cout << setw(15) << "Best of 5" << setw(8) << bestOf5Count 
             << setw(11) << fixed << setprecision(1) << percentage << "%"
             << setw(14) << avgDuration << "m\n";
    }
    
    cout << "\nTotal matches analyzed: " << totalMatches << "\n";
    cout << "===========================\n";
}

void GameResultLogger::exportPlayerStatistics() const {
    cout << "\n=== EXPORTED PLAYER STATISTICS ===\n";
    cout << "Format: CSV-like output for external analysis\n";
    cout << string(50, '-') << "\n";
    
    cout << "PlayerID,PlayerName,TotalMatches,Wins,Losses,WinRate,CurrentStreak,LongestStreak,MostUsedChampion,AvgDuration,LastMatch\n";
    
    for (int i = 0; i < playerCount; ++i) {
        const PlayerStats& stats = playerStatistics[i];
        if (stats.totalMatches > 0) {
            cout << stats.playerId << ","
                 << stats.playerName << ","
                 << stats.totalMatches << ","
                 << stats.wins << ","
                 << stats.losses << ","
                 << fixed << setprecision(2) << stats.winRate << ","
                 << stats.currentWinStreak << ","
                 << stats.longestWinStreak << ","
                 << "Champion#" << static_cast<int>(stats.mostUsedChampion) << ","
                 << fixed << setprecision(1) << stats.averageGameDuration << ","
                 << stats.lastMatchDate << "\n";
        }
    }
    
    cout << "\n" << playerCount << " player records exported.\n";
    cout << "===================================\n";
}

int GameResultLogger::getTotalMatchesLogged() const {
    return completeHistory.getSize();
}

bool GameResultLogger::hasPlayerData(int playerId) const {
    return findPlayerIndex(playerId) != -1;
}

void GameResultLogger::clearAllData() {
    // Clear all data structures
    while (!recentMatches.isEmpty()) {
        recentMatches.dequeue();
    }
    
    completeHistory.clear();
    
    while (!undoStack.isEmpty()) {
        undoStack.pop();
    }
    
    // Reset player statistics
    for (int i = 0; i < MAX_PLAYERS; ++i) {
        playerStatistics[i] = PlayerStats();
    }
    playerCount = 0;
    
    std::cout << "All game result data cleared.\n";
}

void GameResultLogger::loadResultsFromJSON() {
    loadResultsFromJSON("data/results.json");
}

void GameResultLogger::loadResultsFromJSON(const std::string& jsonPath) {
    // Try different paths if the file doesn't exist in the current directory
    std::string fullPath = jsonPath;
    if (!std::filesystem::exists(fullPath)) {
        fullPath = "c:/Users/CHUA/Documents/GitHub/ECMS_2025/" + jsonPath;
    }
    
    std::ifstream file(fullPath);
    if (!file.is_open()) {
        std::cout << "Error: Cannot open results JSON file: " << fullPath << "\n";
        std::cout << "Make sure the file exists and is accessible.\n";
        return;
    }
    
    try {
        json resultData;
        file >> resultData;
        
        // Clear existing loaded results
        loadedResultsCount = 0;
        
        for (const auto& item : resultData) {
            if (loadedResultsCount >= MAX_RESULTS) {
                std::cout << "Warning: Maximum results limit reached (" << MAX_RESULTS << ")\n";
                break;
            }
            
            // Extract basic result information
            int id = item["id"];
            int matchId = item["matchId"];
            int player1Id = item["player1Id"];
            int player2Id = item["player2Id"];
            int gamesPlayed = item["gamesPlayed"];
            std::string score = item["score"];
            int winnerId = item["winnerId"];
            
            // Parse champions for player 1
            Champion championsP1[Result::TEAM_SIZE];
            const auto& p1Champions = item["championsP1"];
            for (int i = 0; i < Result::TEAM_SIZE; ++i) {
                if (i < (int)p1Champions.size()) {
                    std::string championName = p1Champions[i];
                    championsP1[i] = Champion::NoChampion; // Load from JSON - champion mapping removed
                } else {
                    championsP1[i] = Champion::NoChampion;
                }
            }
            
            // Parse champions for player 2
            Champion championsP2[Result::TEAM_SIZE];
            const auto& p2Champions = item["championsP2"];
            for (int i = 0; i < Result::TEAM_SIZE; ++i) {
                if (i < (int)p2Champions.size()) {
                    std::string championName = p2Champions[i];
                    championsP2[i] = Champion::NoChampion; // Load from JSON - champion mapping removed
                } else {
                    championsP2[i] = Champion::NoChampion;
                }
            }
            
            // Store the result in our loaded results array with proper player IDs
            loadedResults[loadedResultsCount] = Result(id, matchId, gamesPlayed, score, championsP1, championsP2, winnerId);
            
            // Store match player information for statistics calculation
            matchPlayerInfo[loadedResultsCount] = MatchPlayerInfo(matchId, player1Id, player2Id, winnerId);
            
            loadedResultsCount++;
        }
        
        file.close();
        
        // Automatically calculate player statistics from loaded results
        calculatePlayerStatistics();
        
    } catch (const json::exception& e) {
        std::cout << "Error parsing JSON file: " << e.what() << "\n";
        file.close();
    } catch (const std::exception& e) {
        std::cout << "Error loading results from JSON: " << e.what() << "\n";
        file.close();
    }
}

void GameResultLogger::calculatePlayerStatistics() {
    // Reset player statistics
    for (int i = 0; i < MAX_PLAYERS; ++i) {
        playerStatistics[i] = PlayerStats();
    }
    playerCount = 0;
    
    // Analyze each loaded result
    for (int i = 0; i < loadedResultsCount; ++i) {
        const Result& result = loadedResults[i];
        const MatchPlayerInfo& playerInfo = matchPlayerInfo[i];
        
        // Get player IDs from the stored match player information
        int player1Id = playerInfo.player1Id;
        int player2Id = playerInfo.player2Id;
        int winnerId = playerInfo.winnerId;
        
        // Find or create player statistics entries
        int player1Index = findPlayerIndex(player1Id);
        if (player1Index == -1 && playerCount < MAX_PLAYERS) {
            playerStatistics[playerCount] = PlayerStats(player1Id, "Player" + std::to_string(player1Id));
            player1Index = playerCount++;
        }
        
        int player2Index = findPlayerIndex(player2Id);
        if (player2Index == -1 && playerCount < MAX_PLAYERS) {
            playerStatistics[playerCount] = PlayerStats(player2Id, "Player" + std::to_string(player2Id));
            player2Index = playerCount++;
        }
        
        // Update statistics for player 1
        if (player1Index != -1) {
            bool player1Won = (winnerId == player1Id);
            Champion player1Champion = (result.championsP1[0] != Champion::NoChampion) ? 
                                       result.championsP1[0] : Champion::NoChampion;
            playerStatistics[player1Index].updateStats(player1Won, player1Champion, "2024-01-01", 30.0f);
        }
        
        // Update statistics for player 2
        if (player2Index != -1) {
            bool player2Won = (winnerId == player2Id);
            Champion player2Champion = (result.championsP2[0] != Champion::NoChampion) ? 
                                       result.championsP2[0] : Champion::NoChampion;
            playerStatistics[player2Index].updateStats(player2Won, player2Champion, "2024-01-01", 30.0f);
        }
    }
}

void GameResultLogger::displayLoadedResults() const {
    std::cout << "\n=== LOADED RESULTS FROM JSON ===\n";
    
    if (loadedResultsCount == 0) {
        std::cout << "No results loaded from JSON.\n";
        return;
    }
    
    std::cout << std::left;
    std::cout << std::setw(6) << "ID" << std::setw(10) << "Match ID" 
              << std::setw(8) << "P1 ID" << std::setw(8) << "P2 ID"
              << std::setw(8) << "Games" << std::setw(10) << "Score"
              << std::setw(10) << "Winner" << std::setw(12) << "P1 Champ"
              << std::setw(12) << "P2 Champ\n";
    std::cout << std::string(84, '-') << "\n";
    
    for (int i = 0; i < loadedResultsCount; ++i) {
        const Result& result = loadedResults[i];
        const MatchPlayerInfo& playerInfo = matchPlayerInfo[i];
        
        std::cout << std::setw(6) << result.id
                  << std::setw(10) << result.matchId
                  << std::setw(8) << playerInfo.player1Id
                  << std::setw(8) << playerInfo.player2Id
                  << std::setw(8) << result.gamesPlayed
                  << std::setw(10) << result.score
                  << std::setw(10) << playerInfo.winnerId
                  << std::setw(12) << ("Champ#" + std::to_string(static_cast<int>(result.championsP1[0]))).substr(0, 11)
                  << ("Champ#" + std::to_string(static_cast<int>(result.championsP2[0]))).substr(0, 11) << "\n";
    }
    
    std::cout << "\nTotal loaded results: " << loadedResultsCount << "\n";
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
            for (int j = 0; j < 52; ++j) {
                if (stats.championUsageCount[j] > maxUsage) {
                    maxUsage = stats.championUsageCount[j];
                }
            }
            
            std::cout << std::setw(12) << stats.playerId
                      << std::setw(20) << stats.playerName
                      << std::setw(20) << ("Champion#" + std::to_string(static_cast<int>(stats.mostUsedChampion)))
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
                std::cout << "Most Used Champion: " << "Champion#" << static_cast<int>(stats.mostUsedChampion) << "\n";
            }
            
            // Show top 3 champions if available
            std::cout << "Champion Usage: ";
            int shownChampions = 0;
            for (int j = 0; j < 52 && shownChampions < 3; ++j) {
                if (stats.championUsageCount[j] > 0) {
                    if (shownChampions > 0) std::cout << ", ";
                    std::cout << "Champion#" << j 
                             << "(" << stats.championUsageCount[j] << ")";
                    shownChampions++;
                }
            }
            std::cout << "\n";
        }
    }
    
    std::cout << "\n========================================\n";
}

int GameResultLogger::getLoadedResultsCount() const {
    return loadedResultsCount;
}
