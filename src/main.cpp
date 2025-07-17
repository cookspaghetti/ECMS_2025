#include <iostream>
#include "functions/PlayerRegistration.hpp"

PlayerRegistration playerReg;

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
        std::cout << "2. Select Spectator Type\n";
        std::cout << "0. Back to Main Menu\n";
        std::cout << "Select an option: ";
        std::cin >> choice;

        switch (choice) {
            case 1:
                std::cout << "Registering spectator...\n";
                break;
            case 2:
                std::cout << "Selecting spectator type...\n";
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
        std::cout << "1. Record Game Result\n";
        std::cout << "2. Search for Player\n";
        std::cout << "3. Search for Tournament\n";
        std::cout << "4. Search Champion Statistics\n";
        std::cout << "0. Back to Main Menu\n";
        std::cout << "Select an option: ";
        std::cin >> choice;

        switch (choice) {
            case 1:
                std::cout << "Recording game result...\n";
                break;
            case 2:
                std::cout << "Searching player...\n";
                break;
            case 3:
                std::cout << "Searching tournament...\n";
                break;
            case 4:
                std::cout << "Searching champion stats...\n";
                break;
            case 0:
                break;
            default:
                std::cout << "Invalid option. Try again.\n";
        }
    } while (choice != 0);
}
