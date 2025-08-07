#include "manager/PlayerManager.hpp"
#include "helper/JsonLoader.hpp"
#include "helper/JsonWriter.hpp"
#include <iostream>

PlayerManager::PlayerManager() : dataLoaded(false) {}

bool PlayerManager::PlayerManager::loadPlayersFromFile() {
    try {
        allPlayers = JsonLoader::loadPlayers(filename);
        dataLoaded = true;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading players from file: " << e.what() << std::endl;
        dataLoaded = false;
        return false;
    }
}

Player* PlayerManager::findPlayerById(const std::string& playerId) {
    if (!dataLoaded) {
        if (!loadPlayersFromFile()) {
            std::cout << "Failed to load player data. Cannot search for player." << std::endl;
            return nullptr;
        }
    }
    
    // Search through all loaded players
    for (int i = 0; i < allPlayers.getSize(); i++) {
        Player* currentPlayer = allPlayers.get(i);
        if (currentPlayer && currentPlayer->id == playerId) {
            std::cout << "Player found in database: " << currentPlayer->name << " (ID: " << playerId << ")" << std::endl;
            return currentPlayer;
        }
    }
    
    std::cout << "Player with ID " << playerId << " not found in database." << std::endl;
    return nullptr;
}

bool PlayerManager::playerExists(const std::string& playerId) {
    return findPlayerById(playerId) != nullptr;
}

const DoublyLinkedList<Player>& PlayerManager::getAllPlayers() const {
    return allPlayers;
}

void PlayerManager::displayAllPlayers() const {
    if (!dataLoaded || allPlayers.getSize() == 0) {
        std::cout << "No players loaded or database is empty." << std::endl;
        return;
    }
    
    std::cout << "\n=== ALL PLAYERS IN DATABASE ===" << std::endl;
    for (int i = 0; i < allPlayers.getSize(); i++) {
        Player* player = allPlayers.get(i);
        if (player) {
            std::cout << "ID: " << player->id << ", Name: " << player->name 
                      << ", Email: " << player->email << ", Date Joined: " << player->dateJoined << std::endl;
        }
    }
    std::cout << "Total players in database: " << allPlayers.getSize() << std::endl;
}

bool PlayerManager::addPlayer(const Player& player) {
    if (!dataLoaded) {
        if (!loadPlayersFromFile()) {
            std::cout << "Failed to load player data. Cannot add player." << std::endl;
            return false;
        }
    }

    // Check if player already exists
    if (playerExists(player.id)) {
        std::cout << "Player with ID " << player.id << " already exists." << std::endl;
        return false;
    }

    allPlayers.append(player);

    // Save changes to file
    if (!JsonWriter::appendPlayer(player, filename)) {
        std::cout << "Failed to save player to database." << std::endl;
        return false;
    }

    std::cout << "Player with ID " << player.id << " has been added." << std::endl;
    return true;
}

void PlayerManager::deletePlayer(const std::string& playerId) {
    if (!dataLoaded) {
        if (!loadPlayersFromFile()) {
            std::cout << "Failed to load player data. Cannot delete player." << std::endl;
            return;
        }
    }

    for (int i = 0; i < allPlayers.getSize(); i++) {
        Player* currentPlayer = allPlayers.get(i);
        if (currentPlayer && currentPlayer->id == playerId) {
            allPlayers.removeAt(i);
            std::cout << "Player with ID " << playerId << " has been deleted." << std::endl;
            return;
        }
    }

    // Save changes to file
    if (JsonWriter::writeAllPlayers(allPlayers, filename)) {
        std::cout << "Player database updated successfully." << std::endl;
    } else {
        std::cout << "Failed to update player database." << std::endl;
    }
    
    std::cout << "Player with ID " << playerId << " not found in database." << std::endl;
}

void PlayerManager::reloadData() {
    dataLoaded = false;
    allPlayers.clear();
    loadPlayersFromFile();
}
