#ifndef GAMERESULTLOGGER_HPP
#define GAMERESULTLOGGER_HPP

#include "../dto/Match.hpp"
#include "../dto/Result.hpp"
#include "../dto/Performance.hpp"
#include "../dto/Player.hpp"
#include "../structures/Stack.hpp"
#include "../helper/JsonLoader.hpp"
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
    std::string playerId;
    std::string playerName;
    int totalMatches;
    int wins;
    int losses;
    float winRate;
    Champion mostUsedChampion;
    int championUsageCount[12]; // Track usage for each champion (0-11)
    std::string lastMatchDate;
    int currentWinStreak;
    int longestWinStreak;
    float averageGameDuration; // in minutes
    
    PlayerStats() : playerId(""), playerName(""), totalMatches(0), wins(0), losses(0), 
                   winRate(0.0f), mostUsedChampion(Champion::NoChampion), 
                   lastMatchDate(""), currentWinStreak(0), longestWinStreak(0),
                   averageGameDuration(0.0f) {
        for (int i = 0; i < 12; ++i) {
            championUsageCount[i] = 0;
        }
    }
    
    PlayerStats(const std::string& id, const std::string& name) : playerId(id), playerName(name),
                totalMatches(0), wins(0), losses(0), winRate(0.0f),
                mostUsedChampion(Champion::NoChampion), lastMatchDate(""),
                currentWinStreak(0), longestWinStreak(0), averageGameDuration(0.0f) {
        for (int i = 0; i < 12; ++i) {
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
    float duration; // in minutes
    
    MatchSummary() : matchId(0), date(""), player1Name(""), player2Name(""), 
                    score(""), winnerName(""), stage(TournamentStage::Qualifiers),
                    duration(0.0f) {}
    
    MatchSummary(int id, const std::string& d, const std::string& p1, const std::string& p2,
                const std::string& sc, const std::string& winner, TournamentStage st, float dur) 
                : matchId(id), date(d), player1Name(p1), player2Name(p2), score(sc),
                  winnerName(winner), stage(st), duration(dur) {}
};

// Helper structure to store match player information from JSON
struct MatchPlayerInfo {
    std::string matchId;
    std::string player1Id;
    std::string player2Id;
    std::string winnerId;
    
    MatchPlayerInfo() : matchId(""), player1Id(""), player2Id(""), winnerId("") {}
    MatchPlayerInfo(const std::string& mid, const std::string& p1, const std::string& p2, const std::string& winner) 
        : matchId(mid), player1Id(p1), player2Id(p2), winnerId(winner) {}
};

class GameResultLogger {
private:
    // ===============================================
    // TASK 4 CORE DATA STRUCTURES (JsonLoader → DoublyLinkedList → Stack)
    // ===============================================
    
    // JsonLoader handles DoublyLinkedList internally - we don't store it directly
    int loadedResultsCount;
    
    // Stack-based operations for Task 4 functionality
    Stack<Result> searchResultsStack;                 // For storing search results
    Stack<PlayerStats> playerAnalysisStack;           // For player analysis operations
    Stack<std::string> operationHistoryStack;        // Track operations performed
    Stack<MatchSummary> processingStack;              // For general processing operations
    
    // Player performance tracking (simplified for Task 4)
    static const int MAX_PLAYERS = 100;
    static const int MAX_RECENT_MATCHES = 50;
    PlayerStats playerStatistics[MAX_PLAYERS];
    int playerCount;
    
    // Helper functions
    int findPlayerIndex(const std::string& playerId) const;
    std::string stageToString(TournamentStage stage) const;
    
public:
    GameResultLogger();
    ~GameResultLogger();
    
    // ===============================================
    // TASK 4 CORE FUNCTIONALITY (JsonLoader → DoublyLinkedList → Stack)
    // ===============================================
    
    // JSON Data Loading using JsonLoader (which handles DoublyLinkedList internally)
    void loadResultsFromJSON();
    void loadResultsFromJSON(const std::string& jsonPath);
    
    // Data operations using JsonLoader results
    void displayLoadedResults() const;                // Display results from JsonLoader
    void traverseResultsForward() const;              // Forward traversal via JsonLoader
    void traverseResultsBackward() const;             // Backward traversal via JsonLoader
    void findResultInList(const std::string& matchId) const;         // Find result using JsonLoader
    void filterResultsByPlayer(const std::string& playerId) const;   // Filter results from JsonLoader
    
    // Stack-based operations for search and analysis
    void pushSearchResult(const Result& result);      // Push result to search stack
    Result popSearchResult();                          // Pop result from search stack
    void clearSearchResults();                        // Clear search results stack
    void displaySearchResultsStack() const;           // Display all results in search stack
    
    void pushPlayerAnalysis(const PlayerStats& stats); // Push player stats to analysis stack
    PlayerStats popPlayerAnalysis();                   // Pop player stats from analysis stack
    void processPlayerAnalysisStack();                 // Process all players in analysis stack
    void displayPlayerAnalysisStack() const;          // Display analysis stack contents
    
    void recordOperation(const std::string& operation); // Record operation in history stack
    std::string getLastOperation();                     // Get last operation from history
    void displayOperationHistory() const;              // Display operation history stack
    void clearOperationHistory();                      // Clear operation history
    
    // Statistical Analysis from JsonLoader data (DoublyLinkedList handled internally)
    void calculatePlayerStatistics();
    void displayPlayerPerformance(const std::string& playerId) const;
    void searchMatchesByPlayer(const std::string& playerId) const;
    void searchMatchesByMatchId(const std::string& matchId) const;
    void displayPlayerFavoriteChampions() const;
    void displayPlayerMatchCount() const;
    void displayPlayerWinRates() const;
    void displayComprehensivePlayerStats() const;
    
    // Utility functions
    int getLoadedResultsCount() const;
    bool hasPlayerData(const std::string& playerId) const;
};

#endif // GAMERESULTLOGGER_HPP
