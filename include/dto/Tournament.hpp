#ifndef TOURNAMENT_HPP
#define TOURNAMENT_HPP

#include <string>
#include "general/Enum.hpp"

class Tournament {
public:
    int id;
    std::string name;
    std::string startDate;
    std::string endDate;
    std::string location;
    TournamentStage stage;
    TournamentCategory category;
    int tournamentSize;

    Tournament() : id(0), name(""), startDate(""), endDate(""), location(""), stage(TournamentStage::Qualifiers), category(TournamentCategory::Local), tournamentSize(0) {}
    Tournament(int id, const std::string& name, const std::string& startDate, const std::string& endDate, const std::string& location, TournamentStage stage, TournamentCategory category, int tournamentSize)
        : id(id), name(name), startDate(startDate), endDate(endDate), location(location), stage(stage), category(category), tournamentSize(tournamentSize) {}
};

inline std::string toString(TournamentCategory category) {
    switch (category) {
        case TournamentCategory::Local: return "Local";
        case TournamentCategory::Regional: return "Regional";
        case TournamentCategory::International: return "International";
        default: return "Unknown";
    }
}

inline std::string toString(TournamentStage stage) {
    switch (stage) {
        case TournamentStage::Qualifiers: return "Qualifiers";
        case TournamentStage::GroupStage: return "Group Stage";
        case TournamentStage::KnockoutStage: return "Knockout Stage";
        case TournamentStage::Quarterfinals: return "Quarterfinals";
        case TournamentStage::Semifinals: return "Semifinals";
        case TournamentStage::Finals: return "Finals";
        default: return "Unknown";
    }
}

#endif
