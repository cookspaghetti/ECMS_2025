#include "Performance.hpp"

Performance::Performance() : playerId(0), matchesPlayed(0), favouriteChampion(Champion::Ahri), lastWin(""), winRate(0.0f) {}

Performance::Performance(int playerId, int matchesPlayed, Champion favouriteChampion,
                         const std::string& lastWin, float winRate)
    : playerId(playerId), matchesPlayed(matchesPlayed), favouriteChampion(favouriteChampion),
      lastWin(lastWin), winRate(winRate) {}
