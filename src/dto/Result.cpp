#include "Result.hpp"

Result::Result() : id(0), matchId(0), gamesPlayed(0), score(""), championsP1(""), championsP2(""), winnerId(0) {}

Result::Result(int id, int matchId, int gamesPlayed, const std::string& score,
               const std::string& championsP1, const std::string& championsP2, int winnerId)
    : id(id), matchId(matchId), gamesPlayed(gamesPlayed), score(score),
      championsP1(championsP1), championsP2(championsP2), winnerId(winnerId) {}
