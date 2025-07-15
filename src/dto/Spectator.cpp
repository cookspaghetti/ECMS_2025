#include "Spectator.hpp"

Spectator::Spectator() : id(0), name(""), gender('O'), email(""), phoneNum(""),
                         type(""), affiliation("") {}

Spectator::Spectator(int id, const std::string& name, char gender, const std::string& email,
                     const std::string& phoneNum, const std::string& type, const std::string& affiliation)
    : id(id), name(name), gender(gender), email(email), phoneNum(phoneNum),
      type(type), affiliation(affiliation) {}
