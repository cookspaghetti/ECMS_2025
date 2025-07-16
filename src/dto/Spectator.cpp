#include "Spectator.hpp"

Spectator::Spectator()
    : id(0), name(""), gender(Gender::Male),
      email(""), phoneNum(""), type(SpectatorType::Normal),
      affiliation("") {}

Spectator::Spectator(int id, const std::string& name, Gender gender, const std::string& email,
                     const std::string& phoneNum, SpectatorType type, const std::string& affiliation)
    : id(id), name(name), gender(gender), email(email),
      phoneNum(phoneNum), type(type), affiliation(affiliation) {}
