#include "helper/JsonWriter.hpp"
#include "helper/JsonLoader.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <filesystem>

using json = nlohmann::json;

// Player operations
bool JsonWriter::appendPlayer(const Player& player, const std::string& filename) {
    if (!createFileIfNotExists(filename)) {
        return false;
    }
    
    // Load existing data
    DoublyLinkedList<Player> existingPlayers = JsonLoader::loadPlayers(filename);
    
    // Check if player already exists (by ID)
    for (int i = 0; i < existingPlayers.getSize(); i++) {
        Player* existingPlayer = existingPlayers.get(i);
        if (existingPlayer && existingPlayer->id == player.id) {
            std::cout << "Player with ID " << player.id << " already exists. Skipping." << std::endl;
            return false;
        }
    }
    
    // Add new player
    existingPlayers.append(player);
    
    // Write back to file
    return writeAllPlayers(existingPlayers, filename);
}

bool JsonWriter::writeAllCheckedInPlayer(PriorityQueue<Player>& checkInQueue, const std::string& filename) {
    json jsonArray = json::array();
    
    // Get all items from the queue with their priorities
    int queueSize = checkInQueue.getSize();
    if (queueSize == 0) {
        // Create empty file if queue is empty
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open file for writing: " << filename << std::endl;
            return false;
        }
        file << "[]";
        file.close();
        std::cout << "Created empty check-in queue file: " << filename << std::endl;
        return true;
    }
    
    Player* players = new Player[queueSize];
    int* priorities = new int[queueSize];
    int count = 0;
    
    checkInQueue.getAllItemsWithPriority(players, priorities, count);
    
    // Convert to JSON
    for (int i = 0; i < count; i++) {
        json playerJson = playerToJson(players[i]);
        playerJson["priority"] = priorities[i];
        jsonArray.push_back(playerJson);
    }
    
    // Clean up dynamic arrays
    delete[] players;
    delete[] priorities;
    
    // Write to file (replace entire content)
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return false;
    }
    
    file << jsonArray.dump(4); // Pretty print with 4 spaces
    file.close();
    
    std::cout << "Successfully wrote " << jsonArray.size() << " checked-in players to " << filename << std::endl;
    return true;
}

bool JsonWriter::writeAllPlayers(const DoublyLinkedList<Player>& players, const std::string& filename) {
    json jsonArray = json::array();
    
    for (int i = 0; i < players.getSize(); i++) {
        Player* player = players.get(i);
        if (player) {
            jsonArray.push_back(playerToJson(*player));
        }
    }
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return false;
    }
    
    file << jsonArray.dump(4); // Pretty print with 4 spaces
    file.close();
    
    std::cout << "Successfully wrote " << players.getSize() << " players to " << filename << std::endl;
    return true;
}

// Match operations
bool JsonWriter::appendMatch(const Match& match, const std::string& filename) {
    if (!createFileIfNotExists(filename)) {
        return false;
    }
    
    DoublyLinkedList<Match> existingMatches = JsonLoader::loadMatches(filename);
    existingMatches.append(match);
    return writeAllMatches(existingMatches, filename);
}

bool JsonWriter::writeAllMatches(const DoublyLinkedList<Match>& matches, const std::string& filename) {
    json jsonArray = json::array();
    
    for (int i = 0; i < matches.getSize(); i++) {
        Match* match = matches.get(i);
        if (match) {
            jsonArray.push_back(matchToJson(*match));
        }
    }
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return false;
    }
    
    file << jsonArray.dump(4);
    file.close();
    
    std::cout << "Successfully wrote " << matches.getSize() << " matches to " << filename << std::endl;
    return true;
}

// Performance operations
bool JsonWriter::appendPerformance(const Performance& performance, const std::string& filename) {
    if (!createFileIfNotExists(filename)) {
        return false;
    }
    
    DoublyLinkedList<Performance> existingPerformances = JsonLoader::loadPerformances(filename);
    existingPerformances.append(performance);
    return writeAllPerformances(existingPerformances, filename);
}

bool JsonWriter::writeAllPerformances(const DoublyLinkedList<Performance>& performances, const std::string& filename) {
    json jsonArray = json::array();
    
    for (int i = 0; i < performances.getSize(); i++) {
        Performance* performance = performances.get(i);
        if (performance) {
            jsonArray.push_back(performanceToJson(*performance));
        }
    }
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return false;
    }
    
    file << jsonArray.dump(4);
    file.close();
    
    std::cout << "Successfully wrote " << performances.getSize() << " performances to " << filename << std::endl;
    return true;
}

// Result operations
bool JsonWriter::appendResult(const Result& result, const std::string& filename) {
    if (!createFileIfNotExists(filename)) {
        return false;
    }
    
    DoublyLinkedList<Result> existingResults = JsonLoader::loadResults(filename);
    existingResults.append(result);
    return writeAllResults(existingResults, filename);
}

bool JsonWriter::writeAllResults(const DoublyLinkedList<Result>& results, const std::string& filename) {
    json jsonArray = json::array();
    
    for (int i = 0; i < results.getSize(); i++) {
        Result* result = results.get(i);
        if (result) {
            jsonArray.push_back(resultToJson(*result));
        }
    }
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return false;
    }
    
    file << jsonArray.dump(4);
    file.close();
    
    std::cout << "Successfully wrote " << results.getSize() << " results to " << filename << std::endl;
    return true;
}

// Spectator operations
bool JsonWriter::appendSpectator(const Spectator& spectator, const std::string& filename) {
    if (!createFileIfNotExists(filename)) {
        return false;
    }
    
    DoublyLinkedList<Spectator> existingSpectators = JsonLoader::loadSpectators(filename);
    existingSpectators.append(spectator);
    return writeAllSpectators(existingSpectators, filename);
}

bool JsonWriter::writeAllSpectators(const DoublyLinkedList<Spectator>& spectators, const std::string& filename) {
    json jsonArray = json::array();
    
    for (int i = 0; i < spectators.getSize(); i++) {
        Spectator* spectator = spectators.get(i);
        if (spectator) {
            jsonArray.push_back(spectatorToJson(*spectator));
        }
    }
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return false;
    }
    
    file << jsonArray.dump(4);
    file.close();
    
    std::cout << "Successfully wrote " << spectators.getSize() << " spectators to " << filename << std::endl;
    return true;
}

// Tournament operations
bool JsonWriter::appendTournament(const Tournament& tournament, const std::string& filename) {
    if (!createFileIfNotExists(filename)) {
        return false;
    }
    
    DoublyLinkedList<Tournament> existingTournaments = JsonLoader::loadTournaments(filename);
    existingTournaments.append(tournament);
    return writeAllTournaments(existingTournaments, filename);
}

bool JsonWriter::writeAllTournaments(const DoublyLinkedList<Tournament>& tournaments, const std::string& filename) {
    if (!createFileIfNotExists(filename)) {
        return false;
    }
    
    json jsonArray = json::array();
    
    for (int i = 0; i < tournaments.getSize(); i++) {
        Tournament* tournament = tournaments.get(i);
        if (tournament) {
            jsonArray.push_back(tournamentToJson(*tournament));
        }
    }
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return false;
    }
    
    file << jsonArray.dump(4);
    file.close();
    
    std::cout << "Successfully wrote " << tournaments.getSize() << " tournaments to " << filename << std::endl;
    return true;
}

// Helper functions
bool JsonWriter::createFileIfNotExists(const std::string& filename) {
    // Create directory if it doesn't exist
    std::filesystem::path filePath(filename);
    std::filesystem::path directory = filePath.parent_path();
    
    if (!directory.empty() && !std::filesystem::exists(directory)) {
        if (!std::filesystem::create_directories(directory)) {
            std::cerr << "Failed to create directory: " << directory << std::endl;
            return false;
        }
    }
    
    // Create file if it doesn't exist
    std::ifstream checkFile(filename);
    if (!checkFile.is_open()) {
        std::ofstream createFile(filename);
        if (!createFile.is_open()) {
            std::cerr << "Failed to create file: " << filename << std::endl;
            return false;
        }
        createFile << "[]"; // Create empty JSON array
        createFile.close();
        std::cout << "Created new file: " << filename << std::endl;
    }
    checkFile.close();
    
    return true;
}

bool JsonWriter::createDirectoryIfNotExists(const std::string& path) {
    if (!std::filesystem::exists(path)) {
        if (!std::filesystem::create_directories(path)) {
            std::cerr << "Failed to create directory: " << path << std::endl;
            return false;
        }
        std::cout << "Created directory: " << path << std::endl;
    }
    return true;
}

// Conversion functions
json JsonWriter::playerToJson(const Player& player) {
    return json{
        {"id", player.id},
        {"name", player.name},
        {"age", player.age},
        {"gender", genderToString(player.gender)},
        {"email", player.email},
        {"phoneNum", player.phoneNum},
        {"points", player.points},
        {"isEarlyBird", player.isEarlyBird},
        {"isWildcard", player.isWildcard},
        {"isLate", player.isLate},
        {"dateJoined", player.dateJoined},
        {"performanceId", player.performanceId}
    };
}

json JsonWriter::matchToJson(const Match& match) {
    return json{
        {"id", match.id},
        {"tournamentId", match.tournamentId},
        {"stage", tournamentStageToString(match.stage)},
        {"matchType", matchTypeToString(match.matchType)},
        {"date", match.date},
        {"time", match.time},
        {"player1", match.player1},
        {"player2", match.player2},
        {"resultId", match.resultId}
    };
}

json JsonWriter::performanceToJson(const Performance& performance) {
    return json{
        {"playerId", performance.playerId},
        {"matchesPlayed", performance.matchesPlayed},
        {"favouriteChampion", performance.favouriteChampion},
        {"lastWin", performance.lastWin},
        {"winRate", performance.winRate}
    };
}

json JsonWriter::resultToJson(const Result& result) {
    json championsP1Array = json::array();
    json championsP2Array = json::array();
    
    for (int i = 0; i < Result::TEAM_SIZE; i++) {
        championsP1Array.push_back(championToString(result.championsP1[i]));
        championsP2Array.push_back(championToString(result.championsP2[i]));
    }
    
    return json{
        {"id", result.id},
        {"matchId", result.matchId},
        {"gamesPlayed", result.gamesPlayed},
        {"score", result.score},
        {"championsP1", championsP1Array},
        {"championsP2", championsP2Array},
        {"winnerId", result.winnerId}
    };
}

json JsonWriter::spectatorToJson(const Spectator& spectator) {
    return json{
        {"id", spectator.id},
        {"name", spectator.name},
        {"gender", genderToString(spectator.gender)},
        {"email", spectator.email},
        {"phoneNum", spectator.phoneNum},
        {"type", spectatorTypeToString(spectator.type)},
        {"affiliation", spectator.affiliation}
    };
}

json JsonWriter::tournamentToJson(const Tournament& tournament) {
    return json{
        {"id", tournament.id},
        {"name", tournament.name},
        {"startDate", tournament.startDate},
        {"endDate", tournament.endDate},
        {"location", tournament.location},
        {"stage", tournamentStageToString(tournament.stage)},
        {"category", tournamentCategoryToString(tournament.category)},
        {"maxParticipants", tournament.maxParticipants},
        {"currentParticipants", tournament.currentParticipants},
        {"prizePool", tournament.prizePool}
    };
}

// String conversion helpers
std::string JsonWriter::championToString(Champion champion) {
    switch (champion) {
        case Champion::Akali: return "Akali";
        case Champion::Ezreal: return "Ezreal";
        case Champion::Fiora: return "Fiora";
        case Champion::Fizz: return "Fizz";
        case Champion::Garen: return "Garen";
        case Champion::Jax: return "Jax";
        case Champion::Katarina: return "Katarina";
        case Champion::LeeSin: return "LeeSin";
        case Champion::Riven: return "Riven";
        case Champion::Yasuo: return "Yasuo";
        case Champion::Zed: return "Zed";
        case Champion::NoChampion: return "NoChampion";
        default: return "NoChampion";
    }
}

std::string JsonWriter::genderToString(Gender gender) {
    switch (gender) {
        case Gender::Male: return "Male";
        case Gender::Female: return "Female";
        default: return "Male";
    }
}

std::string JsonWriter::spectatorTypeToString(SpectatorType type) {
    switch (type) {
        case SpectatorType::Normal: return "Normal";
        case SpectatorType::VIP: return "VIP";
        case SpectatorType::Streamer: return "Streamer";
        case SpectatorType::Influencer: return "Influencer";
        case SpectatorType::Player: return "Player";
        default: return "Normal";
    }
}

std::string JsonWriter::matchTypeToString(MatchType type) {
    switch (type) {
        case MatchType::BestOf1: return "BestOf1";
        case MatchType::BestOf3: return "BestOf3";
        case MatchType::BestOf5: return "BestOf5";
        default: return "BestOf1";
    }
}

std::string JsonWriter::tournamentStageToString(TournamentStage stage) {
    switch (stage) {
        case TournamentStage::Registration: return "Registration";
        case TournamentStage::Qualifiers: return "Qualifiers";
        case TournamentStage::GroupStage: return "GroupStage";
        case TournamentStage::KnockoutStage: return "KnockoutStage";
        case TournamentStage::Quarterfinals: return "Quarterfinals";
        case TournamentStage::Semifinals: return "Semifinals";
        case TournamentStage::Finals: return "Finals";
        case TournamentStage::Completed: return "Completed";
        default: return "Registration";
    }
}

std::string JsonWriter::tournamentCategoryToString(TournamentCategory category) {
    switch (category) {
        case TournamentCategory::Local: return "Local";
        case TournamentCategory::Regional: return "Regional";
        case TournamentCategory::National: return "National";
        case TournamentCategory::International: return "International";
        default: return "Regional";
    }
}
