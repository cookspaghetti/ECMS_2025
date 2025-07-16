#ifndef RESULT_HPP
#define RESULT_HPP

#include <string>

enum class Champion
{
    Ahri,
    Akali,
    Alistar,
    Ashe,
    Azir,
    Braum,
    Cassiopeia,
    Darius,
    Draven,
    Ezreal,
    Fiora,
    Fizz,
    Garen,
    Graves,
    Janna,
    Jax,
    Jhin,
    Jinx,
    Karma,
    Karthus,
    Katarina,
    KhaZix,
    LeeSin,
    Leona,
    Lissandra,
    Lulu,
    Lux,
    Malphite,
    MissFortune,
    Morgana,
    Nami,
    Nautilus,
    Orianna,
    Rengar,
    Riven,
    Ryze,
    Sona,
    Soraka,
    Syndra,
    Taric,
    Teemo,
    Thresh,
    TwistedFate,
    Twitch,
    Vayne,
    VelKoz,
    Viktor,
    Xerath,
    Yasuo,
    Zed,
    Zyra,
    NoChampion // Placeholder for no champion selected
};

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

    Result();
    Result(int id, int matchId, int gamesPlayed, const std::string& score,
           const Champion championsP1[TEAM_SIZE], const Champion championsP2[TEAM_SIZE],
           int winnerId);
};

#endif
