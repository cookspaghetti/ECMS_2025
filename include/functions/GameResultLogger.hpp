#ifndef GAMERESULTLOGGER_HPP
#define GAMERESULTLOGGER_HPP

#include "../dto/Match.hpp"
#include "../dto/Result.hpp"
#include "../dto/Performance.hpp"
#include "../dto/Player.hpp"
#include "../structures/CircularQueue.hpp"
#include "../structures/Queue.hpp"
#include "../structures/Stack.hpp"
#include "../structures/PriorityQueue.hpp"
#include "nlohmann/json.hpp"
#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <filesystem>

// Forward declarations for custom data structures
template<typename T>
struct HistoryNode {
    T data;
    HistoryNode* next;
    
    HistoryNode(const T& item) : data(item), next(nullptr) {}
};

template<typename T>
class LinkedHistory {
private:
    HistoryNode<T>* head;
    HistoryNode<T>* tail;
    int size;
    
public:
    LinkedHistory() : head(nullptr), tail(nullptr), size(0) {}
    
    ~LinkedHistory() {
        clear();
    }
    
    void addRecord(const T& record) {
        HistoryNode<T>* newNode = new HistoryNode<T>(record);
        if (!head) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
        ++size;
    }
    
    void clear() {
        while (head) {
            HistoryNode<T>* temp = head;
            head = head->next;
            delete temp;
        }
        head = tail = nullptr;
        size = 0;
    }
    
    int getSize() const { return size; }
    bool isEmpty() const { return head == nullptr; }
    
    // Iterator-like access
    HistoryNode<T>* getHead() const { return head; }
};

// Player statistics structure for performance tracking
struct PlayerStats {
    int playerId;
    std::string playerName;
    int totalMatches;
    int wins;
    int losses;
    float winRate;
    Champion mostUsedChampion;
    int championUsageCount[52]; // Track usage for each champion
    std::string lastMatchDate;
    int currentWinStreak;
    int longestWinStreak;
    float averageGameDuration; // in minutes
    
    PlayerStats() : playerId(0), playerName(""), totalMatches(0), wins(0), losses(0), 
                   winRate(0.0f), mostUsedChampion(Champion::NoChampion), 
                   lastMatchDate(""), currentWinStreak(0), longestWinStreak(0),
                   averageGameDuration(0.0f) {
        for (int i = 0; i < 52; ++i) {
            championUsageCount[i] = 0;
        }
    }
    
    PlayerStats(int id, const std::string& name) : playerId(id), playerName(name),
                totalMatches(0), wins(0), losses(0), winRate(0.0f),
                mostUsedChampion(Champion::NoChampion), lastMatchDate(""),
                currentWinStreak(0), longestWinStreak(0), averageGameDuration(0.0f) {
        for (int i = 0; i < 52; ++i) {
            championUsageCount[i] = 0;
        }
    }
    
    void updateStats(bool isWin, Champion champion, const std::string& matchDate, float gameDuration) {
        totalMatches++;
        if (isWin) {
            wins++;
            currentWinStreak++;
            if (currentWinStreak > longestWinStreak) {
                longestWinStreak = currentWinStreak;
            }
        } else {
            losses++;
            currentWinStreak = 0;
        }
        
        winRate = (totalMatches > 0) ? (static_cast<float>(wins) / totalMatches) * 100.0f : 0.0f;
        
        // Update champion usage
        if (champion != Champion::NoChampion) {
            int championIndex = static_cast<int>(champion);
            if (championIndex >= 0 && championIndex < 52) {
                championUsageCount[championIndex]++;
                
                // Find most used champion
                int maxUsage = 0;
                for (int i = 0; i < 52; ++i) {
                    if (championUsageCount[i] > maxUsage) {
                        maxUsage = championUsageCount[i];
                        mostUsedChampion = static_cast<Champion>(i);
                    }
                }
            }
        }
        
        lastMatchDate = matchDate;
        
        // Update average game duration
        if (totalMatches > 1) {
            averageGameDuration = ((averageGameDuration * (totalMatches - 1)) + gameDuration) / totalMatches;
        } else {
            averageGameDuration = gameDuration;
        }
    }
};

// Match summary for recent results display
struct MatchSummary {
    int matchId;
    std::string date;
    std::string player1Name;
    std::string player2Name;
    std::string score;
    std::string winnerName;
    TournamentStage stage;
    MatchType matchType;
    float duration; // in minutes
    
    MatchSummary() : matchId(0), date(""), player1Name(""), player2Name(""), 
                    score(""), winnerName(""), stage(TournamentStage::Qualifiers),
                    matchType(MatchType::BestOf1), duration(0.0f) {}
    
    MatchSummary(int id, const std::string& d, const std::string& p1, const std::string& p2,
                const std::string& sc, const std::string& winner, TournamentStage st,
                MatchType mt, float dur) 
                : matchId(id), date(d), player1Name(p1), player2Name(p2), score(sc),
                  winnerName(winner), stage(st), matchType(mt), duration(dur) {}
};

// Helper structure to store match player information from JSON
struct MatchPlayerInfo {
    int matchId;
    int player1Id;
    int player2Id;
    int winnerId;
    
    MatchPlayerInfo() : matchId(0), player1Id(0), player2Id(0), winnerId(0) {}
    MatchPlayerInfo(int mid, int p1, int p2, int winner) 
        : matchId(mid), player1Id(p1), player2Id(p2), winnerId(winner) {}
};

class GameResultLogger {
private:
    // Recent match results (circular queue for efficient storage)
    static const int MAX_RECENT_MATCHES = 50;
    CircularQueue<MatchSummary> recentMatches;
    
    // Complete match history (linked list for unlimited storage)
    LinkedHistory<MatchSummary> completeHistory;
    
    // Loaded results storage (array/linked list for analysis)
    static const int MAX_RESULTS = 200;
    Result loadedResults[MAX_RESULTS];
    MatchPlayerInfo matchPlayerInfo[MAX_RESULTS]; // Store player info for each match
    int loadedResultsCount;
    
    // Player performance tracking
    static const int MAX_PLAYERS = 100;
    PlayerStats playerStatistics[MAX_PLAYERS];
    int playerCount;
    
    // Performance metrics priority queue (for rankings)
    PriorityQueue<PlayerStats> performanceRankings;
    
    // Match results stack (for undo functionality)
    Stack<MatchSummary> undoStack;
    
    // Helper functions
    int findPlayerIndex(int playerId) const;
    std::string stageToString(TournamentStage stage) const;
    std::string matchTypeToString(MatchType type) const;
    float generateRandomDuration(MatchType type) const; // Simulate match duration
    
public:
    GameResultLogger();
    ~GameResultLogger();
    
    // Core functionality
    void logMatchResult(const Match& match, const Result& result, 
                       const std::string& player1Name, const std::string& player2Name);
    void displayRecentResults(int count = 10) const;
    void displayPlayerPerformance(int playerId) const;
    void displayAllPlayerRankings() const;
    void displayMatchHistory(int count = 20) const;
    
    // Advanced features
    void searchMatchesByPlayer(int playerId) const;
    void searchMatchesByMatchId(int matchId) const;
    void searchMatchesByDate(const std::string& date) const;
    void displayTournamentSummary(TournamentStage stage) const;
    void exportPlayerStatistics() const;
    void undoLastResult();
    
    // Statistical analysis
    void displayWinRateAnalysis() const;
    void displayChampionUsageStats() const;
    void displayMatchTypeAnalysis() const;
    
    // Utility functions
    int getTotalMatchesLogged() const;
    bool hasPlayerData(int playerId) const;
    void clearAllData();
    
    // JSON Data Loading
    void loadResultsFromJSON();
    void loadResultsFromJSON(const std::string& jsonPath);
    
    // Statistical Analysis from Loaded Results
    void calculatePlayerStatistics();
    void displayLoadedResults() const;
    void displayPlayerMatchCount() const;
    void displayPlayerWinRates() const;
    void displayPlayerFavoriteChampions() const;
    void displayComprehensivePlayerStats() const;
    int getLoadedResultsCount() const;
};

#endif // GAMERESULTLOGGER_HPP
