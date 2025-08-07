#ifndef TASK2_MANAGER_HPP
#define TASK2_MANAGER_HPP

#include "functions/TournamentManager.hpp"
#include "functions/PlayerRegistration.hpp"
#include "structures/PriorityQueue.hpp"
#include "dto/Player.hpp"

class Task2Manager {
private:
    TournamentManager& tournamentManager;
    PlayerRegistration& playerRegistration;

public:
    // Constructor
    Task2Manager(TournamentManager& tm, PlayerRegistration& pr);
    
    // Core menu functions
    void createNewTournament();
    void viewTournamentDetails();
    void registerPlayer();
    void unregisterPlayer();
    void displayRegisteredPlayers();
    void checkInPlayer();
    void withdrawPlayer();
    void displayCheckInQueue();
    PriorityQueue<Player> endCurrentTournamentRegistration();
    
    // Additional utility functions
    void displayMenu();
    int handleMenuChoice();
    void runTournamentRegistrationSystem();
};

#endif
