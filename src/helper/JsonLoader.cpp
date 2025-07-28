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
            item["isWildcard"],
            item["dateJoined"],
            item["performanceId"]
        );
        list.append(player);
    }
    return list;
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
        if (stageStr == "GroupStage") stage = TournamentStage::GroupStage;
        else if (stageStr == "KnockoutStage") stage = TournamentStage::KnockoutStage;
        else if (stageStr == "Quarterfinals") stage = TournamentStage::Quarterfinals;
        else if (stageStr == "Semifinals") stage = TournamentStage::Semifinals;
        else if (stageStr == "Finals") stage = TournamentStage::Finals;

        // Parse MatchType from string
        MatchType matchType = MatchType::BestOf1;
        std::string matchTypeStr = item["matchType"];
        if (matchTypeStr == "BestOf3") matchType = MatchType::BestOf3;
        else if (matchTypeStr == "BestOf5") matchType = MatchType::BestOf5;

        Match match(
            item["id"],
            item["tournamentId"],
            stage,
            matchType,
            item["date"],
            item["time"],
            item["player1"],
            item["player2"],
            item["resultId"]
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
        Champion championsP1[Result::TEAM_SIZE];
        Champion championsP2[Result::TEAM_SIZE];

        const auto& jsonP1 = item["championsP1"];
        const auto& jsonP2 = item["championsP2"];

        for (int i = 0; i < Result::TEAM_SIZE; ++i) {
            championsP1[i] = championFromString(jsonP1.at(i));
            championsP2[i] = championFromString(jsonP2.at(i));
        }

        Result result(
            item["id"],
            item["matchId"],
            item["gamesPlayed"],
            item["score"],
            championsP1,
            championsP2,
            item["winnerId"]
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
        // Parse TournamentStage from string
        TournamentCategory category = TournamentCategory::Local;
        std::string categoryStr = item["category"];
        if (categoryStr == "Regional") category = TournamentCategory::Regional;
        else if (categoryStr == "International") category = TournamentCategory::International;

        // Parse TournamentStage from string
        TournamentStage stage = TournamentStage::Qualifiers;
        std::string stageStr = item["stage"];
        if (stageStr == "GroupStage") stage = TournamentStage::GroupStage;
        else if (stageStr == "KnockoutStage") stage = TournamentStage::KnockoutStage;
        else if (stageStr == "Quarterfinals") stage = TournamentStage::Quarterfinals;
        else if (stageStr == "Semifinals") stage = TournamentStage::Semifinals;
        else if (stageStr == "Finals") stage = TournamentStage::Finals;

        Tournament t(
            item["id"],
            item["name"],
            item["startDate"],
            item["endDate"],
            item["location"],
            stage,
            category,
            item["tournamentSize"]
        );
        list.append(t);
    }
    return list;
}
