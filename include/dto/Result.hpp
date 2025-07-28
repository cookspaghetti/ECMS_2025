#ifndef RESULT_HPP
#define RESULT_HPP

#include <string>
#include "general/Enum.hpp"

class Result {
public:
    static const int TEAM_SIZE = 5;

    int id;
    int matchId;                            // ID of the associated match
    int gamesPlayed;                        // Number of games played in the match
    std::string score;                      // Format: "winner:loser"
    Champion championsP1[TEAM_SIZE];        // Champions used by player 1
    Champion championsP2[TEAM_SIZE];        // Champions used by player 2
    int winnerId;                           // ID of the winning player

    Result() : id(0), matchId(0), gamesPlayed(0), score(""), winnerId(0) {
        for (int i = 0; i < TEAM_SIZE; ++i) {
            championsP1[i] = Champion::NoChampion;
            championsP2[i] = Champion::NoChampion;
        }
    }
    
    Result(int id, int matchId, int gamesPlayed, const std::string& score,
           Champion championsP1[TEAM_SIZE], Champion championsP2[TEAM_SIZE], int winnerId)
        : id(id), matchId(matchId), gamesPlayed(gamesPlayed), score(score), winnerId(winnerId) {
        for (int i = 0; i < TEAM_SIZE; ++i) {
            this->championsP1[i] = championsP1[i];
            this->championsP2[i] = championsP2[i];
        }
    }
};

inline Champion championFromString(const std::string& str) {
    if (str == "Akali") return Champion::Akali;
    if (str == "Ezreal") return Champion::Ezreal;
    if (str == "Fiora") return Champion::Fiora;
    if (str == "Fizz") return Champion::Fizz;
    if (str == "Garen") return Champion::Garen;
    if (str == "Jax") return Champion::Jax;
    if (str == "Katarina") return Champion::Katarina;
    if (str == "LeeSin") return Champion::LeeSin;
    if (str == "Riven") return Champion::Riven;
    if (str == "Yasuo") return Champion::Yasuo;
    if (str == "Zed") return Champion::Zed;
    return Champion::NoChampion;
}

#endif
