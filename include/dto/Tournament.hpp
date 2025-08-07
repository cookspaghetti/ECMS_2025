#ifndef TOURNAMENT_HPP
#define TOURNAMENT_HPP

#include <string>
#include "general/Enum.hpp"

class Tournament {
public:
    std::string id;
    std::string name;
    std::string startDate;
    std::string endDate;
    std::string location;
    TournamentStage stage;
    TournamentCategory category;
    int maxParticipants;  // Maximum number of participants
    int currentParticipants;  // Current number of registered participants
    double prizePool;  // Prize pool amount

    Tournament() : id(""), name(""), startDate(""), endDate(""), location(""), 
                  stage(TournamentStage::Registration), category(TournamentCategory::Regional), 
                   maxParticipants(0), currentParticipants(0), prizePool(0.0) {}

    Tournament(const std::string& id, const std::string& name, const std::string& startDate, const std::string& endDate, const std::string& location, 
               TournamentStage stage, TournamentCategory category, 
               int maxParticipants, int currentParticipants = 0, double prizePool = 0.0)
        : id(id), name(name), startDate(startDate), endDate(endDate), location(location), 
          stage(stage), category(category), maxParticipants(maxParticipants), 
          currentParticipants(currentParticipants), prizePool(prizePool) {}
};

inline std::string toString(TournamentCategory category) {
    switch (category) {
        case TournamentCategory::Local: return "Local";
        case TournamentCategory::Regional: return "Regional";
        case TournamentCategory::National: return "National";
        case TournamentCategory::International: return "International";
        default: return "Regional";  // Default to Regional if unknown
    }
}

inline std::string toString(TournamentStage stage) {
    switch (stage) {
        case TournamentStage::Registration: return "Registration";
        case TournamentStage::Qualifiers: return "Qualifiers";
        case TournamentStage::Quarterfinals: return "Quarterfinals";
        case TournamentStage::Semifinals: return "Semifinals";
        case TournamentStage::Finals: return "Finals";
        case TournamentStage::Completed: return "Completed";
        default: return "Registration";
    }
}

#endif
