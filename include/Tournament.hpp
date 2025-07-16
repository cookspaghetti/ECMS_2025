#ifndef TOURNAMENT_HPP
#define TOURNAMENT_HPP

#include <string>
#include "Enum.hpp"

class Tournament {
public:
    int id;
    std::string name;
    std::string startDate;
    std::string endDate;
    std::string location;
    TournamentState state;
    TournamentCategory category;

    Tournament() : id(0), name(""), startDate(""), endDate(""), location(""), state(TournamentState::Qualifiers), category(TournamentCategory::Local) {}
    Tournament(int id, const std::string& name, const std::string& startDate, const std::string& endDate, const std::string& location, TournamentState state, TournamentCategory category)
        : id(id), name(name), startDate(startDate), endDate(endDate), location(location), state(state), category(category) {}
};

std::string toString(TournamentCategory category) {
    switch (category) {
        case TournamentCategory::Local: return "Local";
        case TournamentCategory::Regional: return "Regional";
        case TournamentCategory::International: return "International";
        default: return "Unknown";
    }
}

#endif
