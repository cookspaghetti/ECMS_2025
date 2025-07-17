#ifndef PERFORMANCE_HPP
#define PERFORMANCE_HPP

#include <string>
#include "general/Enum.hpp"

class Performance
{
public:
    int playerId;
    int matchesPlayed;
    Champion favouriteChampion;
    std::string lastWin; // Date of last win in format YYYY-MM-DD
    float winRate;

    Performance() : playerId(0), matchesPlayed(0), favouriteChampion(Champion::NoChampion), lastWin(""), winRate(0.0f) {}
    Performance(int playerId, int matchesPlayed, Champion favouriteChampion, const std::string& lastWin, float winRate)
        : playerId(playerId), matchesPlayed(matchesPlayed), favouriteChampion(favouriteChampion), lastWin(lastWin), winRate(winRate) {}
};

#endif
