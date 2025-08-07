#ifndef PLAYER_REGISTRATION_HPP
#define PLAYER_REGISTRATION_HPP

#include "dto/Player.hpp"
#include "structures/PriorityQueue.hpp"
#include "functions/PlayerManager.hpp"
#include "functions/TournamentManager.hpp"
#include "helper/JsonWriter.hpp"
#include "helper/JsonLoader.hpp"

class PlayerRegistration {
private:
    PriorityQueue<Player> checkInQueue;
    DoublyLinkedList<Player> registeredPlayers;
    PlayerManager playerManager;
    TournamentManager tournamentManager;
    int nextPlayerId;
    int registrationCount;
    int maxParticipants;

public:
    PlayerRegistration();

    void registerPlayer();
    Player handleExistingPlayer();
    Player handleNewPlayer();
    void unregisterPlayer();
    void withdrawPlayer(); // Withdraw player even after check-in with status inheritance
    void checkInPlayer();
    void checkInAllPlayers();
    void displayRegisteredPlayers();
    void displayCheckInQueue();
    int getTotalCheckedInPlayers() const;
    PriorityQueue<Player> getCheckInQueue() const;
    void displayAllPlayersInDatabase();
    void saveCurrentRegisteredPlayers();
    void saveCurrentCheckInQueue();
    void clearRegisteredPlayers();
    void clearCheckInQueue();
    void clearAll(); // Clear both registered players and check-in queue
    std::string getCurrentDate();
    bool isPlayerRegistered(const std::string& playerId) const;
    
private:
    void handleStatusInheritance(bool wasEarlyBird, bool wasRegular);
    void updatePlayerPriorityInQueue(const std::string& playerId, int newPriority);
};

#endif
