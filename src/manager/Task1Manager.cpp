#include "manager/Task1Manager.hpp"
#include "helper/JsonLoader.hpp"
#include "helper/JsonWriter.hpp"
#include "functions/GameResultLogger.hpp"
#include "structures/DoublyLinkedList.hpp"
#include <iostream>
#include <random>
#include <iomanip>
#include <cstdio>

Task1Manager::Task1Manager(TournamentManager& tm) 
    : tournamentManager(tm), currentTournamentId("") {
    // Load existing matches if available
    allMatches = loadMatchesFromFile();
}

void Task1Manager::startMatchScheduling() {
    std::cout << "\n=== STARTING MATCH SCHEDULING & PLAYER PROGRESSION ===\n";
        
    // Get current tournament
    Tournament* activeTournament = tournamentManager.getOngoingTournament();
    if (!activeTournament) {
        std::cout << "No active tournament found. Cannot schedule matches.\n";
        return;
    }
    
    currentTournamentId = activeTournament->id;
    std::cout << "Scheduling matches for Tournament: " << activeTournament->name 
              << " (ID: " << currentTournamentId << ")\n";
    
    // Check if tournament is already past the initial scheduling phase
    TournamentStage currentStage = determineCurrentStage();
    
    // Handle Finals or Completed tournaments - offer restart option
    if (currentStage == TournamentStage::Finals || currentStage == TournamentStage::Completed) {
        std::cout << "Tournament is currently at " << MatchScheduler::stageToString(currentStage) 
                  << " stage.\n";
        std::cout << "Do you want to restart the tournament matchmaking? This will:\n";
        std::cout << "  - Clear all existing matches and results\n";
        std::cout << "  - Reset tournament stage to Qualifiers\n";
        std::cout << "  - Start fresh matchmaking with current players\n";
        std::cout << "Enter 'Y' or 'y' to restart, any other key to cancel: ";
        
        char choice;
        std::cin >> choice;
        std::cin.ignore(); // Clear the input buffer
        
        if (choice == 'Y' || choice == 'y') {
            std::cout << "\nRestarting tournament matchmaking...\n";
            
            // Clear in-memory data only first
            clearMemoryOnly();
            
            // Clear matches.json file
            try {
                DoublyLinkedList<Match> emptyMatches;
                JsonWriter::writeAllMatches(emptyMatches, "data/matches.json");
                std::cout << "Cleared matches.json successfully.\n";
            } catch (const std::exception& e) {
                std::cout << "Warning: Could not clear matches.json: " << e.what() << "\n";
            }
            
            // Clear results.json file
            try {
                DoublyLinkedList<Result> emptyResults;
                JsonWriter::writeAllResults(emptyResults, "data/results.json");
                std::cout << "Cleared results.json successfully.\n";
            } catch (const std::exception& e) {
                std::cout << "Warning: Could not clear results.json: " << e.what() << "\n";
            }
            
            // Reset tournament stage to Qualifiers
            updateTournamentStage(TournamentStage::Qualifiers);
            
            std::cout << "Tournament reset complete. Proceeding with fresh matchmaking...\n\n";
        } else {
            std::cout << "Tournament restart cancelled.\n";
            std::cout << "Use 'Advance to Next Stage' option to continue tournament progression.\n";
            return;
        }
    } else if (currentStage != TournamentStage::Registration && currentStage != TournamentStage::Qualifiers) {
        std::cout << "Tournament is already at " << MatchScheduler::stageToString(currentStage) 
                  << " stage. Initial match scheduling is not allowed.\n";
        std::cout << "Use 'Advance to Next Stage' option to continue tournament progression.\n";
        return;
    }
    
    // Load players from checked-in players JSON file
    DynamicArray<Player> allPlayers = matchScheduler.loadPlayersFromFile();
    if (allPlayers.empty()) {
        std::cout << "No players found. Cannot schedule matches.\n";
        return;
    }
    std::cout << "Total players in queue: " << allPlayers.getSize() << "\n";
    
    // Separate wildcards and regular players
    DynamicArray<Player> wildcardPlayers = getWildcardPlayers(allPlayers);
    DynamicArray<Player> regularPlayers;
    DynamicArray<Player> earlyBirdPlayers;
    
    for (int i = 0; i < allPlayers.getSize(); ++i) {
        if (!allPlayers[i].isWildcard) {
            regularPlayers.push_back(allPlayers[i]);
        }
        if (allPlayers[i].isEarlyBird && !allPlayers[i].isWildcard) {
            earlyBirdPlayers.push_back(allPlayers[i]);
        }
    }
    
    std::cout << "Wildcard players (skip qualifiers): " << wildcardPlayers.getSize() << "\n";
    std::cout << "Regular players (need qualifiers): " << regularPlayers.getSize() << "\n";
    std::cout << "Early bird players (included in regular): " << earlyBirdPlayers.getSize() << "\n";

    // Schedule group-based qualifier matches for non-wildcard players
    if (!regularPlayers.empty()) {
        std::cout << "\n=== STARTING GROUP-BASED QUALIFIERS ===\n";
        std::cout << "Expected: 30 players in 6 groups of 5 players each\n";
        std::cout << "Format: Round robin within each group, top player from each group advances\n";
        
        DynamicArray<Match> qualifierMatches = scheduleQualifiers(regularPlayers);
        
        // Append qualifier matches to all matches
        for (int i = 0; i < qualifierMatches.getSize(); ++i) {
            allMatches.push_back(qualifierMatches[i]);
        }
        
        // Simulate group stage and determine 6 group winners
        DynamicArray<Player> groupWinners = simulateGroupStageAndGetWinners(qualifierMatches, regularPlayers);
        
        // Combine 6 group winners with 2 wildcard players for quarterfinals (8 total)
        advancedPlayers = groupWinners;
        for (int i = 0; i < wildcardPlayers.getSize(); ++i) {
            advancedPlayers.push_back(wildcardPlayers[i]);
        }
        
        std::cout << "\n=== QUALIFICATION RESULTS ===\n";
        std::cout << "Group winners advancing: " << groupWinners.getSize() << "\n";
        std::cout << "Wildcard players advancing: " << wildcardPlayers.getSize() << "\n";
        std::cout << "Total advancing to quarterfinals: " << advancedPlayers.getSize() << "\n";
        
        if (advancedPlayers.getSize() != 8) {
            std::cout << "Warning: Expected 8 players for quarterfinals, but got " 
                      << advancedPlayers.getSize() << " players.\n";
        }
    } else {
        // Only wildcards available, they advance directly
        std::cout << "\n=== NO QUALIFIERS NEEDED ===\n";
        std::cout << "All available players are wildcards, advancing directly to quarterfinals.\n";
        
        advancedPlayers = wildcardPlayers;
        
        if (advancedPlayers.getSize() < 8) {
            std::cout << "Warning: Not enough players for proper quarterfinals bracket.\n";
        }
    }
    
    std::cout << "Players advancing to next stage: " << advancedPlayers.getSize() << "\n";
    
    // Save current matches
    saveMatchesToFile(allMatches);
    
    std::cout << "Match scheduling phase completed.\n";
    std::cout << "Use menu options to view brackets or advance to next stages.\n";
}

DynamicArray<Match> Task1Manager::scheduleQualifiers(const DynamicArray<Player>& players) {
    std::cout << "\n--- SCHEDULING QUALIFIER MATCHES ---\n";
    
    DynamicArray<Match> qualifierMatches = MatchScheduler::scheduleQualifierMatches(players, currentTournamentId);
    
    if (!qualifierMatches.empty()) {
        std::cout << "Qualifier matches scheduled successfully:\n";
        MatchScheduler::displayMatchesByStage(qualifierMatches, TournamentStage::Qualifiers);
    }
    
    return qualifierMatches;
}

DynamicArray<Match> Task1Manager::scheduleNextStage(TournamentStage stage, const DynamicArray<Player>& winners) {
    std::cout << "\n--- SCHEDULING " << MatchScheduler::stageToString(stage) << " MATCHES ---\n";
    
    if (!canAdvanceToNextStage(stage, winners.getSize())) {
        std::cout << "Cannot advance to " << MatchScheduler::stageToString(stage) 
                  << " with " << winners.getSize() << " players.\n";
        return DynamicArray<Match>();
    }
    
    DynamicArray<Match> stageMatches = MatchScheduler::scheduleStageMatches(winners, stage, currentTournamentId);
    
    if (!stageMatches.empty()) {
        std::cout << MatchScheduler::stageToString(stage) << " matches scheduled successfully:\n";
        MatchScheduler::displayMatchesByStage(stageMatches, stage);
        
        // Add to main matches collection
        for (int i = 0; i < stageMatches.getSize(); ++i) {
            allMatches.push_back(stageMatches[i]);
        }
        
        // Save updated matches
        saveMatchesToFile(allMatches);
        
        // Automatically simulate the matches to determine results
        std::cout << "\n--- AUTO-SIMULATING " << MatchScheduler::stageToString(stage) << " MATCHES ---\n";
        DynamicArray<Player> stageWinners = simulateMatchResults(stageMatches, winners);
        
        // Update advanced players with the winners from this stage
        if (!stageWinners.empty()) {
            advancedPlayers = stageWinners;
            std::cout << "Stage simulation complete. " << stageWinners.getSize() 
                      << " players advanced from " << MatchScheduler::stageToString(stage) << "\n";
        }
    }
    
    return stageMatches;
}

DynamicArray<Player> Task1Manager::getWildcardPlayers(const DynamicArray<Player>& players) {
    return MatchScheduler::filterWildcards(players);
}

DynamicArray<Player> Task1Manager::simulateMatchResults(const DynamicArray<Match>& matches, const DynamicArray<Player>& players) {
    std::cout << "\n--- SIMULATING MATCH RESULTS ---\n";
    DynamicArray<Player> winners;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1);
    
    for (int i = 0; i < matches.getSize(); ++i) {
        // Find the players in the match
        Player player1, player2;
        bool foundP1 = false, foundP2 = false;
        
        for (int j = 0; j < players.getSize(); ++j) {
            if (players[j].id == matches[i].player1) {
                player1 = players[j];
                foundP1 = true;
            }
            if (players[j].id == matches[i].player2) {
                player2 = players[j];
                foundP2 = true;
            }
        }
        
        if (foundP1 && foundP2) {
            // Simulate result based on points (higher points = better chance)
            int totalPoints = player1.points + player2.points;
            Player winner;
            bool player1Wins;
            
            if (totalPoints > 0) {
                double player1Chance = static_cast<double>(player1.points) / totalPoints;
                std::uniform_real_distribution<> realDis(0.0, 1.0);
                player1Wins = realDis(gen) < player1Chance;
            } else {
                // Random if no points difference
                player1Wins = dis(gen) == 0;
            }

            // Select random champions for the match
            Champion championP1 = getRandomChampion(gen);
            Champion championP2 = getRandomChampion(gen);
            
            winner = player1Wins ? player1 : player2;
            winners.push_back(winner);
            
            std::cout << "Match " << matches[i].id << ": " 
                      << winner.name << " wins vs " 
                      << (player1Wins ? player2.name : player1.name) 
                      << " (Champions: " << JsonWriter::championToString(championP1) 
                      << " vs " << JsonWriter::championToString(championP2) << ")\n";
            
            // Log the result by creating a Result object and saving to results.json
            try {
                Result result;
                result.id = generateUniqueResultId(); // Generate unique result ID
                result.matchId = matches[i].id;
                result.championsP1 = championP1;
                result.championsP2 = championP2;
                result.winnerId = winner.id;
                
                // Save to results using JsonWriter (simplified approach)
                saveResultToFile(result);
            } catch (const std::exception& e) {
                std::cout << "Warning: Could not log match result: " << e.what() << "\n";
            }
        }
    }
    
    std::cout << "Simulation complete. " << winners.getSize() << " winners determined.\n";
    return winners;
}

void Task1Manager::advanceToNextStage(const DynamicArray<Match>& currentMatches) {
    if (currentMatches.empty()) {
        std::cout << "No matches to advance from.\n";
        return;
    }
    
    TournamentStage currentStage = currentMatches[0].stage;
    TournamentStage nextStage = getNextStage(currentStage);
    
    if (nextStage == currentStage) {
        std::cout << "Already at final stage or invalid stage progression.\n";
        return;
    }
    
    // Simulate results and get winners
    DynamicArray<Player> winners = simulateMatchResults(currentMatches, advancedPlayers);
    
    if (winners.empty()) {
        std::cout << "No winners to advance to next stage.\n";
        return;
    }
    
    // Schedule next stage matches
    DynamicArray<Match> nextStageMatches = scheduleNextStage(nextStage, winners);
    
    if (!nextStageMatches.empty()) {
        advancedPlayers = winners;
        std::cout << "Successfully advanced " << winners.getSize() 
                  << " players to " << MatchScheduler::stageToString(nextStage) << "\n";
    }
}

bool Task1Manager::canAdvanceToNextStage(TournamentStage currentStage, int playerCount) {
    int requiredPlayers = getRequiredPlayersForStage(currentStage);
    return playerCount >= requiredPlayers;
}

TournamentStage Task1Manager::getNextStage(TournamentStage currentStage) const {
    switch (currentStage) {
        case TournamentStage::Registration:
            return TournamentStage::Qualifiers;
        case TournamentStage::Qualifiers:
            return TournamentStage::Quarterfinals;  // Skip tiebreakers for now, handle within qualifiers
        case TournamentStage::Tiebreakers:
            return TournamentStage::Quarterfinals;
        case TournamentStage::Quarterfinals:
            return TournamentStage::Semifinals;
        case TournamentStage::Semifinals:
            return TournamentStage::Finals;
        case TournamentStage::Finals:
            return TournamentStage::Completed;
        case TournamentStage::Completed:
            return TournamentStage::Completed;  // Already completed
        default:
            return currentStage; // No progression available
    }
}

int Task1Manager::getRequiredPlayersForStage(TournamentStage stage) {
    switch (stage) {
        case TournamentStage::Quarterfinals:
            return 8;
        case TournamentStage::Semifinals:
            return 4;
        case TournamentStage::Finals:
            return 2;
        default:
            return 2; // Minimum for any match
    }
}

void Task1Manager::displayCurrentBracket() const {
    std::cout << "\n=== CURRENT TOURNAMENT BRACKET ===\n";
    
    if (allMatches.empty()) {
        std::cout << "No matches scheduled yet.\n";
        return;
    }
    
    // Show current tournament stage
    TournamentStage currentStage = determineCurrentStage();
    std::cout << "Current Tournament Stage: " << MatchScheduler::stageToString(currentStage) << "\n\n";
    
    MatchScheduler::displayTournamentBracket(allMatches);
    
    if (!advancedPlayers.empty()) {
        // Determine what stage these players are for
        TournamentStage nextStage = getNextStage(currentStage);
        
        if (currentStage == TournamentStage::Completed) {
            std::cout << "\nTOURNAMENT CHAMPION\n";
            Player champion = advancedPlayers[0];
            std::cout << "Winner: " << champion.name << " (ID: " << champion.id 
                      << ", Points: " << champion.points << ")\n";
            if (champion.isWildcard) std::cout << "  Status: Wildcard Player\n";
            if (champion.isEarlyBird) std::cout << "  Status: Early Bird Player\n";
        } else {
            // Check if we have matches for the current stage that need to be simulated
            DynamicArray<Match> currentStageMatches = getMatchesByStage(currentStage);
            
            if (!currentStageMatches.empty() && currentStage != TournamentStage::Qualifiers) {
                // We have matches for current stage - show the winners who advanced
                std::cout << "\n" << MatchScheduler::stageToString(currentStage) 
                          << " matches completed. Winners advancing to " 
                          << MatchScheduler::stageToString(nextStage) << ": " 
                          << advancedPlayers.getSize() << "\n";
                for (int i = 0; i < advancedPlayers.getSize(); ++i) {
                    std::cout << "  " << (i + 1) << ". " << advancedPlayers[i].name 
                              << " (ID: " << advancedPlayers[i].id 
                              << ", Points: " << advancedPlayers[i].points;
                    if (advancedPlayers[i].isWildcard) std::cout << ", Wildcard";
                    if (advancedPlayers[i].isEarlyBird) std::cout << ", Early Bird";
                    std::cout << ")\n";
                }
                std::cout << "Use 'Advance to Next Stage' to proceed to " 
                          << MatchScheduler::stageToString(nextStage) << ".\n";
            } else {
                // No current stage matches or already completed - show qualified players for next stage
                std::cout << "\nPlayers qualified for " << MatchScheduler::stageToString(nextStage) 
                          << ": " << advancedPlayers.getSize() << "\n";
                for (int i = 0; i < advancedPlayers.getSize(); ++i) {
                    std::cout << "  " << (i + 1) << ". " << advancedPlayers[i].name 
                              << " (ID: " << advancedPlayers[i].id 
                              << ", Points: " << advancedPlayers[i].points;
                    if (advancedPlayers[i].isWildcard) std::cout << ", Wildcard";
                    if (advancedPlayers[i].isEarlyBird) std::cout << ", Early Bird";
                    std::cout << ")\n";
                }
            }
        }
    }
}

void Task1Manager::displayStageResults(TournamentStage stage) const {
    std::cout << "\n=== " << MatchScheduler::stageToString(stage) << " RESULTS ===\n";
    
    DynamicArray<Match> stageMatches = getMatchesByStage(stage);
    
    if (stageMatches.empty()) {
        std::cout << "No matches found for " << MatchScheduler::stageToString(stage) << "\n";
        return;
    }
    
    // Load results from file
    DoublyLinkedList<Result> results;
    try {
        results = JsonLoader::loadResults("data/results.json");
    } catch (const std::exception& e) {
        std::cout << "Warning: Could not load match results: " << e.what() << "\n";
        std::cout << "Displaying matches without results:\n";
        for (int i = 0; i < stageMatches.getSize(); ++i) {
            const Match& match = stageMatches[i];
            std::cout << "Match " << match.id << ": Player " << match.player1 
                      << " vs Player " << match.player2 << " (Result not available)\n";
        }
        return;
    }
    
    // Load players to get player names
    DynamicArray<Player> allPlayers;
    try {
        // Create a temporary MatchScheduler to load players
        MatchScheduler tempScheduler;
        allPlayers = tempScheduler.loadPlayersFromFile();
    } catch (const std::exception& e) {
        std::cout << "Warning: Could not load player data: " << e.what() << "\n";
    }
    
    // Display each match with its result
    for (int i = 0; i < stageMatches.getSize(); ++i) {
        const Match& match = stageMatches[i];
        
        // Find the corresponding result
        Result* matchResult = nullptr;
        for (int j = 0; j < results.getSize(); ++j) {
            Result* result = results.get(j);
            if (result && result->matchId == match.id) {
                matchResult = result;
                break;
            }
        }
        
        // Find player names
        std::string player1Name = match.player1;
        std::string player2Name = match.player2;
        std::string winnerName = "Unknown";
        
        for (int k = 0; k < allPlayers.getSize(); ++k) {
            if (allPlayers[k].id == match.player1) {
                player1Name = allPlayers[k].name;
            }
            if (allPlayers[k].id == match.player2) {
                player2Name = allPlayers[k].name;
            }
            if (matchResult && allPlayers[k].id == matchResult->winnerId) {
                winnerName = allPlayers[k].name;
            }
        }
        
        // Display match result
        if (matchResult) {
            std::cout << "Match " << match.id << ": " << player1Name << " (" 
                      << JsonWriter::championToString(matchResult->championsP1) << ") vs " 
                      << player2Name << " (" 
                      << JsonWriter::championToString(matchResult->championsP2) << ")\n";
            std::cout << "         Winner: " << winnerName << " (Result ID: " << matchResult->id << ")\n";
        } else {
            std::cout << "Match " << match.id << ": " << player1Name 
                      << " vs " << player2Name << " (Result not found)\n";
        }
    }
    
    std::cout << "Total matches in " << MatchScheduler::stageToString(stage) << ": " << stageMatches.getSize() << "\n";
}

void Task1Manager::displayPlayerProgression() const {
    std::cout << "\n=== PLAYER PROGRESSION SUMMARY ===\n";
    
    std::cout << "Current Tournament ID: " << currentTournamentId << "\n";
    std::cout << "Total Matches Scheduled: " << allMatches.getSize() << "\n";
    std::cout << "Players Currently Advanced: " << advancedPlayers.getSize() << "\n";
    
    if (!advancedPlayers.empty()) {
        std::cout << "\nAdvanced Players:\n";
        for (int i = 0; i < advancedPlayers.getSize(); ++i) {
            const Player& player = advancedPlayers[i];
            std::cout << std::setw(3) << (i + 1) << ". " << player.name 
                      << " (ID: " << player.id << ", Points: " << player.points;
            
            if (player.isWildcard) std::cout << ", Wildcard";
            if (player.isEarlyBird) std::cout << ", Early Bird";
            
            std::cout << ")\n";
        }
    }
    
    // Show stage progression
    DynamicArray<TournamentStage> stages;
    stages.push_back(TournamentStage::Qualifiers);
    stages.push_back(TournamentStage::Tiebreakers);
    stages.push_back(TournamentStage::Quarterfinals);
    stages.push_back(TournamentStage::Semifinals);
    stages.push_back(TournamentStage::Finals);
    
    std::cout << "\nStage Progression:\n";
    for (int i = 0; i < stages.getSize(); ++i) {
        DynamicArray<Match> stageMatches = getMatchesByStage(stages[i]);
        std::cout << "  " << MatchScheduler::stageToString(stages[i]) 
                  << ": " << stageMatches.getSize() << " matches\n";
    }
}

void Task1Manager::displayTournamentSummary() const {
    std::cout << "\n=== TOURNAMENT SUMMARY ===\n";
    
    Tournament* activeTournament = tournamentManager.getRegisteringTournament();
    if (activeTournament) {
        std::cout << "Tournament: " << activeTournament->name << "\n";
        std::cout << "Stage: " << MatchScheduler::stageToString(activeTournament->stage) << "\n";
        std::cout << "Location: " << activeTournament->location << "\n\n";
    }
    
    displayPlayerProgression();
    std::cout << "\n" << std::string(56, '=') << "\n";
}

void Task1Manager::saveMatchesToFile(const DynamicArray<Match>& matches, const std::string& filename) {
    try {
        // Convert DynamicArray to DoublyLinkedList for JsonWriter
        DoublyLinkedList<Match> matchList;
        for (int i = 0; i < matches.getSize(); ++i) {
            matchList.append(matches[i]);
        }
        
        JsonWriter::writeAllMatches(matchList, filename);
        std::cout << "Saved " << matches.getSize() << " matches to " << filename << "\n";
    } catch (const std::exception& e) {
        std::cout << "Error saving matches: " << e.what() << "\n";
    }
}

DynamicArray<Match> Task1Manager::loadMatchesFromFile(const std::string& filename) {
    try {
        auto matchesList = JsonLoader::loadMatches(filename);
        DynamicArray<Match> matches;
        
        for (int i = 0; i < matchesList.getSize(); ++i) {
            Match* match = matchesList.get(i);
            if (match) {
                matches.push_back(*match);
            }
        }
        
        if (!matches.empty()) {
            std::cout << "Loaded " << matches.getSize() << " matches from " << filename << "\n";
        }
        
        return matches;
    } catch (const std::exception& e) {
        std::cout << "Could not load matches from " << filename << ": " << e.what() << "\n";
        return DynamicArray<Match>();
    }
}

void Task1Manager::displayMenu() const {
    std::cout << "\n=== MATCH SCHEDULING & PLAYER PROGRESSION MENU ===\n";
    std::cout << "1. Start Match Scheduling\n";
    std::cout << "2. Display Current Tournament Bracket\n";
    std::cout << "3. Advance to Next Stage\n";
    std::cout << "4. Display Stage Results\n";
    std::cout << "5. Display Tournament Summary\n";
    std::cout << "6. Clear All Matches\n";
    std::cout << "0. Back to Main Menu\n";
    std::cout << "Select an option: ";
}

int Task1Manager::handleMenuChoice() {
    int choice;
    while (!(std::cin >> choice)) {
        std::cin.clear(); // Clear error flag
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
        std::cout << "Invalid input. Please enter a number: ";
    }
    return choice;
}

void Task1Manager::runMatchSchedulingSystem() {
    int choice;
    do {
        displayMenu();
        choice = handleMenuChoice();
        
        switch (choice) {
            case 1: {
                std::cout << "Starting match scheduling...\n";
                startMatchScheduling();
                break;
            }
            case 2:
                displayCurrentBracket();
                break;
            case 3: {
                if (allMatches.empty()) {
                    std::cout << "No matches available. Please schedule matches first.\n";
                    break;
                }
                
                // Automatic stage advancement - determine current stage and advance
                std::cout << "Checking for automatic stage advancement...\n";
                TournamentStage currentStage = determineCurrentStage();
                
                if (currentStage == TournamentStage::Completed) {
                    std::cout << "Tournament is already completed!\n";
                    if (!advancedPlayers.empty()) {
                        Player champion = advancedPlayers[0];
                        std::cout << "Champion: " << champion.name << " (ID: " << champion.id << ")\n";
                        std::cout << "Points: " << champion.points;
                        if (champion.isWildcard) std::cout << " (Wildcard Player)";
                        if (champion.isEarlyBird) std::cout << " (Early Bird)";
                        std::cout << "\nCongratulations to the champion!\n";
                    }
                    break;
                }
                
                TournamentStage nextStage = getNextStage(currentStage);
                if (nextStage == currentStage && currentStage != TournamentStage::Finals) {
                    std::cout << "Cannot advance further from " << MatchScheduler::stageToString(currentStage) << "\n";
                    break;
                }
                
                std::cout << "Current stage: " << MatchScheduler::stageToString(currentStage) << "\n";
                
                // Special handling for finals
                if (currentStage == TournamentStage::Finals) {
                    std::cout << "Determining tournament winner from finals...\n";
                    bool advancementSuccess = automaticAdvanceToNextStage(currentStage, TournamentStage::Completed);
                    
                    if (advancementSuccess) {
                        updateTournamentStage(TournamentStage::Completed);
                        std::cout << "Tournament completed successfully!\n";
                    } else {
                        std::cout << "Failed to complete tournament.\n";
                    }
                } else {
                    std::cout << "Advancing to: " << MatchScheduler::stageToString(nextStage) << "\n";
                    bool advancementSuccess = automaticAdvanceToNextStage(currentStage, nextStage);
                    
                    if (advancementSuccess) {
                        updateTournamentStage(nextStage);
                        std::cout << "Successfully advanced tournament to " << MatchScheduler::stageToString(nextStage) << "!\n";
                    } else {
                        std::cout << "Failed to advance tournament stage.\n";
                    }
                }
                break;
            }
            case 4: {
                std::cout << "Select stage to view:\n";
                std::cout << "1. Qualifiers\n2. Quarterfinals\n3. Semifinals\n4. Finals\n";
                int stageChoice = handleMenuChoice();
                
                TournamentStage stage;
                switch (stageChoice) {
                    case 1: stage = TournamentStage::Qualifiers; break;
                    case 2: stage = TournamentStage::Quarterfinals; break;
                    case 3: stage = TournamentStage::Semifinals; break;
                    case 4: stage = TournamentStage::Finals; break;
                    default:
                        std::cout << "Invalid stage selection.\n";
                        continue;
                }
                displayStageResults(stage);
                break;
            }
            case 5:
                displayPlayerProgression();
                break;
            case 6:
                clearAllMatches();
                std::cout << "All matches cleared.\n";
                break;
            case 0:
                std::cout << "Returning to main menu...\n";
                break;
            default:
                std::cout << "Invalid option. Please try again.\n";
                break;
        }
        
        if (choice != 0) {
            std::cout << "\nPress Enter to continue...";
            std::cin.ignore();
            std::cin.get();
        }
        
    } while (choice != 0);
}

void Task1Manager::clearAllMatches() {
    allMatches.clear();
    advancedPlayers.clear();
    
    // Also clear the JSON files
    try {
        DoublyLinkedList<Match> emptyMatches;
        JsonWriter::writeAllMatches(emptyMatches, "data/matches.json");
        std::cout << "Cleared matches.json file.\n";
    } catch (const std::exception& e) {
        std::cout << "Warning: Could not clear matches.json: " << e.what() << "\n";
    }
    
    try {
        DoublyLinkedList<Result> emptyResults;
        JsonWriter::writeAllResults(emptyResults, "data/results.json");
        std::cout << "Cleared results.json file.\n";
    } catch (const std::exception& e) {
        std::cout << "Warning: Could not clear results.json: " << e.what() << "\n";
    }
}

void Task1Manager::clearMemoryOnly() {
    allMatches.clear();
    advancedPlayers.clear();
}

DynamicArray<Match> Task1Manager::getMatchesByStage(TournamentStage stage) const {
    DynamicArray<Match> stageMatches;
    for (int i = 0; i < allMatches.getSize(); ++i) {
        if (allMatches[i].stage == stage) {
            stageMatches.push_back(allMatches[i]);
        }
    }
    return stageMatches;
}

int Task1Manager::getTotalMatchesCount() const {
    return allMatches.getSize();
}

bool Task1Manager::hasScheduledMatches() const {
    return !allMatches.empty();
}

void Task1Manager::saveResultToFile(const Result& result, const std::string& filename) {
    try {
        // Load existing results
        DoublyLinkedList<Result> resultsList;
        try {
            resultsList = JsonLoader::loadResults(filename);
        } catch (const std::exception&) {
            // File might not exist yet, that's okay
        }
        
        // Add new result
        resultsList.append(result);
        
        // Save updated results
        JsonWriter::writeAllResults(resultsList, filename);
        std::cout << "Saved match result " << result.id << " to " << filename << "\n";
    } catch (const std::exception& e) {
        std::cout << "Error saving result: " << e.what() << "\n";
    }
}

Champion Task1Manager::getRandomChampion(std::mt19937& gen) const {
    // All available champions except NoChampion
    DynamicArray<Champion> champions;
    champions.push_back(Champion::Akali);
    champions.push_back(Champion::Ezreal);
    champions.push_back(Champion::Fiora);
    champions.push_back(Champion::Fizz);
    champions.push_back(Champion::Garen);
    champions.push_back(Champion::Jax);
    champions.push_back(Champion::Katarina);
    champions.push_back(Champion::LeeSin);
    champions.push_back(Champion::Riven);
    champions.push_back(Champion::Yasuo);
    champions.push_back(Champion::Zed);
    
    std::uniform_int_distribution<> dis(0, champions.getSize() - 1);
    return champions[dis(gen)];
}

std::string Task1Manager::generateUniqueResultId() const {
    // Load existing results to get the next available ID
    try {
        DoublyLinkedList<Result> existingResults = JsonLoader::loadResults("data/results.json");
        int nextId = existingResults.getSize() + 1;
        std::cout << "Generating unique result ID starting from: " << nextId << "\n";
        // Ensure uniqueness by checking if ID already exists
        bool idExists = true;
        while (idExists) {
            idExists = false;
            // Format as R00000
            char candidateId[7];
            std::sprintf(candidateId, "R%05d", nextId);
            std::string candidateIdStr(candidateId);
            
            for (int i = 0; i < existingResults.getSize(); ++i) {
                Result* result = existingResults.get(i);
                if (result && result->id == candidateIdStr) {
                    idExists = true;
                    nextId++;
                    break;
                }
            }
        }
        
        // Format final ID as R00000
        char finalId[7];
        std::sprintf(finalId, "R%05d", nextId);
        return std::string(finalId);
    } catch (const std::exception&) {
        // If file doesn't exist or can't be loaded, start with ID "R00001"
        return "R00001";
    }
}

DynamicArray<Player> Task1Manager::simulateGroupStageAndGetWinners(const DynamicArray<Match>& matches, const DynamicArray<Player>& players) {
    std::cout << "\n--- SIMULATING GROUP STAGE MATCHES ---\n";
    
    // First simulate all group matches
    simulateMatchResults(matches, players);
    
    // Now we need to determine the 6 group winners
    // For this implementation, we'll use a simplified approach:
    // Divide players into 6 groups and select the highest-ranked player from each group
    
    DynamicArray<Player> groupWinners;
    
    // Sort players by points (highest first)
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
    
    // Select winners from each group (simplified: take every 5th player starting from the top)
    int playersPerGroup = sortedPlayers.getSize() / 6;
    if (playersPerGroup == 0) playersPerGroup = 1;
    
    for (int group = 0; group < 6 && group * playersPerGroup < sortedPlayers.getSize(); ++group) {
        int winnerIndex = group * playersPerGroup;
        if (winnerIndex < sortedPlayers.getSize()) {
            groupWinners.push_back(sortedPlayers[winnerIndex]);
            std::cout << "Group " << (group + 1) << " winner: " << sortedPlayers[winnerIndex].name 
                      << " (Points: " << sortedPlayers[winnerIndex].points << ")\n";
        }
    }
    
    std::cout << "Group stage complete. " << groupWinners.getSize() << " group winners advance.\n";
    return groupWinners;
}

// Automatic stage advancement functions
TournamentStage Task1Manager::determineCurrentStage() const {
    if (allMatches.empty()) {
        return TournamentStage::Registration;
    }
    
    // Check tournament object for completed status
    Tournament* activeTournament = tournamentManager.getOngoingTournament();
    if (activeTournament && activeTournament->stage == TournamentStage::Completed) {
        return TournamentStage::Completed;
    }
    
    // Check what stages have matches scheduled
    bool hasQualifiers = false;
    bool hasQuarterfinals = false;
    bool hasSemifinals = false;
    bool hasFinals = false;
    
    for (int i = 0; i < allMatches.getSize(); ++i) {
        switch (allMatches[i].stage) {
            case TournamentStage::Qualifiers:
                hasQualifiers = true;
                break;
            case TournamentStage::Quarterfinals:
                hasQuarterfinals = true;
                break;
            case TournamentStage::Semifinals:
                hasSemifinals = true;
                break;
            case TournamentStage::Finals:
                hasFinals = true;
                break;
            default:
                break;
        }
    }
    
    // Determine current stage based on what's scheduled and completed
    if (hasFinals) {
        return TournamentStage::Finals;
    } else if (hasSemifinals) {
        return TournamentStage::Semifinals;
    } else if (hasQuarterfinals) {
        return TournamentStage::Quarterfinals;
    } else if (hasQualifiers) {
        return TournamentStage::Qualifiers;
    } else {
        return TournamentStage::Registration;
    }
}

bool Task1Manager::automaticAdvanceToNextStage(TournamentStage currentStage, TournamentStage nextStage) {
    std::cout << "\n=== AUTOMATIC STAGE ADVANCEMENT ===\n";
    
    // Special handling for Finals - determine winner and complete tournament
    if (currentStage == TournamentStage::Finals) {
        std::cout << "Finals stage detected - determining tournament winner...\n";
        
        DynamicArray<Match> finalMatches = getMatchesByStage(TournamentStage::Finals);
        if (finalMatches.empty()) {
            std::cout << "No final match found!\n";
            return false;
        }
        
        // Check if finals have already been simulated (advancedPlayers should contain the winner)
        if (advancedPlayers.getSize() == 1) {
            // Finals already simulated, just announce the winner
            Player champion = advancedPlayers[0];
            std::cout << "\nTOURNAMENT CHAMPION\n";
            std::cout << "Winner: " << champion.name << " (ID: " << champion.id << ")\n";
            std::cout << "Points: " << champion.points;
            if (champion.isWildcard) std::cout << " (Wildcard Player)";
            if (champion.isEarlyBird) std::cout << " (Early Bird)";
            std::cout << "\n";
            std::cout << "Congratulations to the new champion!\n";
            return true;
        } else {
            // Simulate the final match to determine the winner
            DynamicArray<Player> finalWinners = simulateMatchResults(finalMatches, advancedPlayers);
            
            if (finalWinners.empty()) {
                std::cout << "Failed to determine tournament winner!\n";
                return false;
            }
            
            // Announce the tournament winner
            Player champion = finalWinners[0];
            std::cout << "\nTOURNAMENT CHAMPION\n";
            std::cout << "Winner: " << champion.name << " (ID: " << champion.id << ")\n";
            std::cout << "Points: " << champion.points;
            if (champion.isWildcard) std::cout << " (Wildcard Player)";
            if (champion.isEarlyBird) std::cout << " (Early Bird)";
            std::cout << "\n";
            std::cout << "Congratulations to the new champion!\n";

            // Update advanced players to show only the winner
            advancedPlayers.clear();
            advancedPlayers.push_back(champion);
            
            return true;
        }
    }
    
    // For other stages, we need to schedule the next stage matches
    // The current stage matches should already be simulated, so advancedPlayers contains the winners
    
    if (advancedPlayers.empty()) {
        std::cout << "No advanced players found to proceed to " << MatchScheduler::stageToString(nextStage) << "\n";
        return false;
    }
    
    // Validate number of winners for next stage
    int requiredPlayers = getRequiredPlayersForStage(nextStage);
    if (advancedPlayers.getSize() < requiredPlayers) {
        std::cout << "Not enough players for " << MatchScheduler::stageToString(nextStage) 
                  << ". Required: " << requiredPlayers << ", Got: " << advancedPlayers.getSize() << "\n";
        return false;
    }
    
    std::cout << "Advancing " << advancedPlayers.getSize() << " players from " 
              << MatchScheduler::stageToString(currentStage) << " to " 
              << MatchScheduler::stageToString(nextStage) << "\n";
    
    // Schedule next stage matches (this will also auto-simulate them)
    DynamicArray<Match> nextStageMatches = scheduleNextStage(nextStage, advancedPlayers);
    
    if (nextStageMatches.empty()) {
        std::cout << "Failed to schedule " << MatchScheduler::stageToString(nextStage) << " matches\n";
        return false;
    }
    
    std::cout << "Successfully scheduled and simulated " << nextStageMatches.getSize() 
              << " matches for " << MatchScheduler::stageToString(nextStage) << "\n";
    std::cout << advancedPlayers.getSize() << " players advanced from " 
              << MatchScheduler::stageToString(nextStage) << "\n";
    
    return true;
}

void Task1Manager::updateTournamentStage(TournamentStage newStage) {
    Tournament* activeTournament = tournamentManager.getOngoingTournament();
    if (activeTournament) {
        activeTournament->stage = newStage;
        std::cout << "Updated tournament '" << activeTournament->name 
                  << "' stage to " << MatchScheduler::stageToString(newStage) << "\n";
        
        // Save the updated tournament (this would typically involve calling a save method)
        // For now, just log the update
        std::cout << "Tournament stage updated in memory. Consider saving to persistence.\n";
    } else {
        std::cout << "Warning: No active tournament found to update stage.\n";
    }
}

bool Task1Manager::isStageCompleted(TournamentStage stage) const {
    // This would check if all matches in a stage have been completed
    // For now, we'll implement a simplified version
    DynamicArray<Match> stageMatches = getMatchesByStage(stage);
    
    if (stageMatches.empty()) {
        return false; // No matches means stage not started
    }
    
    // In a complete implementation, this would check match results
    // For now, assume stage is completed if matches exist
    // (since we simulate all matches immediately)
    return true;
}
