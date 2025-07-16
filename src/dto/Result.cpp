#include "Result.hpp"

Result::Result() 
    : id(0), matchId(0), gamesPlayed(0), score(""), winnerId(0) {
    for (int i = 0; i < TEAM_SIZE; ++i) {
        championsP1[i] = Champion::NoChampion; // Default champion
        championsP2[i] = Champion::NoChampion; // Default champion
    }
}

Result::Result(int id, int matchId, int gamesPlayed, const std::string& score,
               const Champion championsP1[TEAM_SIZE], const Champion championsP2[TEAM_SIZE],
               int winnerId)
    : id(id), matchId(matchId), gamesPlayed(gamesPlayed), score(score), winnerId(winnerId) {
    for (int i = 0; i < TEAM_SIZE; ++i) {
        this->championsP1[i] = championsP1[i];
        this->championsP2[i] = championsP2[i];
    }
}
