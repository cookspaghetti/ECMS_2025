#ifndef RESULT_HPP
#define RESULT_HPP

#include <string>

class Result {
public:
    int id;
    int matchId; // Pointer to Match
    int gamesPlayed; // Number of games played in the match
    std::string score; // Format: "winner:loser"
    std::string championsP1; // Champions used by player 1, comma-separated
    std::string championsP2; // Champions used by player 2, comma-separated
    int winnerId; // ID of the winning player

    Result();
    Result(int id, int matchId, int gamesPlayed, const std::string& score,
           const std::string& championsP1, const std::string& championsP2, int winnerId);
};

#endif
