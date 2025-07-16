#ifndef PERFORMANCE_HPP
#define PERFORMANCE_HPP

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

class Performance
{
public:
    int playerId;
    int matchesPlayed;
    Champion favouriteChampion;
    std::string lastWin; // Date of last win in format YYYY-MM-DD
    float winRate;

    Performance();
    Performance(int playerId, int matchesPlayed, Champion favouriteChampion,
               const std::string& lastWin, float winRate);
};

#endif
