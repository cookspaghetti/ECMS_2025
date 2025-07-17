#ifndef MATCH_HPP
#define MATCH_HPP

#include <string>
#include "general/Enum.hpp"

class Match
{
public:
    int id;
    int tournamentId; // pointer to Tournament
    TournamentStage stage;
    MatchType matchType;
    std::string date;
    std::string time;
    int player1;
    int player2;
    int resultId; // pointer to Result

    Match() : id(0), tournamentId(0), stage(TournamentStage::Qualifiers), matchType(MatchType::BestOf1), date(""), time(""), player1(0), player2(0), resultId(0) {}
    Match(int id, int tournamentId, TournamentStage stage, MatchType matchType, const std::string &date, const std::string &time, int player1, int player2, int resultId)
        : id(id), tournamentId(tournamentId), stage(stage), matchType(matchType), date(date), time(time), player1(player1), player2(player2), resultId(resultId) {}
};

#endif
