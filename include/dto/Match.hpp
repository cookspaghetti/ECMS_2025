#ifndef MATCH_HPP
#define MATCH_HPP

#include <string>
#include "general/Enum.hpp"

class Match
{
public:
    std::string id;
    std::string tournamentId;
    TournamentStage stage;
    std::string date;
    std::string time;
    std::string player1;
    std::string player2;

    Match() : id(""), tournamentId(""), stage(TournamentStage::Qualifiers), date(""), time(""), player1(""), player2("") {}
    Match(const std::string &id, const std::string &tournamentId, TournamentStage stage, const std::string &date, const std::string &time, const std::string &player1, const std::string &player2)
        : id(id), tournamentId(tournamentId), stage(stage), date(date), time(time), player1(player1), player2(player2) {}
};

#endif
