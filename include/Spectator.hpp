#ifndef SPECTATOR_HPP
#define SPECTATOR_HPP

#include <string>

enum class Gender {
    Male,
    Female
};

enum class SpectatorType {
    Normal,
    VIP,
    Streamer,
    Influencer,
    Player
};

class Spectator {
public:
    int id;
    std::string name;
    Gender gender;
    std::string email;
    std::string phoneNum;
    SpectatorType type;
    std::string affiliation; // Streaming Platform, Team, or Organization

    Spectator();
    Spectator(int id, const std::string& name, Gender gender, const std::string& email,
              const std::string& phoneNum, SpectatorType type, const std::string& affiliation);
};

std::string toString(SpectatorType type) {
    switch (type) {
        case SpectatorType::Normal: return "Normal";
        case SpectatorType::VIP: return "VIP";
        case SpectatorType::Streamer: return "Streamer";
        case SpectatorType::Influencer: return "Influencer";
        case SpectatorType::Player: return "Player";
        default: return "Unknown";
    }
}

#endif
