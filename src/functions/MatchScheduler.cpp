#include "functions/MatchScheduler.hpp"
#include <random>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <sstream>

int MatchScheduler::nextMatchIdCounter = 1;

// Load dummy check in players from JSON file
DynamicArray<Player> MatchScheduler::loadPlayersFromFile() {
    PriorityQueue<Player> checkedInPlayers;
    try {
        checkedInPlayers = JsonLoader::loadCheckedInPlayers(filename);
        std::cout << "Loaded " << checkedInPlayers.getSize() << " players from file.\n";
    } catch (const std::exception& e) {
        std::cerr << "Error loading players: " << e.what() << std::endl;
    }
    DynamicArray<Player> checkedInPlayersArray;
    int size = checkedInPlayers.getSize();
    for (int i = 0; i < size; ++i) {
        Player player = checkedInPlayers.dequeue();
        checkedInPlayersArray.push_back(player);
    }
    return checkedInPlayersArray;
}

DynamicArray<Match> MatchScheduler::scheduleQualifierMatches(const DynamicArray<Player>& players, const std::string& tournamentId) {
    DynamicArray<Match> matches;
    
    // Filter out wildcards for qualifiers
    DynamicArray<Player> qualifierPlayers = filterNonWildcards(players);
    
    if (qualifierPlayers.empty()) {
        std::cout << "No players available for qualifier matches (all are wildcards).\n";
        return matches;
    }
    
    if (qualifierPlayers.getSize() != 30) {
        std::cout << "Warning: Expected 30 players for group qualifiers, but got " 
                  << qualifierPlayers.getSize() << " players.\n";
    }
    
    // Create 6 groups of 5 players each based on points
    DynamicArray<DynamicArray<Player>> groups = createGroupsByPoints(qualifierPlayers, 6);
    
    std::cout << "Created " << groups.getSize() << " groups for round robin qualifiers:\n";
    for (int g = 0; g < groups.getSize(); ++g) {
        std::cout << "Group " << (g + 1) << ": " << groups[g].getSize() << " players\n";
    }
    
    // Schedule round robin matches for each group
    std::string currentDate = "2025-08-01";
    // std::string currentDate = formatDateTime();
    
    for (int groupIndex = 0; groupIndex < groups.getSize(); ++groupIndex) {
        DynamicArray<Match> groupMatches = scheduleRoundRobinMatches(
            groups[groupIndex], groupIndex + 1, tournamentId, currentDate
        );
        
        // Add group matches to overall matches
        for (int i = 0; i < groupMatches.getSize(); ++i) {
            matches.push_back(groupMatches[i]);
        }
    }
    
    std::cout << "Scheduled " << matches.getSize() << " qualifier matches across " 
              << groups.getSize() << " groups.\n";
    return matches;
}

DynamicArray<Match> MatchScheduler::scheduleStageMatches(const DynamicArray<Player>& players, TournamentStage stage, const std::string& tournamentId) {
    DynamicArray<Match> matches;
    
    if (players.empty()) {
        std::cout << "No players available for " << stageToString(stage) << " matches.\n";
        return matches;
    }
    
    if (players.getSize() % 2 != 0) {
        std::cout << "Warning: Odd number of players (" << players.getSize() 
                  << ") for " << stageToString(stage) << ". One player will receive a bye.\n";
    }
    
    // After qualifiers, use random pairing
    DynamicArray<PlayerPair> pairs = randomPairing(players);
    
    // Create matches from pairs
    std::string currentDate = formatDateTime();
    for (int i = 0; i < pairs.getSize(); ++i) {
        Match match = createMatch(getNextMatchId(), tournamentId, stage, 
                                pairs[i].first, pairs[i].second, currentDate, "14:00");
        matches.push_back(match);
    }
    
    std::cout << "Scheduled " << matches.getSize() << " " << stageToString(stage) << " matches.\n";
    return matches;
}

DynamicArray<PlayerPair> MatchScheduler::pairPlayersByPoints(const DynamicArray<Player>& players) {
    DynamicArray<PlayerPair> pairs;
    DynamicArray<Player> availablePlayers = players;
    
    // Custom sort by points (bubble sort for simplicity)
    for (int i = 0; i < availablePlayers.getSize() - 1; ++i) {
        for (int j = 0; j < availablePlayers.getSize() - i - 1; ++j) {
            if (availablePlayers[j].points < availablePlayers[j + 1].points) {
                Player temp = availablePlayers[j];
                availablePlayers[j] = availablePlayers[j + 1];
                availablePlayers[j + 1] = temp;
            }
        }
    }
    
    // Separate early birds and regular players
    DynamicArray<Player> earlyBirds = filterEarlyBirds(availablePlayers);
    DynamicArray<Player> regularPlayers;
    
    for (int i = 0; i < availablePlayers.getSize(); ++i) {
        if (!availablePlayers[i].isEarlyBird) {
            regularPlayers.push_back(availablePlayers[i]);
        }
    }
    
    // First, pair regular players with each other
    for (int i = 0; i < regularPlayers.getSize() - 1; i += 2) {
        pairs.push_back(PlayerPair(regularPlayers[i], regularPlayers[i + 1]));
    }
    
    // Then pair early birds with remaining regular players
    int regularIndex = (regularPlayers.getSize() % 2 == 0) ? regularPlayers.getSize() : regularPlayers.getSize() - 1;
    int earlyBirdIndex = 0;
    
    while (earlyBirdIndex < earlyBirds.getSize() && regularIndex < regularPlayers.getSize()) {
        pairs.push_back(PlayerPair(earlyBirds[earlyBirdIndex], regularPlayers[regularIndex]));
        earlyBirdIndex++;
        regularIndex++;
    }
    
    // If we have remaining early birds, we need to pair them together (but this violates the rule)
    // This should be handled as an exception case
    if (earlyBirdIndex < earlyBirds.getSize()) {
        std::cout << "Warning: Not enough regular players to pair with all early birds.\n";
        std::cout << "Some early birds will be paired together, violating the early bird protection rule.\n";
        
        for (int i = earlyBirdIndex; i < earlyBirds.getSize() - 1; i += 2) {
            pairs.push_back(PlayerPair(earlyBirds[i], earlyBirds[i + 1]));
        }
    }
    
    return pairs;
}

DynamicArray<PlayerPair> MatchScheduler::randomPairing(const DynamicArray<Player>& players) {
    DynamicArray<PlayerPair> pairs;
    DynamicArray<Player> shuffledPlayers = players;
    
    // Simple shuffle using random swaps
    std::random_device rd;
    std::mt19937 g(rd());
    
    for (int i = shuffledPlayers.getSize() - 1; i > 0; --i) {
        std::uniform_int_distribution<int> dis(0, i);
        int j = dis(g);
        
        Player temp = shuffledPlayers[i];
        shuffledPlayers[i] = shuffledPlayers[j];
        shuffledPlayers[j] = temp;
    }
    
    // Create pairs
    for (int i = 0; i < shuffledPlayers.getSize() - 1; i += 2) {
        pairs.push_back(PlayerPair(shuffledPlayers[i], shuffledPlayers[i + 1]));
    }
    
    return pairs;
}

DynamicArray<Player> MatchScheduler::filterWildcards(const DynamicArray<Player>& players) {
    DynamicArray<Player> wildcards;
    for (int i = 0; i < players.getSize(); ++i) {
        if (players[i].isWildcard) {
            wildcards.push_back(players[i]);
        }
    }
    return wildcards;
}

DynamicArray<Player> MatchScheduler::filterNonWildcards(const DynamicArray<Player>& players) {
    DynamicArray<Player> nonWildcards;
    for (int i = 0; i < players.getSize(); ++i) {
        if (!players[i].isWildcard) {
            nonWildcards.push_back(players[i]);
        }
    }
    return nonWildcards;
}

DynamicArray<Player> MatchScheduler::filterEarlyBirds(const DynamicArray<Player>& players) {
    DynamicArray<Player> earlyBirds;
    for (int i = 0; i < players.getSize(); ++i) {
        if (players[i].isEarlyBird) {
            earlyBirds.push_back(players[i]);
        }
    }
    return earlyBirds;
}

bool MatchScheduler::canPairPlayers(const Player& p1, const Player& p2, TournamentStage stage) {
    // For qualifiers, check early bird conflict
    if (stage == TournamentStage::Qualifiers) {
        return !hasEarlyBirdConflict(p1, p2);
    }
    
    // For other stages, any pairing is allowed
    return true;
}

bool MatchScheduler::hasEarlyBirdConflict(const Player& p1, const Player& p2) {
    // Early birds cannot be paired against each other in qualifiers
    return p1.isEarlyBird && p2.isEarlyBird;
}

Match MatchScheduler::createMatch(const std::string& matchId, const std::string& tournamentId, TournamentStage stage, 
                                const Player& player1, const Player& player2, 
                                const std::string& date, const std::string& time) {
    return Match(matchId, tournamentId, stage, date, time, player1.id, player2.id);
}

void MatchScheduler::displayMatchesByStage(const DynamicArray<Match>& matches, TournamentStage stage) {
    std::cout << "\n=== " << stageToString(stage) << " MATCHES ===\n";
    
    bool hasMatches = false;
    for (int i = 0; i < matches.getSize(); ++i) {
        if (matches[i].stage == stage) {
            std::cout << "Match " << matches[i].id << ": Player " << matches[i].player1 
                      << " vs Player " << matches[i].player2;
            if (!matches[i].time.empty() && matches[i].time != "TBD") {
                std::cout << " (Time: " << matches[i].time << ")";
            }
            std::cout << "\n";
            hasMatches = true;
        }
    }
    
    if (!hasMatches) {
        std::cout << "No matches scheduled for this stage.\n";
    }
    std::cout << "================================\n";
}

void MatchScheduler::displayTournamentBracket(const DynamicArray<Match>& matches) {
    std::cout << "\n=== TOURNAMENT BRACKET ===\n";
    
    // Display by stage progression
    DynamicArray<TournamentStage> stageOrder;
    stageOrder.push_back(TournamentStage::Qualifiers);
    stageOrder.push_back(TournamentStage::Quarterfinals);
    stageOrder.push_back(TournamentStage::Semifinals);
    stageOrder.push_back(TournamentStage::Finals);
    
    for (int i = 0; i < stageOrder.getSize(); ++i) {
        displayMatchesByStage(matches, stageOrder[i]);
    }
}

std::string MatchScheduler::stageToString(TournamentStage stage) {
    switch (stage) {
        case TournamentStage::Registration: return "Registration";
        case TournamentStage::Qualifiers: return "Qualifiers";
        case TournamentStage::Tiebreakers: return "Tiebreakers";
        case TournamentStage::Quarterfinals: return "Quarterfinals";
        case TournamentStage::Semifinals: return "Semifinals";
        case TournamentStage::Finals: return "Finals";
        case TournamentStage::Completed: return "Completed";
        default: return "Unknown Stage";
    }
}

std::string MatchScheduler::formatDateTime() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d");
    return ss.str();
}

std::string MatchScheduler::getNextMatchId() {
    std::string counterStr = std::to_string(nextMatchIdCounter);
    std::string id = "M" + std::string(5 - counterStr.length(), '0') + counterStr;
    nextMatchIdCounter++;
    return id;
}

// Group-based qualifier functions
DynamicArray<DynamicArray<Player>> MatchScheduler::createGroupsByPoints(const DynamicArray<Player>& players, int numGroups) {
    DynamicArray<DynamicArray<Player>> groups;
    
    // Initialize groups
    for (int i = 0; i < numGroups; ++i) {
        groups.push_back(DynamicArray<Player>());
    }
    
    // Sort players by points (descending order)
    DynamicArray<Player> sortedPlayers = players;
    for (int i = 0; i < sortedPlayers.getSize() - 1; ++i) {
        for (int j = 0; j < sortedPlayers.getSize() - i - 1; ++j) {
            if (sortedPlayers[j].points < sortedPlayers[j + 1].points) {
                Player temp = sortedPlayers[j];
                sortedPlayers[j] = sortedPlayers[j + 1];
                sortedPlayers[j + 1] = temp;
            }
        }
    }
    
    // Distribute players to groups in snake draft pattern for balanced groups
    // This ensures each group has a mix of high and low-ranked players
    for (int i = 0; i < sortedPlayers.getSize(); ++i) {
        int groupIndex = (i / numGroups) % 2 == 0 ? 
                        (i % numGroups) : 
                        (numGroups - 1 - (i % numGroups));
        groups[groupIndex].push_back(sortedPlayers[i]);
    }
    
    // Display group composition
    for (int g = 0; g < groups.getSize(); ++g) {
        std::cout << "Group " << (g + 1) << " players: ";
        for (int p = 0; p < groups[g].getSize(); ++p) {
            std::cout << groups[g][p].name << "(" << groups[g][p].points << ") ";
        }
        std::cout << "\n";
    }
    
    return groups;
}

DynamicArray<Match> MatchScheduler::scheduleRoundRobinMatches(const DynamicArray<Player>& groupPlayers, int groupNumber, 
                                                             const std::string& tournamentId, const std::string& date) {
    DynamicArray<Match> matches;
    
    std::cout << "\nScheduling round robin for Group " << groupNumber << ":\n";
    
    // Create all possible pairings within the group (round robin)
    for (int i = 0; i < groupPlayers.getSize(); ++i) {
        for (int j = i + 1; j < groupPlayers.getSize(); ++j) {
            std::string matchId = getNextMatchId();
            Match match = createMatch(matchId, tournamentId, TournamentStage::Qualifiers,
                                    groupPlayers[i], groupPlayers[j], date, "TBD");
            
            // Add group identifier to match (we can use a comment or extend Match structure)
            matches.push_back(match);
            
            std::cout << "  " << matchId << ": " << groupPlayers[i].name 
                      << " vs " << groupPlayers[j].name << "\n";
        }
    }
    
    return matches;
}

DynamicArray<Player> MatchScheduler::determineGroupWinners(const DynamicArray<Player>& groupPlayers, const DynamicArray<Match>& groupMatches) {
    DynamicArray<Player> winners;
    
    // For now, return the first player (this would be replaced with actual match result analysis)
    // In a real implementation, this would:
    // 1. Analyze match results to calculate wins/losses for each player
    // 2. Handle ties with head-to-head records
    // 3. Call tiebreaker matches if needed
    
    if (!groupPlayers.empty()) {
        winners.push_back(groupPlayers[0]); // Placeholder - return highest ranked player
    }
    
    return winners;
}

DynamicArray<Match> MatchScheduler::scheduleTiebreakerMatches(const DynamicArray<Player>& tiedPlayers, int groupNumber,
                                                             const std::string& tournamentId, const std::string& date) {
    DynamicArray<Match> tiebreakerMatches;
    
    std::cout << "\nScheduling tiebreaker matches for Group " << groupNumber << ":\n";
    
    // Simple tiebreaker: if 2 players tied, single match
    // If 3+ players tied, mini round robin or elimination
    if (tiedPlayers.getSize() == 2) {
        std::string matchId = getNextMatchId();
        Match match = createMatch(matchId, tournamentId, TournamentStage::Tiebreakers,
                                tiedPlayers[0], tiedPlayers[1], date, "TBD");
        tiebreakerMatches.push_back(match);
        
        std::cout << "  Tiebreaker " << matchId << ": " << tiedPlayers[0].name 
                  << " vs " << tiedPlayers[1].name << "\n";
    } else if (tiedPlayers.getSize() > 2) {
        // Mini round robin for multiple tied players
        for (int i = 0; i < tiedPlayers.getSize(); ++i) {
            for (int j = i + 1; j < tiedPlayers.getSize(); ++j) {
                std::string matchId = getNextMatchId();
                Match match = createMatch(matchId, tournamentId, TournamentStage::Tiebreakers,
                                        tiedPlayers[i], tiedPlayers[j], date, "TBD");
                tiebreakerMatches.push_back(match);
                
                std::cout << "  Tiebreaker " << matchId << ": " << tiedPlayers[i].name 
                          << " vs " << tiedPlayers[j].name << "\n";
            }
        }
    }
    
    return tiebreakerMatches;
}
