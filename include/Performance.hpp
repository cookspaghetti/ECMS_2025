#ifndef PERFORMANCE_HPP
#define PERFORMANCE_HPP

#include <string>

class Performance {
public:
    int playerId;
    int matchesPlayed;
    std::string favouriteChampion;
    std::string lastWin; // Date of last win in format YYYY-MM-DD
    float winRate;

    Performance();
    Performance(int playerId, int matchesPlayed, const std::string& favouriteChampion,
                const std::string& lastWin, float winRate);
};

#endif
