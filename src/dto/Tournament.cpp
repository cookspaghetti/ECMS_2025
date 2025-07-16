#include "Tournament.hpp"

Tournament::Tournament()
    : id(0), name(""), startDate(""), endDate(""),
      location(""), category(TournamentCategory::Local) {}

Tournament::Tournament(int id, const std::string& name, const std::string& startDate,
                       const std::string& endDate, const std::string& location,
                       TournamentCategory category)
    : id(id), name(name), startDate(startDate), endDate(endDate),
      location(location), category(category) {}
