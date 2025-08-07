#include "helper/JsonLoader.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

DoublyLinkedList<Player> JsonLoader::loadPlayers(const std::string& filename) {
    DoublyLinkedList<Player> list;
    std::ifstream file(filename);
    if (!file.is_open()) return list;

    json data;
    file >> data;

    for (const auto& item : data) {
        // Parse Gender
        Gender gender = item["gender"] == "Male" ? Gender::Male : Gender::Female;

        Player player(
            item["id"],
            item["name"],
            item["age"],
            gender,
            item["email"],
            item["phoneNum"],
            item["points"],
            item["isEarlyBird"],
            item["isWildcard"],
            item["isLate"],
            item["dateJoined"]
        );

        std::cout << "Loaded player: " << player.name << " (ID: " << player.id << ")" << std::endl;
        list.append(player);
    }
    return list;
}

PriorityQueue<Player> JsonLoader::loadCheckedInPlayers(const std::string& filename) {
    PriorityQueue<Player> queue;
    std::ifstream file(filename);
    if (!file.is_open()) return queue;

    json data;
    file >> data;
    for (const auto& item : data) {
        Player player(
            item["id"],
            item["name"],
            item["age"],
            item["gender"] == "Male" ? Gender::Male : Gender::Female,
            item["email"],
            item["phoneNum"],
            item["points"],
            item["isEarlyBird"],
            item["isWildcard"],
            item["isLate"],
            item["dateJoined"]
        );
        queue.enqueue(player, item["priority"]);
    }
    return queue;
}

DoublyLinkedList<Match> JsonLoader::loadMatches(const std::string& filename) {
    DoublyLinkedList<Match> list;
    std::ifstream file(filename);
    if (!file.is_open()) return list;

    json data;
    file >> data;

    for (const auto& item : data) {
        // Parse TournamentStage from string
        TournamentStage stage = TournamentStage::Qualifiers;
        std::string stageStr = item["stage"];
        if (stageStr == "Registration") stage = TournamentStage::Registration;
        else if (stageStr == "Qualifiers") stage = TournamentStage::Qualifiers;
        else if (stageStr == "Tiebreakers") stage = TournamentStage::Tiebreakers;
        else if (stageStr == "Quarterfinals") stage = TournamentStage::Quarterfinals;
        else if (stageStr == "Semifinals") stage = TournamentStage::Semifinals;
        else if (stageStr == "Finals") stage = TournamentStage::Finals;

        Match match(
            item["id"],
            item["tournamentId"],
            stage,
            item["date"],
            item["time"],
            item["player1"],
            item["player2"]
        );
        list.append(match);
    }
    return list;
}

DoublyLinkedList<Performance> JsonLoader::loadPerformances(const std::string& filename) {
    DoublyLinkedList<Performance> list;
    std::ifstream file(filename);
    if (!file.is_open()) return list;

    json data;
    file >> data;

    for (const auto& item : data) {
        Performance perf(
            item["playerId"],
            item["matchPlayed"],
            item["favouriteChampion"],
            item["lastWin"],
            item["winRate"]
        );
        list.append(perf);
    }
    return list;
}

DoublyLinkedList<Result> JsonLoader::loadResults(const std::string& filename) {
    DoublyLinkedList<Result> list;
    std::ifstream file(filename);
    if (!file.is_open()) return list;

    json data;
    file >> data;

    for (const auto& item : data) {
        Champion champP1 = championFromString(item["championsP1"]);
        Champion champP2 = championFromString(item["championsP2"]);

        Result result(
            item["id"].get<std::string>(),
            item["matchId"].get<std::string>(),
            champP1,
            champP2,
            item["winnerId"].get<std::string>()
        );
        list.append(result);
    }

    return list;
}

DoublyLinkedList<Spectator> JsonLoader::loadSpectators(const std::string& filename) {
    DoublyLinkedList<Spectator> list;
    std::ifstream file(filename);
    if (!file.is_open()) return list;

    json data;
    file >> data;

    for (const auto& item : data) {
        // Parse SpectatorType from string
        SpectatorType type = SpectatorType::Normal;
        std::string typeStr = item["type"];
        if (typeStr == "VIP") type = SpectatorType::VIP;
        else if (typeStr == "Streamer") type = SpectatorType::Streamer;
        else if (typeStr == "Influencer") type = SpectatorType::Influencer;
        else if (typeStr == "Player") type = SpectatorType::Player;

        
        Spectator spec(
            item["id"],
            item["name"],
            item["gender"] == "Male" ? Gender::Male : Gender::Female,
            item["email"],
            item["phoneNum"],
            type,
            item["affiliation"]
        );
        list.append(spec);
    }
    return list;
}

DoublyLinkedList<Tournament> JsonLoader::loadTournaments(const std::string& filename) {
    DoublyLinkedList<Tournament> list;
    std::ifstream file(filename);
    if (!file.is_open()) return list;

    json data;
    file >> data;

    for (const auto& item : data) {
        // Parse TournamentCategory from string
        TournamentCategory category = TournamentCategory::Local;
        std::string categoryStr = item["category"];
        if (categoryStr == "Regional") category = TournamentCategory::Regional;
        else if (categoryStr == "International") category = TournamentCategory::International;

        // Parse TournamentStage from string
        TournamentStage stage = TournamentStage::Qualifiers;
        std::string stageStr = item["stage"];
        if (stageStr == "Registration") stage = TournamentStage::Registration;
        else if (stageStr == "Qualifiers") stage = TournamentStage::Qualifiers;
        else if (stageStr == "Tiebreakers") stage = TournamentStage::Tiebreakers;
        else if (stageStr == "Quarterfinals") stage = TournamentStage::Quarterfinals;
        else if (stageStr == "Semifinals") stage = TournamentStage::Semifinals;
        else if (stageStr == "Finals") stage = TournamentStage::Finals;
        else if (stageStr == "Completed") stage = TournamentStage::Completed;

        Tournament t(
            item["id"],
            item["name"],
            item["startDate"],
            item["endDate"],
            item["location"],
            stage,
            category,
            item["maxParticipants"],
            item["currentParticipants"],
            item["prizePool"]
        );
        list.append(t);
    }
    return list;
}
