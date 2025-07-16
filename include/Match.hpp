#ifndef MATCH_HPP
#define MATCH_HPP

#include <string>

enum class MatchType {
    BestOf1,
    BestOf3,
    BestOf5
};

class Match {
public:
    int id;
    int tournamentId; // pointer to Tournament
    MatchType matchType;
    std::string date;
    std::string time;
    int player1;
    int player2;
    int resultId; // pointer to Result

    Match();
    Match(int id, int tournamentId, MatchType matchType, const std::string& date, const std::string& time, int player1, int player2, int resultId)
        : id(id), tournamentId(tournamentId), matchType(matchType), date(date), time(time), player1(player1), player2(player2), resultId(resultId) {}
};

#endif
