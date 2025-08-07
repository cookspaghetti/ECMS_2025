#include "manager/Task2Manager.hpp"
#include "manager/Task1Manager.hpp"
#include <iostream>
#include <limits>

Task2Manager::Task2Manager(TournamentManager& tm, PlayerRegistration& pr) 
    : tournamentManager(tm), playerRegistration(pr) {
}

void Task2Manager::createNewTournament() {
    tournamentManager.createTournament();
}

void Task2Manager::viewTournamentDetails() {
    tournamentManager.displayTournamentDetails();
}

void Task2Manager::registerPlayer() {
    playerRegistration.registerPlayer();
}

void Task2Manager::unregisterPlayer() {
    playerRegistration.unregisterPlayer();
}

void Task2Manager::displayRegisteredPlayers() {
    playerRegistration.displayRegisteredPlayers();
}

void Task2Manager::checkInPlayer() {
    playerRegistration.checkInPlayer();
}

void Task2Manager::withdrawPlayer() {
    playerRegistration.withdrawPlayer();
}

void Task2Manager::displayCheckInQueue() {
    playerRegistration.displayCheckInQueue();
}

PriorityQueue<Player> Task2Manager::endCurrentTournamentRegistration() {
    
    int totalCheckedInPlayers = playerRegistration.getTotalCheckedInPlayers();
    
    std::cout << "Total players checked in: " << totalCheckedInPlayers << std::endl;
    
    // End the tournament registration
    bool success = tournamentManager.endRegisteringTournament(totalCheckedInPlayers);

    if (!success) {
        std::cout << "Failed to end tournament registration.\n";
        return PriorityQueue<Player>(); // Return empty queue
    }

    PriorityQueue<Player> finalQueue = playerRegistration.getCheckInQueue();
    // Save the current queue to file
    if (!JsonWriter::writeAllCheckedInPlayer(finalQueue, "data/checked_in_players.json")) {
        std::cout << "Failed to save check-in queue to file.\n";
        return PriorityQueue<Player>(); // Return empty queue
    }
    
    // Return the check-in queue
    return finalQueue;
}

void Task2Manager::displayMenu() {
    std::cout << "\n=== TASK 2: TOURNAMENT REGISTRATION & PLAYER QUEUEING ===\n";
    std::cout << "1. Create New Tournament\n";
    std::cout << "2. View Tournament Details\n";
    std::cout << "3. Register Player\n";
    std::cout << "4. Unregister Player\n";
    std::cout << "5. Display Registered Players\n";
    std::cout << "6. Check-In Player\n";
    std::cout << "7. Withdraw Player (After Check-in)\n";
    std::cout << "8. Display Check-in Queue\n";
    std::cout << "9. End Current Tournament Registration\n";
    std::cout << "0. Back to Main Menu\n";
    std::cout << "Select an option: ";
}

int Task2Manager::handleMenuChoice() {
    int choice;
    while (!(std::cin >> choice)) {
        std::cin.clear(); // Clear error flag
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
        std::cout << "Invalid input. Please enter a number: ";
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    
    switch (choice) {
        case 1:
            createNewTournament();
            break;
        case 2:
            viewTournamentDetails();
            break;
        case 3:
            registerPlayer();
            break;
        case 4:
            unregisterPlayer();
            break;
        case 5:
            displayRegisteredPlayers();
            break;
        case 6:
            checkInPlayer();
            break;
        case 7:
            withdrawPlayer();
            break;
        case 8:
            displayCheckInQueue();
            break;
        case 9: {
            PriorityQueue<Player> finalQueue = endCurrentTournamentRegistration();
            if (finalQueue.isEmpty()) {
                std::cout << "Ending tournament registration failed. No players checked in.\n";
                break;
            }
            std::cout << "Tournament registration ended. Final queue contains " 
                      << finalQueue.getSize() << " players.\n";
            
            // Clear checkin queue after ending registration
            playerRegistration.clearCheckInQueue();
            std::cout << "Check-in queue cleared after ending registration.\n";
            break;
        }
        case 0:
            std::cout << "Returning to main menu...\n";
            break;
        default:
            std::cout << "Invalid option. Try again.\n";
    }
    
    return choice;
}

void Task2Manager::runTournamentRegistrationSystem() {    
    int choice;
    do {
        displayMenu();
        choice = handleMenuChoice();
    } while (choice != 0);
    
    std::cout << "Exiting...\n";
}
