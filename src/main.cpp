#include <iostream>
#include <limits>
#include "functions/PlayerRegistration.hpp"
#include "functions/SpectatorRegistration.hpp"
#include "functions/GameResultLogger.hpp"

PlayerRegistration playerReg;
SpectatorRegistration spectatorReg; 
GameResultLogger logger; // Global logger instance 


void handleMatchScheduling();
void handleRegistration();
void handleSpectatorQueue();
void handleResultLogging();

int main() {
    int choice;

    do {
        std::cout << "\n=== APUEC Management System ===\n";
        std::cout << "1. Match Scheduling & Player Progression\n";
        std::cout << "2. Tournament Registration & Player Queueing\n";
        std::cout << "3. Live Stream & Spectator Queue Management\n";
        std::cout << "4. Game Result Logging & Performance History\n";
        std::cout << "0. Exit\n";
        std::cout << "Select an option: ";
        std::cin >> choice;

        switch (choice) {
            case 1:
                handleMatchScheduling();
                break;
            case 2:
                handleRegistration();
                break;
            case 3:
                handleSpectatorQueue();
                break;
            case 4:
                handleResultLogging();
                break;
            case 0:
                std::cout << "Exiting... Goodbye!\n";
                break;
            default:
                std::cout << "Invalid option. Try again.\n";
        }

    } while (choice != 0);

    return 0;
}

void handleMatchScheduling() {
    int choice;
    do {
        std::cout << "\n-- Match Scheduling Menu --\n";
        std::cout << "1. Show Current Tournaments\n";
        std::cout << "2. Check Tournament Info\n";
        std::cout << "3. Search Player\n";
        std::cout << "0. Back to Main Menu\n";
        std::cout << "Select an option: ";
        std::cin >> choice;

        switch (choice) {
            case 1:
                std::cout << "Showing current tournaments...\n";
                break;
            case 2:
                std::cout << "Checking tournament info...\n";
                break;
            case 3:
                std::cout << "Searching player...\n";
                break;
            case 0:
                break;
            default:
                std::cout << "Invalid option. Try again.\n";
        }
    } while (choice != 0);
}

void handleRegistration() {
    int choice;
    do {
        std::cout << "\n-- Tournament Registration Menu --\n";
        std::cout << "1. Register Player\n";
        std::cout << "2. Check-In Player\n";
        std::cout << "3. Show Registered Queue\n";
        std::cout << "0. Back to Main Menu\n";
        std::cout << "Select an option: ";
        std::cin >> choice;

        switch (choice) {
            case 1:
                playerReg.registerPlayer();
                break;
            case 2:
                playerReg.checkInPlayer();
                break;
            case 3:
                playerReg.displayQueue();
                break;
            case 0:
                break;
            default:
                std::cout << "Invalid option. Try again.\n";
        }
    } while (choice != 0);
}

void handleSpectatorQueue() {
    int choice;
    do {
        std::cout << "\n-- Spectator Management Menu --\n";
        std::cout << "1. Register Spectator\n";
        std::cout << "2. Display Queue\n";
        std::cout << "0. Back to Main Menu\n";
        std::cout << "Select an option: ";
        
        if (!(std::cin >> choice)) {
            // Input failed, clear error state and ignore bad input
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            choice = -1; // Set to invalid choice to trigger default case
        }

        switch (choice) {
            case 1:
                spectatorReg.registerSpectator();
                break;
            case 2:
                std::cout << "Displaying spectator queue and seating status...\n";
                spectatorReg.displayQueue();
                break;
            case 0:
                break;
            default:
                std::cout << "Invalid option. Try again.\n";
        }
    } while (choice != 0);
}

void handleResultLogging() {
    int choice;
    
    do {
        std::cout << "\n-- Game Result Menu --\n";
        std::cout << "1. Display All Loaded Results\n";
        std::cout << "2. Search for Player\n";
        std::cout << "3. Search for Tournament\n";
        std::cout << "4. Search Champion Statistics\n";
        std::cout << "0. Back to Main Menu\n";
        std::cout << "Select an option: ";
        
        if (!(std::cin >> choice)) {
            // Input failed, clear error state and ignore bad input
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            choice = -1; // Set to invalid choice to trigger default case
        }

        switch (choice) {
            case 1:
                std::cout << "\n=== ALL LOADED GAME RESULTS ===\n";
                logger.displayLoadedResults();
                break;
            case 2: {
                std::cout << "=== PLAYER SEARCH ===\n";
                std::cout << "Enter Player ID to search: ";
                std::string input;
                std::cin >> input;
                int playerId;
                try {
                    playerId = std::stoi(input);
                } catch (...) {
                    std::cout << "Invalid Player ID entered.\n";
                    break;
                }
                
                // Search for specific player matches (data already loaded)
                logger.searchMatchesByPlayer(playerId);
                
                // Also display player's win rate and statistics
                std::cout << "\n--- PLAYER STATISTICS ---\n";
                logger.displayPlayerPerformance(playerId);
                break;
            }
            case 3: {
                std::cout << "=== TOURNAMENT SEARCH ===\n";
                std::cout << "Enter Tournament/Match ID to search: ";
                std::string input;
                std::cin >> input;
                int tournamentId;
                try {
                    tournamentId = std::stoi(input);
                } catch (...) {
                    std::cout << "Invalid Tournament/Match ID entered.\n";
                    break;
                }
                
                // Search for specific tournament/match (data already loaded)
                logger.searchMatchesByMatchId(tournamentId);
                break;
            }
            case 4: {
                std::cout << "=== CHAMPION STATISTICS ===\n";
                std::cout << "Enter Champion name to search: ";
                std::string championName;
                std::cin >> championName;
                
                // Show champion-specific statistics (data already loaded)
                std::cout << "\n=== STATISTICS FOR CHAMPION: " << championName << " ===\n";
                std::cout << "Searching for matches where " << championName << " was played...\n\n";
                
                // Show favorite champions to see if the searched champion appears
                logger.displayPlayerFavoriteChampions();
                std::cout << "\nNote: The table above shows each player's most-used champion.\n";
                std::cout << "If " << championName << " appears as someone's favorite, they use it frequently.\n";
                break;
            }
            case 0:
                break;
            default:
                std::cout << "Invalid option. Try again.\n";
        }
        
    } while (choice != 0);
}
