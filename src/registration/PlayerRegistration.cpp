#include <iostream>
#include <string>
#include <ctime>
#include <filesystem>
#include <cmath>

#include "functions/PlayerRegistration.hpp"

PlayerRegistration::PlayerRegistration() : nextPlayerId(1), registrationCount(0), maxParticipants(0) {
    // Load existing players to determine next available ID
    if (playerManager.loadPlayersFromFile()) {
        const DoublyLinkedList<Player>& allPlayers = playerManager.getAllPlayers();
        int maxId = 0;
        
        // Find the highest existing ID number
        for (int i = 0; i < allPlayers.getSize(); i++) {
            Player* player = allPlayers.get(i);
            if (player && !player->id.empty() && player->id.length() >= 2) {
                // Extract numeric part from "P00000" format
                std::string numericPart = player->id.substr(1);
                try {
                    int id = std::stoi(numericPart);
                    if (id > maxId) {
                        maxId = id;
                    }
                } catch (const std::exception&) {
                    // Invalid ID format, skip
                }
            }
        }
        
        // Set next player ID to be one higher than the maximum existing ID
        nextPlayerId = maxId + 1;
        std::cout << "Player database loaded. Next player ID will be: " << nextPlayerId << std::endl;
    } else {
        std::cout << "Warning: Could not load player database. Starting with ID 1." << std::endl;
    }

    // Load existing registered players
    try{
        registeredPlayers = JsonLoader::loadPlayers("data/temp_registered_players.json");
        std::cout << "Loaded " << registeredPlayers.getSize() << " previously registered players." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error loading registered players: " << e.what() << std::endl;
        registeredPlayers.clear();
    }

    // Load existing check-in queue
    try {
        checkInQueue = JsonLoader::loadCheckedInPlayers("data/temp_checkin_queue.json");
        std::cout << "Loaded " << checkInQueue.getSize() << " players from temporary check-in queue." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error loading check-in queue: " << e.what() << std::endl;
        checkInQueue.clear();
    }

    // Get the maximum number of participants from the active tournament
    if (tournamentManager.hasRegisteringTournament()) {
        Tournament* registeringTournament = tournamentManager.getRegisteringTournament();
        if (registeringTournament) {
            maxParticipants = registeringTournament->maxParticipants;
            std::cout << "Max participants for current tournament: " << maxParticipants << std::endl;
        }
    } else {
        std::cout << "No active tournament found. Max participants set to 0." << std::endl;
        clearAll();
    }
    
}

void PlayerRegistration::registerPlayer() {
    int choice;
    std::cout << "Is the player new or existing? (1/2): ";

    std::cin >> choice;
    Player player;

    if (choice == 1) {
        player = handleNewPlayer();

        if (player.id.empty()) {
            std::cout << "No valid player created. Registration aborted." << std::endl;
            return;
        }
    } else if (choice == 2) {
        player = handleExistingPlayer();

        if (player.id.empty()) {
            std::cout << "No valid player found. Registration aborted." << std::endl;
            return;
        }

        // Check if player is already registered
        if (PlayerRegistration::isPlayerRegistered(player.id)) {
            std::cout << "Player is already registered." << std::endl;
            return; // Player already registered
        }
    } else {
        std::cout << "Invalid choice." << std::endl;
        return;
    }

    // Assign status based on registration order and type
    registrationCount++;

    // Calculate the number of players that will be late
    int lateThreshold = static_cast<int>(std::ceil(maxParticipants * 0.8)); // round up to the nearest integer

    // First 10 players get early bird status
    if (registeredPlayers.getSize() < 10) {
        player.isEarlyBird = true;
        std::cout << "Early bird status assigned!" << std::endl;
    }
    // Players after lateThreshold without wildcard get late status
    else if (registeredPlayers.getSize() > lateThreshold && !player.isWildcard) {
        player.isLate = true;
        std::cout << "Late registration status assigned." << std::endl;
    }
    
    registeredPlayers.append(player);
    std::cout << "Player registered successfully (Registration #" << registrationCount << ")" << std::endl;
    std::cout << "Status - Early Bird: " << (player.isEarlyBird ? "Yes" : "No") 
              << ", Wildcard: " << (player.isWildcard ? "Yes" : "No")
              << ", Late: " << (player.isLate ? "Yes" : "No") << std::endl;
    
    // Update tournament participant count
    if (tournamentManager.hasRegisteringTournament()) {
        tournamentManager.incrementParticipantCount();
    }
    
    // Save registered players immediately after successful registration
    saveCurrentRegisteredPlayers();
}

// Handle existing player registration
Player PlayerRegistration::handleExistingPlayer() {
    bool isWildcard = false;
    std::string id;
    int wildcardChoice;
    
    std::cout << "Enter existing player ID: ";
    std::cin >> id;

    std::cout << "Is this a wildcard entry? (1 for Yes, 0 for No): ";
    std::cin >> wildcardChoice;
    if (wildcardChoice == 1) {
        isWildcard = true;
    }
    
    // Use Player Manager to find player from database
    Player* found = playerManager.findPlayerById(id);

    if (found) {
        if (PlayerRegistration::isPlayerRegistered(found->id)) {
            return *found; // Return already registered player
        }

        // Create a copy of the found player
        Player existingPlayer = *found;
        
        // Update wildcard status for this tournament registration
        if (isWildcard) {
            existingPlayer.isWildcard = true;
            std::cout << "Wildcard entry confirmed for existing player.\n";
        } else {
            existingPlayer.isWildcard = false;
            std::cout << "Regular entry confirmed for existing player.\n";
        }
        
        std::cout << "Player found: " << existingPlayer.name << std::endl;
        std::cout << "Original registration date: " << existingPlayer.dateJoined << std::endl;
        
        return existingPlayer;
    } else {
        std::cout << "Player not found in database. Please register as new player." << std::endl;
        return Player(); // Return default player
    }
}

Player PlayerRegistration::handleNewPlayer() {
    std::cin.ignore();

    std::string name, email, phoneNum, genderStr;
    int age, points;
    Gender gender;

    std::cout << "Enter player name: ";
    std::getline(std::cin, name);
    std::cout << "Enter player age: ";
    std::cin >> age;
    std::cin.ignore();

    std::cout << "Enter gender (M/F): ";
    std::getline(std::cin, genderStr);
    if (genderStr == "M") gender = Gender::Male;
    else gender = Gender::Female;

    std::cout << "Enter email: ";
    std::getline(std::cin, email);
    std::cout << "Enter phone number: ";
    std::getline(std::cin, phoneNum);
    

    points = 0; // Default points for new players
    std::string dateJoined = getCurrentDate();

    // Generate unique player ID in P00000 format
    std::string counterStr = std::to_string(nextPlayerId);
    std::string id = "P" + std::string(5 - counterStr.length(), '0') + counterStr;
    nextPlayerId++;

    // Create player
    Player player = Player(id, name, age, gender, email, phoneNum, points, false, false, false, dateJoined);

    // Save new player to database
    if (JsonWriter::appendPlayer(player, "data/players.json")) {
        std::cout << "New player saved to database successfully!" << std::endl;
        // Reload player manager data to include the new player
        playerManager.reloadData();
    } else {
        std::cout << "Warning: Failed to save player to database." << std::endl;
    }

    std::cout << "New player registered with ID: " << id << std::endl;
    return player;
}

void PlayerRegistration::unregisterPlayer() {
    if (registeredPlayers.getSize() == 0) {
        std::cout << "No registered players to unregister.\n";
        return;
    }

    std::string playerId;
    std::cout << "Enter player ID to unregister: ";
    std::cin >> playerId;
    std::cin.ignore();
    
    
    bool found = false;
    // Search and remove from registered players list
    for (int i = 0; i < registeredPlayers.getSize(); i++) {
        Player* currentPlayer = registeredPlayers.get(i);
        if (currentPlayer && currentPlayer->id == playerId) {
            found = true;
            std::cout << "Player " << currentPlayer->name << " (ID: " << playerId << ") has been unregistered.\n";
            registeredPlayers.removeAt(i);
            registrationCount--;
            break;
        }
    }
    
    if (!found) {
        std::cout << "Player with ID " << playerId << " not found in the registered players list.\n";
    } else {
        // Update tournament participant count
        if (tournamentManager.hasRegisteringTournament()) {
            tournamentManager.decrementParticipantCount();
        }
        
        // Save registered players immediately after successful unregistration
        saveCurrentRegisteredPlayers();
    }
}

void PlayerRegistration::withdrawPlayer() {
    if (checkInQueue.getSize() == 0) {
        std::cout << "No registered players to withdraw.\n";
        return;
    }

    std::string playerId;
    std::cout << "Enter player ID to withdraw: ";
    std::cin >> playerId;
    std::cin.ignore();
    
    Player* withdrawingPlayer = nullptr;
    bool wasEarlyBird = false;
    bool wasWildcard = false;
    bool wasLate = false;
    bool wasRegular = false;
    
    // Find the withdrawing player in the check-in queue
    for (int i = 0; i < checkInQueue.getSize(); i++) {
        // We need to get the player from the queue - but PriorityQueue doesn't have direct access
        // So we'll check during the queue reconstruction process
    }
    
    // We'll find the player during the queue reconstruction
    bool playerFound = false;
    
    // Create a new check-in queue without the withdrawing player
    PriorityQueue<Player> newCheckInQueue;
    
    // Get all players from current check-in queue
    if (!checkInQueue.isEmpty()) {
        int queueSize = checkInQueue.getSize();
        Player* players = new Player[queueSize];
        int* priorities = new int[queueSize];
        int count = 0;
        
        checkInQueue.getAllItemsWithPriority(players, priorities, count);
        
        // Find the withdrawing player and get their details
        for (int i = 0; i < count; i++) {
            if (players[i].id == playerId) {
                withdrawingPlayer = &players[i];
                playerFound = true;
                // Store the status of the withdrawing player
                wasEarlyBird = withdrawingPlayer->isEarlyBird;
                wasWildcard = withdrawingPlayer->isWildcard;
                wasLate = withdrawingPlayer->isLate;
                wasRegular = (!wasEarlyBird && !wasWildcard && !wasLate);
                std::cout << "Withdrawing player: " << withdrawingPlayer->name << " (ID: " << playerId << ")\n";
            } else {
                // Re-add all other players to the new queue
                newCheckInQueue.enqueue(players[i], priorities[i]);
            }
        }
        
        // Clean up
        delete[] players;
        delete[] priorities;
        
        // Replace the old queue with the new one
        checkInQueue = newCheckInQueue;
    }
    
    if (!playerFound) {
        std::cout << "Player with ID " << playerId << " not found in the check-in queue.\n";
        return;
    }
    
    std::cout << "Player " << withdrawingPlayer->name << " has been withdrawn successfully.\n";
    
    // Update tournament participant count (since player is leaving the tournament)
    if (tournamentManager.hasRegisteringTournament()) {
        tournamentManager.decrementParticipantCount();
    }
    
    // Display status changes if any occurred
    if (wasEarlyBird || wasRegular) {
        std::cout << "Status inheritance has been processed. Check registered players for updates.\n";
    }
    
    // Save both registered players and check-in queue
    saveCurrentRegisteredPlayers();
    if (!checkInQueue.isEmpty()) {
        saveCurrentCheckInQueue();
    } else {
        // If queue is now empty, clear the file
        clearCheckInQueue();
    }
}

void PlayerRegistration::handleStatusInheritance(bool wasEarlyBird, bool wasRegular) {
    // Count current status holders
    int earlyBirdCount = 0;
    int regularCount = 0;
    
    for (int i = 0; i < registeredPlayers.getSize(); i++) {
        Player* player = registeredPlayers.get(i);
        if (player) {
            if (player->isEarlyBird) earlyBirdCount++;
            else if (!player->isWildcard && !player->isLate) regularCount++;
        }
    }
    
    // If an early bird withdrew and we have less than 10 early birds
    if (wasEarlyBird && earlyBirdCount < 10) {
        // Find the first regular player (chronologically) to promote
        for (int i = 0; i < registeredPlayers.getSize(); i++) {
            Player* player = registeredPlayers.get(i);
            if (player && !player->isEarlyBird && !player->isWildcard && !player->isLate) {
                player->isEarlyBird = true;
                std::cout << "Player " << player->name << " (ID: " << player->id << ") promoted to Early Bird status!\n";
                
                // Update their priority in check-in queue if they're checked in
                updatePlayerPriorityInQueue(player->id, 1);
                break;
            }
        }
    }
    
    // If a regular player withdrew and we have late players who could be promoted
    if (wasRegular) {
        // Find the first late player to promote to regular
        for (int i = 0; i < registeredPlayers.getSize(); i++) {
            Player* player = registeredPlayers.get(i);
            if (player && player->isLate && !player->isWildcard) {
                player->isLate = false; // Now becomes regular
                std::cout << "Player " << player->name << " (ID: " << player->id << ") promoted from Late to Regular status!\n";
                
                // Update their priority in check-in queue if they're checked in
                updatePlayerPriorityInQueue(player->id, 2);
                break;
            }
        }
    }
}

void PlayerRegistration::updatePlayerPriorityInQueue(const std::string& playerId, int newPriority) {
    // Create a new queue with updated priority
    PriorityQueue<Player> newQueue;
    
    if (!checkInQueue.isEmpty()) {
        int queueSize = checkInQueue.getSize();
        Player* players = new Player[queueSize];
        int* priorities = new int[queueSize];
        int count = 0;
        
        checkInQueue.getAllItemsWithPriority(players, priorities, count);
        
        // Re-add all players with updated priority for the specified player
        for (int i = 0; i < count; i++) {
            if (players[i].id == playerId) {
                newQueue.enqueue(players[i], newPriority);
                std::cout << "Updated priority for " << players[i].name << " to " << newPriority << " in check-in queue.\n";
            } else {
                newQueue.enqueue(players[i], priorities[i]);
            }
        }
        
        // Clean up
        delete[] players;
        delete[] priorities;
        
        // Replace the old queue
        checkInQueue = newQueue;
    }
}

void PlayerRegistration::checkInPlayer() {
    std::string playerId;
    std::cout << "Enter player ID to check-in: ";
    std::cin >> playerId;
    std::cin.ignore();

    if (registeredPlayers.getSize() == 0) {
        std::cout << "No registered players available for check-in.\n";
        return;
    }
    
    Player* found = nullptr;
    // Search for player in registered players list
    for (int i = 0; i < registeredPlayers.getSize(); i++) {
        Player* currentPlayer = registeredPlayers.get(i);
        if (currentPlayer && currentPlayer->id == playerId) {
            found = currentPlayer;
            break;
        }
    }
    if (found) {
        std::cout << "Checking in player: " << found->name << " (ID: " << playerId << ")\n";
        // Determine priority based on player flags
        int priority;
        if (found->isEarlyBird) {
            priority = 1; // Highest priority
            std::cout << "Early bird player checked in with highest priority!\n";
        } else if (found->isWildcard) {
            priority = 3; // Wildcard priority
            std::cout << "Wildcard player checked in with wildcard priority!\n";
        } else if (found->isLate) {
            priority = 4; // Lowest priority
            std::cout << "Late registration player checked in with low priority.\n";
        } else {
            priority = 2; // Regular priority
            std::cout << "Regular player checked in with normal priority.\n";
        }
        
        checkInQueue.enqueue(*found, priority);
        std::cout << "Player " << found->name << " (ID: " << playerId << ") checked in successfully!\n";

        // remove from registered players list
        for (int i = 0; i < registeredPlayers.getSize(); i++) {
            Player* currentPlayer = registeredPlayers.get(i);
            if (currentPlayer && currentPlayer->id == playerId) {
                registeredPlayers.removeAt(i);
                registrationCount--;
                break;
            }
        }
        std::cout << "Player " << found->name << " has been removed from registered players list after check-in.\n";
        // save registered players immediately after check-in
        saveCurrentRegisteredPlayers();
        
        // Save check-in queue immediately after successful check-in
        saveCurrentCheckInQueue();
    } else {
        std::cout << "Player with ID " << playerId << " not found in registered players.\n";
    }
}

void PlayerRegistration::checkInAllPlayers() {
    if (registeredPlayers.getSize() == 0) {
        std::cout << "No registered players available for check-in.\n";
        return;
    }
    
    std::cout << "Checking in all registered players...\n";
    
    for (int i = 0; i < registeredPlayers.getSize(); i++) {
        Player* currentPlayer = registeredPlayers.get(i);
        if (currentPlayer) {
            // Determine priority based on player flags
            int priority;
            if (currentPlayer->isEarlyBird) {
                priority = 1; // Highest priority
            } else if (currentPlayer->isWildcard) {
                priority = 3; // Wildcard priority
            } else if (currentPlayer->isLate) {
                priority = 4; // Lowest priority
            } else {
                priority = 2; // Regular priority
            }
            
            checkInQueue.enqueue(*currentPlayer, priority);
            std::cout << "Checked in: " << currentPlayer->name << " (Priority: " << priority << ")\n";
        }
    }
    
    std::cout << "All players checked in successfully!\n";
    
    // Save check-in queue immediately after successful batch check-in
    saveCurrentCheckInQueue();
}

void PlayerRegistration::displayRegisteredPlayers() {
    if (registeredPlayers.getSize() == 0) {
        std::cout << "No registered players.\n";
        return;
    }
    
    std::cout << "\n=== REGISTERED PLAYERS ===\n";
    for (int i = 0; i < registeredPlayers.getSize(); i++) {
        Player* player = registeredPlayers.get(i);
        if (player) {
            std::cout << "Player ID: " << player->id
                      << ", Name: " << player->name
                      << ", Age: " << player->age
                      << ", Gender: " << (player->gender == Gender::Male ? "Male" : "Female")
                      << ", Early Bird: " << (player->isEarlyBird ? "Yes" : "No")
                      << ", Wildcard: " << (player->isWildcard ? "Yes" : "No")
                      << ", Late Registration: " << (player->isLate ? "Yes" : "No")
                      << std::endl;
        }
    }
    std::cout << "Total registered players: " << registeredPlayers.getSize() << std::endl;
}

void PlayerRegistration::displayCheckInQueue() {
    if (checkInQueue.isEmpty()) {
        std::cout << "Check-in queue is empty.\n";
        return;
    }
    std::cout << "\n=== CHECK-IN QUEUE ===\n";
    checkInQueue.display();
}

int PlayerRegistration::getTotalCheckedInPlayers() const {
    return checkInQueue.getSize();
}

PriorityQueue<Player> PlayerRegistration::getCheckInQueue() const {
    return checkInQueue;
}

void PlayerRegistration::displayAllPlayersInDatabase() {
    std::cout << "\n=== DISPLAYING ALL PLAYERS IN DATABASE ===\n";
    playerManager.displayAllPlayers();
}

void PlayerRegistration::saveCurrentRegisteredPlayers() {
    if (registeredPlayers.getSize() == 0) {
        std::cout << "No registered players to save.\n";
        clearRegisteredPlayers();
        return;
    }
    
    std::cout << "Saving current registered players to database...\n";
    
    // Use writeAllPlayers to replace the entire file content (avoid duplicates)
    if (JsonWriter::writeAllPlayers(registeredPlayers, "data/temp_registered_players.json")) {
        std::cout << "Successfully saved " << registeredPlayers.getSize() << " registered players to temporary file.\n";
    } else {
        std::cout << "Failed to save registered players.\n";
    }
}

void PlayerRegistration::saveCurrentCheckInQueue() {
    if (checkInQueue.isEmpty()) {
        std::cout << "Check-in queue is empty. Nothing to save.\n";
        clearCheckInQueue();
        return;
    }
    
    std::cout << "Saving current check-in queue to database...\n";

    JsonWriter::writeAllCheckedInPlayer(checkInQueue, "data/temp_checkin_queue.json");
}

void PlayerRegistration::clearRegisteredPlayers() {   
    int playerCount = registeredPlayers.getSize();
    registeredPlayers.clear();
    registrationCount = 0;
    
    // Update tournament participant count to zero
    if (tournamentManager.hasRegisteringTournament()) {
        Tournament* tournament = tournamentManager.getRegisteringTournament();
        if (tournament) {
            // Reset participant count to zero
            for (int i = 0; i < playerCount; i++) {
                tournamentManager.decrementParticipantCount();
            }
        }
    }
    
    // Clear the temporary registered players file
    DoublyLinkedList<Player> emptyList;
    JsonWriter::writeAllPlayers(emptyList, "data/temp_registered_players.json");
    
    std::cout << "Cleared " << playerCount << " registered players.\n";
    std::cout << "Tournament participant count has been reset.\n";
}

void PlayerRegistration::clearCheckInQueue() {
    int queueSize = checkInQueue.getSize();
    checkInQueue.clear();
    
    // Clear the temporary check-in queue file
    PriorityQueue<Player> emptyQueue;
    JsonWriter::writeAllCheckedInPlayer(emptyQueue, "data/temp_checkin_queue.json");
    
    std::cout << "Cleared " << queueSize << " players from check-in queue.\n";
}

void PlayerRegistration::clearAll() {
    std::cout << "\n=== CLEARING ALL REGISTRATION DATA ===\n";
    
    int totalRegistered = registeredPlayers.getSize();
    int totalCheckedIn = checkInQueue.getSize();
    
    // Clear both lists
    clearRegisteredPlayers();
    clearCheckInQueue();
    
    std::cout << "\n=== CLEAR OPERATION COMPLETE ===\n";
    std::cout << "Total registered players cleared: " << totalRegistered << std::endl;
    std::cout << "Total check-in queue entries cleared: " << totalCheckedIn << std::endl;
    std::cout << "All registration data has been reset.\n";
}

std::string PlayerRegistration::getCurrentDate() {
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    char buffer[11]; // YYYY-MM-DD format
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", localTime);
    return std::string(buffer);
}

bool PlayerRegistration::isPlayerRegistered(const std::string& playerId) const {
    for (int i = 0; i < registeredPlayers.getSize(); i++) {
        Player* player = registeredPlayers.get(i);
        if (player && player->id == playerId) {
            return true; // Player is registered
        }
    }
    return false; // Player not found in registered players
}