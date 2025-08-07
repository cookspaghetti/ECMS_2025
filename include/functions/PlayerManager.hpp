#ifndef PLAYER_MANAGER_HPP
#define PLAYER_MANAGER_HPP

#include "dto/Player.hpp"
#include "structures/DoublyLinkedList.hpp"
#include <string>

class PlayerManager {
private:
    DoublyLinkedList<Player> allPlayers;
    bool dataLoaded;
    const std::string filename = "data/players.json";

public:
    PlayerManager();
    
    // Load players from JSON file
    bool loadPlayersFromFile();
    
    // Search for player by ID
    Player* findPlayerById(const std::string& playerId);
    
    // Check if player exists
    bool playerExists(const std::string& playerId);
    
    // Get all loaded players
    const DoublyLinkedList<Player>& getAllPlayers() const;
    
    // Display all loaded players
    void displayAllPlayers() const;

    // Add a new player to the database
    bool addPlayer(const Player& player);

    // Delete a player by ID
    void deletePlayer(const std::string& playerId);

    // Reload data from file
    void reloadData();
};

#endif
