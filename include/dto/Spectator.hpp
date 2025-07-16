#ifndef SPECTATOR_HPP
#define SPECTATOR_HPP

#include <string>
#include "general/Enum.hpp"

class Spectator
{
public:
    int id;
    std::string name;
    Gender gender;
    std::string email;
    std::string phoneNum;
    SpectatorType type;
    std::string affiliation; // Streaming Platform, Team, or Organization

    Spectator() : id(0), name(""), gender(Gender::Male), email(""), phoneNum(""), type(SpectatorType::Normal), affiliation("") {}
    Spectator(int id, const std::string &name, Gender gender, const std::string &email, const std::string &phoneNum, SpectatorType type, const std::string &affiliation) 
        : id(id), name(name), gender(gender), email(email), phoneNum(phoneNum), type(type), affiliation(affiliation) {}
};

std::string toString(SpectatorType type)
{
    switch (type)
    {
    case SpectatorType::Normal:
        return "Normal";
    case SpectatorType::VIP:
        return "VIP";
    case SpectatorType::Streamer:
        return "Streamer";
    case SpectatorType::Influencer:
        return "Influencer";
    case SpectatorType::Player:
        return "Player";
    default:
        return "Unknown";
    }
}

#endif
