#ifndef TOURNAMENT_HPP
#define TOURNAMENT_HPP

#include <string>

class Tournament {
public:
    int id;
    std::string date;
    std::string category; // Local, Regional, International

    Tournament();
    Tournament(int id, const std::string& date, const std::string& category);
};

#endif
