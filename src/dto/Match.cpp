#include "Match.hpp"

Match::Match() : id(0), tournamentId(0), matchType(MatchType::BestOf1), date(""), time(""), player1(0), player2(0), resultId(0) {}

Match::Match(int id, int tournamentId, MatchType matchType, const std::string& date, const std::string& time, int player1, int player2, int resultId)
    : id(id), tournamentId(tournamentId), matchType(matchType), date(date), time(time), player1(player1), player2(player2), resultId(resultId) {}