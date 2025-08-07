#ifndef PERFORMANCE_HPP
#define PERFORMANCE_HPP

#include <string>
#include "general/Enum.hpp"

class Performance
{
public:
    std::string playerId;
    int matchesPlayed;
    Champion favouriteChampion;
    std::string lastWin;
    float winRate;

    Performance() : playerId(""), matchesPlayed(0), favouriteChampion(Champion::NoChampion), lastWin(""), winRate(0.0f) {}
    Performance(const std::string& playerId, int matchesPlayed, Champion favouriteChampion, const std::string& lastWin, float winRate)
        : playerId(playerId), matchesPlayed(matchesPlayed), favouriteChampion(favouriteChampion), lastWin(lastWin), winRate(winRate) {}
};

#endif
