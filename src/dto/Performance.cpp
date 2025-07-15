#include "Performance.hpp"

Performance::Performance() : playerId(0), matchesPlayed(0), favouriteChampion(""),
                             lastWin(""), winRate(0.0f) {}

Performance::Performance(int playerId, int matchesPlayed, const std::string& favouriteChampion,
                         const std::string& lastWin, float winRate)
    : playerId(playerId), matchesPlayed(matchesPlayed), favouriteChampion(favouriteChampion),
      lastWin(lastWin), winRate(winRate) {}
